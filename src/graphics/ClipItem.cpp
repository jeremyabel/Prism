#include <QDebug>
#include <QCursor>
#include <math.h>

#include "ClipItem.h"
#include "TrackItem.h"

const int       TRACK_HEIGHT         = 15;
const float     TRACK_DRAG_MIN_WIDTH = 9.0f;

ClipItem::ClipItem( ClipModel* clipModel, QGraphicsItem *parent ) : QGraphicsObject( parent )
{
    m_fWidth                = 0;
    m_bExtendLeft           = false;
    m_bExtendRight          = false;

    bDetached               = false;
    bMoved                  = false;
    bLocked                 = false;
    bMultiSelected          = false;
    pClipModel              = clipModel;
    masterInitStarting16ths = -1;

    setFlag( QGraphicsItem::ItemIsSelectable );
    setFlag( QGraphicsItem::ItemIsMovable );
    setFlag( QGraphicsItem::ItemSendsGeometryChanges );

    setAcceptHoverEvents( true );
}


QRectF ClipItem::boundingRect() const
{
    return QRectF( 0, 0, m_fWidth, TRACK_HEIGHT );
}


void ClipItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    m_fWidth = pClipModel->length16th * ( m_fSpacing / (float)m_iInvDivisions );

    // Draw clip rect
    painter->setPen( QPen( Qt::NoPen ) );
    painter->setBrush( QBrush( color.darker( isSelected() ? 100 : 130 ) ) );
    painter->drawRect( QRect( 0, 0, m_fWidth - 1, TRACK_HEIGHT ) );

    // Draw left dividing line
    painter->setBrush( Qt::NoBrush );
    painter->setPen( color.darker( isSelected() ? 100 : 140 ) );
    painter->drawLine( 0, 0, 0, TRACK_HEIGHT - 1 );

    // Draw right dividing line
    painter->setPen( color.darker( isSelected() ? 140 : 400 ) );
    painter->drawLine( m_fWidth - 1, 0, m_fWidth - 1, TRACK_HEIGHT - 1 );
    painter->setPen( color.darker(isSelected() ? 100 : 120 ) );
    painter->drawLine( m_fWidth - 2, 0, m_fWidth - 2, TRACK_HEIGHT - 1 );

    // Draw divisions if we're long enough
    if ( pClipModel->length16th + 1 > 16 / pClipModel->distro16th && pClipModel->distro16th <= m_iDivisions )
    {
        painter->setPen( color.lighter(isSelected() ? 140 : 130) );

        for ( int i = 1; i < pClipModel->length16th / (16 / pClipModel->distro16th); i++ )
        {
            float fLineX = (float)i * (m_fSpacing / (float)m_iInvDivisions * (float)(16 / pClipModel->distro16th) );
            painter->drawLine( fLineX, 0, fLineX, TRACK_HEIGHT - 1 );
        }
    }
}


void ClipItem::setZoomParams( float spacing, int divisions )
{
    m_fSpacing   = spacing;
    m_iDivisions = divisions;

    if ( m_iDivisions > 0 )
        m_iInvDivisions = 16 / m_iDivisions;

    m_fSpacing16ths = m_fSpacing / (float)m_iInvDivisions;

    setPos( calculateXPos(), 0 );
    prepareGeometryChange();
}


float ClipItem::calculateXPos()
{
    return 101.0f + (float)pClipModel->starting16th * m_fSpacing16ths;
}


void ClipItem::snapTo16ths( int posx )
{
    float fDeltaX           = m_clickCursorPos.x() - posx;
    int   iDeltaDivisions   = -(int)roundf( fDeltaX / m_fSpacing16ths );

    pClipModel->setStarting16th( clamp( m_clickStarting16ths + iDeltaDivisions, 0, INFINITY ), true );
}


void ClipItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    m_clickCursorPos =
    m_prevCursorPos  = QCursor::pos();
    bMoved = false;

    m_clickStarting16ths        = pClipModel->starting16th;
    m_clickEnding16ths          = pClipModel->ending16th;
    pClipModel->oldStarting16th = m_clickStarting16ths;
    pClipModel->oldEnding16th   = m_clickEnding16ths;

    QGraphicsItem::mousePressEvent( event );
    emit mouseDown( this );
}


void ClipItem::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    if ( ( m_bExtendLeft || m_bExtendRight ) && !bMultiSelected && !bLocked )
    {
        TrackItem* track     = dynamic_cast<TrackItem*>(parentItem());
        ClipModel* leftClip  = track->pTrackModel->getLeftClip(  pClipModel );
        ClipModel* rightClip = track->pTrackModel->getRightClip( pClipModel );

        // Clamp to end of left-most clip
        if ( leftClip && QCursor::pos().x() - m_prevCursorPos.x() < 0 )
        {
            if ( pClipModel->starting16th <= leftClip->ending16th )
            {
                pClipModel->starting16th = leftClip->ending16th;
                update();
                return;
            }
        }

        // Clamp to start of right-most clip
        if ( rightClip && QCursor::pos().x() - m_prevCursorPos.x() > 0 )
        {
            if ( pClipModel->ending16th >= rightClip->starting16th )
            {
                pClipModel->ending16th = rightClip->starting16th;
                update();
                return;
            }
        }

        // Determine how much to adjust size
        int deltaX      = QCursor::pos().x() - m_clickCursorPos.x();
        int delta16ths  = (int)roundf((float)deltaX / m_fSpacing16ths);

        if ( m_bExtendRight && pClipModel->length16th >= 1 && m_fWidth > TRACK_DRAG_MIN_WIDTH )
        {
            // Drag right side
            pClipModel->setEnding16th( m_clickEnding16ths + delta16ths );
            emit resized( this, RESIZE_RIGHT );
        }
        else if ( pClipModel->length16th >= 1 && m_fWidth > TRACK_DRAG_MIN_WIDTH )
        {
            // Drag left side
            snapTo16ths( QCursor::pos().x() );
            pClipModel->setEnding16th( m_clickEnding16ths );
            setX( calculateXPos() );
            emit resized( this, RESIZE_LEFT );
        }

        update();
    }
    else    // Normal drag behavior
    {
        QGraphicsItem::mouseMoveEvent( event );

        bMoved = true;

        // If it looks like the user is trying to drag the clip to another track, detach it.
        // Otherwise, keep it here and snap to 16ths.
        if ( ( abs( m_prevCursorPos.y()  - QCursor::pos().y() ) > 5 ||
               abs( m_clickCursorPos.y() - QCursor::pos().y() ) > 7 ) && !bDetached && !bLocked )
        {
            bDetached = true;
            this->setOpacity( 0.5f );
            emit detached();
        }
        else if ( !bDetached )
        {
            snapTo16ths( QCursor::pos().x() );
            setX( calculateXPos() );
            setY( 0 );
        }

        // Clamp
        setY( clamp( pos().y(), 0, INFINITY ) );
    }

    m_prevCursorPos = QCursor::pos();
}


void ClipItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    QGraphicsItem::mouseReleaseEvent( event );
    bDetached = false;

    this->setOpacity( 1.0f );

    // Snap to position
    if ( !m_bExtendLeft && !m_bExtendRight )
    {
        snapTo16ths( QCursor::pos().x() );
        setPos( calculateXPos(), 0 );
    }

    // Notify and reset
    emit mouseUp( this );
    bLocked         = false;
    m_bExtendLeft   = false;
    m_bExtendRight  = false;
}


void ClipItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    QGraphicsItem::mouseDoubleClickEvent( event );
    emit mouseDouble( this );
}


void ClipItem::hoverMoveEvent( QGraphicsSceneHoverEvent *event )
{
    QGraphicsItem::hoverMoveEvent( event );
    setFocus( Qt::MouseFocusReason );

    // Enable dragging to extend
    m_bExtendLeft  = event->scenePos().x() - pos().x() < 3              && m_fWidth > TRACK_DRAG_MIN_WIDTH;
    m_bExtendRight = event->scenePos().x() - pos().x() - m_fWidth > -3  && m_fWidth > TRACK_DRAG_MIN_WIDTH;

    if ( m_bExtendLeft || m_bExtendRight )
        setCursor( QCursor(Qt::SizeHorCursor) );
    else
        setCursor( QCursor(Qt::ArrowCursor) );
}


void ClipItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    QGraphicsItem::hoverLeaveEvent( event );

    m_bExtendLeft  = false;
    m_bExtendRight = false;

    setCursor( QCursor(Qt::ArrowCursor) );
}


QVariant ClipItem::itemChange( GraphicsItemChange change, const QVariant &value )
{
    // Apply master offset for multi-selection groups
    if ( change == ItemPositionChange && scene() && bMultiSelected && pMasterClipItem )
    {
        // Original offset
        if ( masterInitStarting16ths == -1 )
            masterInitStarting16ths = pMasterClipItem->pClipModel->starting16th;

        // New offset
        int inc = pClipModel->starting16th + (pMasterClipItem->pClipModel->starting16th - masterInitStarting16ths);

        if ( inc > 0 )
            masterInitStarting16ths = pMasterClipItem->pClipModel->starting16th;

        if ( inc >= 0 )
            pClipModel->setStarting16th( inc );

        QPointF newPos = value.toPointF();
        newPos.setX( calculateXPos() );
        newPos.setY( 0 );

        return newPos;
    }

    return QGraphicsItem::itemChange( change, value );
}

#include <QDebug>
#include <QCursor>
#include <math.h>

#include "ClipItem.h"

const int TRACK_HEIGHT = 15;

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
    return QRectF( 0, 0, m_fWidth, 15 );
}


void ClipItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    m_fWidth = pClipModel->length16th * ( m_fSpacing / (float)m_iInvDivisions );

    // Draw clip rect
    painter->setPen( QPen( Qt::NoPen ) );
    painter->setBrush( QBrush( color.darker(isSelected() ? 100 : 140) ) );
    painter->drawRect( QRect( 0, 0, m_fWidth - 1, TRACK_HEIGHT ) );

    // Draw right dividing line
    painter->setBrush( Qt::NoBrush );
    painter->setPen( color.darker() );
    painter->drawLine( m_fWidth - 1, 0, m_fWidth - 1, TRACK_HEIGHT - 1 );
}


void ClipItem::setZoomParams( float spacing, int divisions )
{
    m_fSpacing   = spacing;
    m_iDivisions = divisions;

    if (      m_iDivisions == 1  )  m_iInvDivisions = 16;
    else if ( m_iDivisions == 2  )  m_iInvDivisions = 8;
    else if ( m_iDivisions == 4  )  m_iInvDivisions = 4;
    else if ( m_iDivisions == 8  )  m_iInvDivisions = 2;
    else if ( m_iDivisions == 16 )  m_iInvDivisions = 1;

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

    pClipModel->setStarting16th( clamp( m_clickStarting16ths + iDeltaDivisions, 0, INFINITY ) );
}


void ClipItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    m_clickCursorPos =
    m_prevCursorPos  = QCursor::pos();
    bMoved = false;

    m_clickStarting16ths = pClipModel->starting16th;

    QGraphicsItem::mousePressEvent( event );
    emit mouseDown( this );
}


void ClipItem::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
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

    m_prevCursorPos = QCursor::pos();
}


void ClipItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    QGraphicsItem::mouseReleaseEvent( event );
    bDetached = false;

    this->setOpacity( 1.0f );

    // Snap to position
    snapTo16ths( QCursor::pos().x() );
    setPos( calculateXPos(), 0 );

    // Notify and reset
    emit mouseUp( this );
    bLocked = false;
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
    m_bExtendLeft  = event->scenePos().x() - pos().x() < 5;
    m_bExtendRight = event->scenePos().x() - pos().x() - m_fWidth > -7;

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

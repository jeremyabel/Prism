#include <QDebug>
#include <QCursor>
#include <math.h>

#include "ClipItem.h"

const int TRACK_HEIGHT = 15;

ClipItem::ClipItem( ClipModel* clipModel, QGraphicsItem *parent ) : QGraphicsObject( parent )
{
    m_fWidth    = 0;
    bDetached   = false;
    bMoved      = false;
    pClipModel  = clipModel;

    setFlag( QGraphicsItem::ItemIsSelectable );
    setFlag( QGraphicsItem::ItemIsMovable );
    setFlag( QGraphicsItem::ItemSendsGeometryChanges );
}


QRectF ClipItem::boundingRect() const
{
    return QRectF( 0, 0, m_fWidth, 15 );
}


void ClipItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    m_fWidth = pClipModel->length16th * ( m_fSpacing / (float)m_iInvDivisions );

    // Draw clip rect
    painter->setPen( QPen( Qt::NoPen ) );
    painter->setBrush( QBrush( color ) );
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


void ClipItem::snapTo16ths()
{
    float fDeltaX           = m_clickCursorPos.x() - QCursor::pos().x();
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
           abs( m_clickCursorPos.y() - QCursor::pos().y() ) > 7 ) && !bDetached )
    {
        bDetached = true;
        this->setOpacity( 0.5f );
        emit detached();
    }
    else if ( !bDetached )
    {
        snapTo16ths();
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
    snapTo16ths();
    setPos( calculateXPos(), 0 );

    emit mouseUp( this );
}


void ClipItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    QGraphicsItem::mouseDoubleClickEvent( event );
    emit mouseDouble( this );
}


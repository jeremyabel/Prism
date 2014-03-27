#include <QPainter>
#include <QRectF>
#include <QDebug>

#include "TimelineItem.h"

const int               MAX_SPACING = 200;
const int               MIN_SPACING = 24;
const int               TICK_HEIGHT = 15;

TimelineItem::TimelineItem()
{
    iMeasures = 100;
    m_iWidth  = 0;
    setZoomValue( 500 );
}

void TimelineItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw ticks
    for ( int i = 0; i < iMeasures * iDivisions; i++ )
    {
        // Figure out tick height
        int iHeight = i % iDivisions ? 4 : TICK_HEIGHT;
        if ( iDivisions >= 8 && iHeight < 5 )
        {
            int mult = iDivisions > 8 ? 4 : 2;
            iHeight *= i % mult ? 1 : 2.35f;
        }

        painter->setPen( QPen( QColor(160, 160, 160) ) );
        painter->drawLine( 100 + i * fSpacing, 1, 100 + i * fSpacing, iHeight + 1 );

        m_iWidth += 1 + fSpacing;
    }

    // Horiz lines
    painter->setPen( QPen( QColor(45, 45, 45) ) );
    painter->drawLine( 0, 0, m_iWidth, 0 );
    painter->drawLine( 0, TICK_HEIGHT + 1, m_iWidth, TICK_HEIGHT + 1 );
}


QRectF TimelineItem::boundingRect() const
{
    return QRectF( 0, 0, ( 1 + fSpacing ) * iMeasures * iDivisions, TICK_HEIGHT + 2 );
}


void TimelineItem::setZoomValue( int value )
{
    // Adaptive subdivision
    if (      value <= 100 )    iDivisions = 1;
    else if ( value <= 250 )    iDivisions = 2;
    else if ( value <= 450 )    iDivisions = 4;
    else if ( value <= 650 )    iDivisions = 8;
    else if ( value <= 800 )    iDivisions = 16;

    float minSpacing = (float)MIN_SPACING / (float)iDivisions;
    float maxSpacing = (float)MAX_SPACING / (float)iDivisions;

    fSpacing = ( (float)value / 1000.0f ) * ( maxSpacing - minSpacing ) + minSpacing;

    // Trigger repaint
    prepareGeometryChange();
}

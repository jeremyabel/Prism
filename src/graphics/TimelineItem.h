#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QGraphicsItem>

class TimelineItem : public QGraphicsItem
{

public:
    TimelineItem();

    QRectF          boundingRect() const;
    void            paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    void            setZoomValue( int value );

    int             iMeasures;
    float           fSpacing;
    int             iDivisions;

private:
    int             m_iWidth;
};

#endif // TIMELINEITEM_H

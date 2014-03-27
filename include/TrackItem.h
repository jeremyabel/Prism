#ifndef TRACKITEM_H
#define TRACKITEM_H

#include <QGraphicsObject>
#include <QList>

#include "TrackModel.h"
#include "ClipModel.h"
#include "ClipItem.h"

class TrackItem : public QGraphicsObject
{
    Q_OBJECT

public:
    TrackItem( TrackModel *trackModel );

    QRectF          boundingRect() const;

    TrackModel*     pTrackModel;

signals:
    void            mouseDouble( TrackItem* track );

public slots:
    void            updateClips();

protected:
    virtual void    paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    virtual void    mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );

private:
    QColor          m_clipColor;
};

#endif // TRACKITEM_H

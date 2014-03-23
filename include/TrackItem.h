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
    void            paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    TrackModel*     pTrackModel;

public slots:
    void            updateClips();

private:
    QColor          m_clipColor;
};

#endif // TRACKITEM_H

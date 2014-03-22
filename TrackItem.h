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
    ClipItem*       getClipUnderMouse();
    ClipItem*       getClipToLeftOfPoint(  QPoint p );
    ClipItem*       getClipToRightOfPoint( QPoint p );
    void            offsetClips( int starting16th, int offset16ths );

    TrackModel*     pTrackModel;

public slots:
    void            updateClips();

private:
    QColor          m_clipColor;
};

#endif // TRACKITEM_H

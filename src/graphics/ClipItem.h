#ifndef CLIPITEM_H
#define CLIPITEM_H

#include <QGraphicsObject>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QColor>
#include <QRectF>

#include "ClipModel.h"
#include "TrackItem.h"
#include "TrackModel.h"
#include "ui_ClipParamDialog.h"

class TrackItem;

class ClipItem : public QGraphicsObject
{
    Q_OBJECT

public:
    enum ResizeDirection { RESIZE_LEFT, RESIZE_RIGHT };

    ClipItem( ClipModel* clipModel, QGraphicsItem *parent = 0 );

    QRectF          boundingRect() const;

    void            setZoomParams( float fSpacing, int iDivisions );
    float           calculateXPos();
    inline float    clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }

    bool            bDetached;
    bool            bMoved;
    bool            bLocked;
    bool            bMultiSelected;
    QColor          color;
    ClipModel*      pClipModel;
    TrackItem*      pPrevTrackItem;
    ClipItem*       pMasterClipItem;
    int             masterInitStarting16ths;

signals:
    void            mouseDown(   ClipItem* clip );
    void            mouseUp(     ClipItem* clip );
    void            mouseDouble( ClipItem* clip );
    void            resized(     ClipItem* clip, ClipItem::ResizeDirection resizeDir );
    void            detached();

protected:
    void            paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    virtual void    mousePressEvent(        QGraphicsSceneMouseEvent *event );
    virtual void    mouseMoveEvent(         QGraphicsSceneMouseEvent *event );
    virtual void    mouseReleaseEvent(      QGraphicsSceneMouseEvent *event );
    virtual void    mouseDoubleClickEvent(  QGraphicsSceneMouseEvent *event );
    virtual void    hoverMoveEvent(         QGraphicsSceneHoverEvent *event );
    virtual void    hoverLeaveEvent(        QGraphicsSceneHoverEvent *event );
    QVariant        itemChange(             GraphicsItemChange change, const QVariant &value );

private:
    void            snapTo16ths( int posx );

    float           m_fSpacing;
    float           m_fSpacing16ths;
    float           m_fWidth;
    int             m_iDivisions;
    int             m_iInvDivisions;
    QPoint          m_prevCursorPos;
    QPoint          m_clickCursorPos;
    int             m_clickStarting16ths;
    int             m_clickEnding16ths;
    bool            m_bExtendLeft;
    bool            m_bExtendRight;
};

#endif // CLIPITEM_H

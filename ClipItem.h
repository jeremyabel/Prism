#ifndef CLIPITEM_H
#define CLIPITEM_H

#include <QGraphicsObject>
#include <QPainter>
#include <QColor>
#include <QRectF>

#include "ClipModel.h"
#include "ui_ClipParamDialog.h"


class ClipItem : public QGraphicsObject
{
    Q_OBJECT

public:
    ClipItem( ClipModel* clipModel, QGraphicsItem *parent = 0 );

    QRectF          boundingRect() const;
    void            mousePressEvent( QGraphicsSceneMouseEvent *event );
    void            mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    void            mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    void            mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );
    void            setZoomParams( float fSpacing, int iDivisions );
    float           calculateXPos();

    inline float    clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }

    bool            bDetached;
    bool            bMoved;
    QColor          color;
    ClipModel*      pClipModel;

signals:
    void            mouseDown(   ClipItem* clip );
    void            mouseUp(     ClipItem* clip );
    void            mouseDouble( ClipItem* clip );
    void            detached();

protected:
    void            paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

private:
    void            snapTo16ths();

    float           m_fSpacing;
    float           m_fSpacing16ths;
    float           m_fWidth;
    int             m_iDivisions;
    int             m_iInvDivisions;
    QPoint          m_prevCursorPos;
    QPoint          m_clickCursorPos;
    int             m_clickStarting16ths;
};

#endif // CLIPITEM_H

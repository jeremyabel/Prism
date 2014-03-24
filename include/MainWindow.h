#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

#include "ClipModel.h"
#include "ClipItem.h"
#include "TrackItem.h"
#include "TimelineItem.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit                MainWindow( QWidget *parent = 0 );

    ~MainWindow();

private slots:
    void                    on_timeZoomSlider_valueChanged( int value );
    void                    on_timelineClipGrabbed( ClipItem* clip );
    void                    on_timelineClipReleased();
    void                    on_timelineClipDoubleClicked( ClipItem* clip );
    void                    on_timelineClipMoved();
    void                    on_timelineClipDetached();

    void on_actionAdd_Track_triggered();

    void on_actionDelete_Track_triggered();

private:
    Ui::MainWindow*         ui;

    QGraphicsScene*         m_pScene;
    QList<TrackItem*>       m_pTrackItems;
    QList<ClipItem*>        m_pClipItems;
    TimelineItem*           m_pTimeline;
    ClipItem*               m_pDraggingClip;
    TrackItem*              m_pOriginalTrack;
    TrackItem*              m_pHoverTrack;

    float                   m_fSpacing;
    int                     m_iDivisions;
    int                     m_iInvDivisions;
    int                     m_iMeasures;
    bool                    m_bDialogOpen;
};

#endif // MAINWINDOW_H

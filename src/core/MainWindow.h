#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QUndoStack>
#include <QUndoView>
#include <QAction>

#include "CategoryData.h"
#include "ClipModel.h"
#include "ClipItem.h"
#include "ImageData.h"
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

    void                    keyPressEvent( QKeyEvent *event );

private slots:
    void                    on_timeZoomSlider_valueChanged( int value );
    void                    on_timelineClipGrabbed( ClipItem* clip );
    void                    on_timelineClipReleased( ClipItem* clip );
    void                    on_timelineClipDoubleClicked( ClipItem* clip );
    void                    on_timelineClipMoved();
    void                    on_timelineClipDetached();
    void                    on_timelineClipResized( ClipItem* clip, ClipItem::ResizeDirection resizeDir );
    void                    on_trackDoubleClicked( TrackItem* track );

    void                    on_actionNew_triggered();
    void                    on_actionOpen_triggered();
    void                    on_actionSave_triggered();
    void                    on_actionSave_As_triggered();
    void                    on_actionImport_triggered();
    void                    on_actionExport_triggered();
    void                    on_actionAdd_Track_triggered();

    void                    on_menu_aboutToShow();
    void                    on_graphicsView_customContextMenuRequested( const QPoint &pos );

    void on_actionUndo_triggered();

private:
    bool                    releaseModifiedFile();
    void                    addClip( ClipModel* clipModel, TrackItem* trackItem, bool append = false );
    void                    addTrack( TrackModel* trackModel );
    void                    removeTrack( TrackItem* track );
    int                     getNearest16th();

    Ui::MainWindow*         ui;

    QGraphicsScene*         m_pScene;
    QList<TrackItem*>       m_pTrackItems;
    QList<ClipItem*>        m_pClipItems;
    TimelineItem*           m_pTimeline;
    ClipItem*               m_pDraggingClip;
    TrackItem*              m_pOriginalTrack;
    TrackItem*              m_pHoverTrack;
    CategoryData*           m_pCategoryData;
    ImageData*              m_pImageData;

    QUndoStack*             m_pUndoStack;
    QUndoView*              m_pUndoView;

    QString                 m_sCurrentPath;
    QString                 m_sCategoryPath;
    float                   m_fSpacing;
    int                     m_iDivisions;
    int                     m_iInvDivisions;
    int                     m_iMeasures;
    bool                    m_bDialogOpen;
    bool                    m_bModified;
};

#endif // MAINWINDOW_H

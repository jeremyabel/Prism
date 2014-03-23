#include <QGraphicsView>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QDebug>
#include <QList>
#include <QRectF>
#include <math.h>

#include "MainWindow.h"
#include "ui_MainWindow.h"

const int               MAX_SPACING = 200;
const int               MIN_SPACING = 24;
const int               TICK_HEIGHT = 15;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    m_iMeasures   = 100;
    m_bDialogOpen = false;

    ui->setupUi(this);

    m_pScene = new QGraphicsScene( this );
    ui->graphicsView->setScene( m_pScene );

    m_pTimeline = new TimelineItem();
    m_pScene->addItem( m_pTimeline );

    // Make track data
    TrackModel* pTrackModelA = new TrackModel( "track 1" );
    TrackModel* pTrackModelB = new TrackModel( "track 2", QColor( Qt::green ) );
    TrackModel* pTrackModelC = new TrackModel( "track 3", QColor( Qt::blue ) );

    // Make clip data
    ClipModel*  pClipModelA  = new ClipModel( 10, 30 );
    ClipModel*  pClipModelB  = new ClipModel( 15, 18 );
    ClipModel*  pClipModelC  = new ClipModel(  8,  7 );
    pTrackModelA->pClips.append( pClipModelA );
    pTrackModelB->pClips.append( pClipModelB );
    pTrackModelB->pClips.append( pClipModelC );

    // Make track displays
    m_pTrackItems.append( new TrackItem( pTrackModelA ) );
    m_pTrackItems.append( new TrackItem( pTrackModelB ) );
    m_pTrackItems.append( new TrackItem( pTrackModelC ) );

    for ( int i = 0; i < m_pTrackItems.length(); i++ )
    {
        int iOffset = ( i > 0 ) ? 1 : 0;

        // Add track item
        TrackItem* item = m_pTrackItems[i];
        item->setX( m_pScene->sceneRect().left() + iOffset );
        item->setY( m_pTimeline->boundingRect().height() + ( i * item->boundingRect().height() ) + iOffset );

        // Draw horizontal divider
        float fYPos  = item->pos().y() + item->boundingRect().height();
        float fWidth = m_pTimeline->boundingRect().width();

        m_pScene->addItem( item );

        // Add clips
        for ( int j = 0; j < item->pTrackModel->pClips.length(); j++ )
        {
            ClipItem* clipItem = new ClipItem( item->pTrackModel->pClips[j] );
            connect( clipItem, SIGNAL( mouseDown(ClipItem*) ),   SLOT( on_timelineClipGrabbed(ClipItem*) ) );
            connect( clipItem, SIGNAL( mouseUp(ClipItem*) ),     SLOT( on_timelineClipReleased() ) );
            connect( clipItem, SIGNAL( mouseDouble(ClipItem*) ), SLOT( on_timelineClipDoubleClicked(ClipItem*) ) );
            connect( clipItem, SIGNAL( detached() ),             SLOT( on_timelineClipDetached() ) );

            clipItem->setParentItem( item );
            clipItem->color = item->pTrackModel->qColor;
            clipItem->setObjectName( "clip_" + QString::number( j * i ) );

            m_pClipItems.append( clipItem );
            m_pScene->addItem( clipItem );
        }

        m_pScene->addLine( 0, fYPos, fWidth, fYPos, QPen( QColor(45, 45, 45) ) );
    }

    on_timeZoomSlider_valueChanged( 500 );
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_timeZoomSlider_valueChanged(int value)
{
    m_pTimeline->setZoomValue( value );

    // Adaptive subdivision
    if (      value <= 100 )    m_iDivisions = 1;
    else if ( value <= 250 )    m_iDivisions = 2;
    else if ( value <= 450 )    m_iDivisions = 4;
    else if ( value <= 650 )    m_iDivisions = 8;
    else if ( value <= 800 )    m_iDivisions = 16;

    float minSpacing = (float)MIN_SPACING / (float)m_iDivisions;
    float maxSpacing = (float)MAX_SPACING / (float)m_iDivisions;

    m_fSpacing = ( (float)value / 1000.0f ) * ( maxSpacing - minSpacing ) + minSpacing;

    // Update clips
    for ( int i = 0; i < m_pClipItems.length(); i++ )
    {
        m_pClipItems[i]->setZoomParams( m_fSpacing, m_iDivisions );
    }
}


void MainWindow::on_timelineClipGrabbed( ClipItem *clip )
{
    m_pDraggingClip = clip;
    connect( m_pDraggingClip, SIGNAL(xChanged()), SLOT(on_timelineClipMoved()) );
    connect( m_pDraggingClip, SIGNAL(yChanged()), SLOT(on_timelineClipMoved()) );
}


void MainWindow::on_timelineClipDetached()
{
    // Remove from parent model
    TrackItem* targetTrackItem = (TrackItem*)m_pDraggingClip->parentObject();
    targetTrackItem->pTrackModel->remove( m_pDraggingClip->pClipModel );

    m_pOriginalTrack = targetTrackItem;

    // Clear parent
    m_pDraggingClip->setY( ui->graphicsView->mapFromGlobal( QCursor::pos() ).y() - ( m_pDraggingClip->boundingRect().height() / 2 ) );
    m_pDraggingClip->setParentItem( 0 );
}


void MainWindow::on_timelineClipReleased()
{
    TrackItem* targetTrackItem = (TrackItem*)m_pDraggingClip->parentObject();

    if ( !m_pDraggingClip->bMoved || m_bDialogOpen )
        return;

    // Remove from old parent track
    if ( !m_pDraggingClip->bDetached && targetTrackItem != NULL )
        targetTrackItem->pTrackModel->remove( m_pDraggingClip->pClipModel );

    // If we're not hovering over a track, go back to the original track
    if ( m_pHoverTrack != NULL )
        targetTrackItem = m_pHoverTrack;
    else
        targetTrackItem = m_pOriginalTrack;

    // Reassign parent
    m_pDraggingClip->setParentItem( targetTrackItem );

    // Insert into new parent track
    targetTrackItem->pTrackModel->insert( m_pDraggingClip->pClipModel );

    // Clean up
    disconnect( m_pDraggingClip, SIGNAL(xChanged()), 0, 0 );
    disconnect( m_pDraggingClip, SIGNAL(yChanged()), 0, 0 );
    m_pDraggingClip   = NULL;
    m_pOriginalTrack = NULL;
    m_pHoverTrack    = NULL;
}


void MainWindow::on_timelineClipDoubleClicked( ClipItem *clip )
{
    qDebug() << "Opening clip dialog...";

    m_bDialogOpen = true;

    m_pClipDialog = new ClipParamDialog( *clip->pClipModel, this );
    m_pClipDialog->show();
}


void MainWindow::on_timelineClipMoved()
{
    m_pHoverTrack = NULL;
    QPoint cursor = ui->graphicsView->mapFromGlobal( QCursor::pos() );

    // Redraw tracks
    for ( int i = 0; i < m_pTrackItems.length(); i++ )
        m_pTrackItems[i]->update();

    // Set clip y to cursor y
    if ( m_pDraggingClip->bDetached )
        m_pDraggingClip->setY( m_pDraggingClip->clamp( cursor.y() - ( m_pDraggingClip->boundingRect().height() / 2 ), 0, INFINITY ) );

    // See what track we're over
    for ( int i = 0; i < m_pTrackItems.length(); i++ )
    {
       TrackItem* item = m_pTrackItems[i];

       // Extend bounding rect to the end of the timeline
       QRectF bounds = item->boundingRect();
       bounds.setWidth( bounds.width() + m_pTimeline->boundingRect().width() );

       // Exit if we're over this one
       if ( bounds.contains( cursor ) )
       {
           m_pHoverTrack = m_pTrackItems[i];
           break;
       }
    }

    // Handle hovering over tracks
    /*
    if ( hoverTrack != NULL )
    {
        ClipItem* hoverClip = hoverTrack->getClipUnderMouse();

        // Handle hovering over a clip
        if ( hoverClip != NULL )
        {
            //qDebug() << "We're over a clip starting at" << hoverClip->pClipModel->iStarting16th
            //         << "in the track" << m_pTrackItems[hoverClip->pClipModel->iTrack]->pTrackModel->sName;

            // What side of the clip are we on?
            if ( hoverClip->boundingRect().width() < m_draggingClip->boundingRect().width() )
            {
                float fDist = cursor.x() - ( hoverClip->boundingRect().center().x() + hoverClip->pos().x() );

                if ( fDist < -3.0f )
                    qDebug() << "LEFT";
                else if ( fDist > 3.0f )
                    qDebug() << "RIGHT";
                else
                    qDebug() << "";
            }
        }
        else
        {
            // Find closest items on either side
            ClipItem* pClosestLeftItem  = hoverTrack->getClipToLeftOfPoint(  cursor );
            ClipItem* pClosestRightItem = hoverTrack->getClipToRightOfPoint( cursor );

            if ( pClosestLeftItem != NULL && pClosestLeftItem->isObscuredBy( m_draggingClip ) )
            {
                m_pLeftIntersectClip = pClosestLeftItem;
                qDebug() << pClosestLeftItem->pClipModel->starting16th;
            }
            else
            {
                m_pLeftIntersectClip = NULL;
            }

            if ( pClosestRightItem != NULL && pClosestRightItem->isObscuredBy( m_draggingClip ) )
            {
                m_pRightIntersectClip = pClosestRightItem;
                qDebug() << pClosestRightItem->pClipModel->starting16th;
            }
            else
            {
                m_pRightIntersectClip = NULL;
            }
        }
    }
    */
}

#include <QGraphicsView>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QDebug>
#include <QList>
#include <QRectF>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include <math.h>

#include "AddTrackDialog.h"
#include "ClipParamDialog.h"
#include "ClipCommands.h"
#include "ExportDialog.h"
#include "FileManager.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

const int               MAX_SPACING = 200;
const int               MIN_SPACING = 24;
const int               TICK_HEIGHT = 15;
const int               MIN_IMAGES  = 1;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    m_iMeasures     = 100;
    m_bDialogOpen   = false;
    m_bModified     = false;
    m_sCurrentPath  = "";
    m_sCategoryPath = "";
    m_pUndoStack    = new QUndoStack(this);

    ui->setupUi(this);

    addAction( ui->actionNew );
    addAction( ui->actionOpen );
    addAction( ui->actionSave );
    addAction( ui->actionSave_As );
    addAction( ui->actionImport );
    addAction( ui->actionExport );
    addAction( ui->quit );
    addAction( ui->actionUndo );
    addAction( ui->actionRedo );
    addAction( ui->actionCopy_Clip );
    addAction( ui->actionPaste_Clip );
    addAction( ui->actionAdd_Track );

    connect( ui->menuFile, SIGNAL( aboutToShow() ), SLOT( on_menu_aboutToShow() ) );
    connect( ui->menuEdit, SIGNAL( aboutToShow() ), SLOT( on_menu_aboutToShow() ) );

    m_pScene = new QGraphicsScene( this );
    ui->graphicsView->setScene( m_pScene );

    m_pTimeline = new TimelineItem();
    m_pScene->addItem( m_pTimeline );

    // Get settings
    bool initOpenFailed = true;
    QSettings settings("jeremyabel.com", "Prism");
    if ( settings.status() == QSettings::NoError )
    {
        if ( settings.contains("path") )
        {
            m_pCategoryData = new CategoryData();
            m_pImageData    = new ImageData();
            m_sCurrentPath  = settings.value("path").toString();
            qDebug() << " Current path:" << m_sCurrentPath;

            // Attempt to load last file
            QList<TrackModel*> loadedTracks;
            if ( FileManager::open( m_sCurrentPath, &loadedTracks, m_pCategoryData, m_pImageData ) )
            {
                initOpenFailed = false;

                // Add track items
                for ( int i = 0; i < loadedTracks.size(); i++ )
                    addTrack( loadedTracks[i] );

                m_bModified = false;

                ui->statusBar->showMessage("Loaded " + QString::number(loadedTracks.size()) + " track(s) " + "from " + m_sCurrentPath + ".", 10000 );
            }
            else
            {
                // Reset saved path if we have a problem opening this one
                qDebug() << "Previous file could not be opened... resetting.";
                settings.setValue("path", "");
            }
        }
    }

    // Start with new document if we can't open the old one
    if ( initOpenFailed )
    {
        qDebug() << "No previous file found...";

        m_pCategoryData = new CategoryData();
        m_pImageData    = new ImageData();

        // Add one new track
        TrackModel* pTrackModel = new TrackModel( "track 1", QColor(Qt::red) );
        addTrack( pTrackModel );

        ui->statusBar->showMessage("");
    }

    on_timeZoomSlider_valueChanged( 500 );
}


MainWindow::~MainWindow()
{
    m_pImageData->close();
    delete ui;
}


void MainWindow::addClip( ClipModel *clipModel, TrackItem *trackItem, bool append )
{
    m_bModified = true;
    qDebug() << "Adding clip item to track" << trackItem->pTrackModel->sName;

    ClipItem* clipItem = new ClipItem( clipModel );
    connect( clipItem, SIGNAL( mouseDown(ClipItem*) ),   SLOT( on_timelineClipGrabbed(ClipItem*) ) );
    connect( clipItem, SIGNAL( mouseUp(ClipItem*) ),     SLOT( on_timelineClipReleased(ClipItem*) ) );
    connect( clipItem, SIGNAL( mouseDouble(ClipItem*) ), SLOT( on_timelineClipDoubleClicked(ClipItem*) ) );
    connect( clipItem, SIGNAL( detached() ),             SLOT( on_timelineClipDetached() ) );
    connect( clipItem, SIGNAL( resized(ClipItem*, ClipItem::ResizeDirection) ), SLOT( on_timelineClipResized(ClipItem*, ClipItem::ResizeDirection) ) );

    clipItem->setParentItem( trackItem );
    clipItem->color = trackItem->pTrackModel->qColor;
    clipItem->setObjectName( "clip_" + QString::number(m_pClipItems.size()) );

    m_pClipItems.append( clipItem );

    clipItem->setZoomParams( m_fSpacing, m_iDivisions );
    m_pScene->invalidate();

    if ( append )
        trackItem->pTrackModel->insert( clipModel );
}


void MainWindow::addTrack( TrackModel *trackModel )
{
    m_bModified = true;

    ui->statusBar->showMessage( "Added track '" + trackModel->sName + "'", 2500 );
    qDebug() << "Adding track" << trackModel->sName;

    int s = m_pTrackItems.size();
    int iOffset = ( s > 0 ) ? 1 : 0;

    // Add track item
    TrackItem* item = new TrackItem( trackModel );
    item->setY( m_pTimeline->boundingRect().height() + s * ( item->boundingRect().height() + iOffset ) );

    connect( item, SIGNAL( mouseDouble(TrackItem*) ), SLOT( on_trackDoubleClicked(TrackItem*) ) );
    m_pScene->addItem( item );
    m_pTrackItems.append( item );

    // Add clips
    for ( int j = 0; j < item->pTrackModel->pClips.length(); j++ )
        addClip( item->pTrackModel->pClips[j], item );

    // Draw horizontal divider
    float fYPos  = item->pos().y() + item->boundingRect().height();
    float fWidth = m_pTimeline->boundingRect().width();
    item->pBottomLine = m_pScene->addLine( 0, fYPos, fWidth, fYPos, QPen( QColor(45, 45, 45) ) );
}


void MainWindow::removeTrack( TrackItem *track )
{
    m_bModified = true;

    ui->statusBar->showMessage( "Removed track '" + track->pTrackModel->sName + "'", 2500 );
    qDebug() << "Removing track" << track->pTrackModel->sName;

    // Remove and clean up child clips
    for ( int i = 0; i < track->childItems().size(); i++ )
    {
        QGraphicsItem* item = track->childItems()[i];
        m_pScene->removeItem( item );

        ClipItem* clipItem = dynamic_cast<ClipItem*>(item);

        if ( clipItem && m_pClipItems.contains(clipItem) )
        {
            disconnect( clipItem, SIGNAL( mouseDown(ClipItem*) ),   0, 0 );
            disconnect( clipItem, SIGNAL( mouseUp(ClipItem*) ),     0, 0 );
            disconnect( clipItem, SIGNAL( mouseDouble(ClipItem*) ), 0, 0 );
            disconnect( clipItem, SIGNAL( resized(ClipItem*) ),     0, 0 );
            disconnect( clipItem, SIGNAL( detached() ),             0, 0 );

            m_pClipItems.removeAt(m_pClipItems.indexOf(clipItem));
        }
    }

    disconnect( track, SIGNAL( mouseDouble(TrackItem*) ), 0, 0 );

    int trackIndex = m_pTrackItems.indexOf(track);
    m_pTrackItems.removeAt( trackIndex );
    m_pScene->removeItem( track->pBottomLine );
    m_pScene->removeItem( track );

    // Move other tracks up
    if ( m_pTrackItems.size() > 0 )
    {
        for ( int i = trackIndex; i < m_pTrackItems.size(); i++ )
        {
            TrackItem* trackItem = m_pTrackItems[i];
            trackItem->setPos( trackItem->pos().x(), trackItem->pos().y() - trackItem->boundingRect().height() - 1 );
            trackItem->pBottomLine->setPos( trackItem->pBottomLine->pos().x(), trackItem->pBottomLine->pos().y() - trackItem->boundingRect().height() - 1 );
        }
    }
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

    if ( m_iDivisions > 0 )
        m_iInvDivisions = 16 / m_iDivisions;

    float minSpacing = (float)MIN_SPACING / (float)m_iDivisions;
    float maxSpacing = (float)MAX_SPACING / (float)m_iDivisions;

    m_fSpacing = ((float)value / 1000.0f) * (maxSpacing - minSpacing) + minSpacing;

    // Update clips
    for ( int i = 0; i < m_pClipItems.length(); i++ )
    {
        m_pClipItems[i]->setZoomParams( m_fSpacing, m_iDivisions );
    }
}


void MainWindow::on_timelineClipGrabbed( ClipItem *clip )
{
    m_pDraggingClip = clip;
    m_pDraggingClip->pClipModel->oldStarting16th = m_pDraggingClip->pClipModel->starting16th;

    connect( m_pDraggingClip, SIGNAL(xChanged()), SLOT(on_timelineClipMoved()) );
    connect( m_pDraggingClip, SIGNAL(yChanged()), SLOT(on_timelineClipMoved()) );

    // Show status message
    int matches  = m_pImageData->makeQuery(clip->pClipModel->getImageQuery()).size();
    QString imgs = (matches != 1 ? " images: " : " image: ");
    ui->statusBar->showMessage( "Matches " + QString::number(matches) + imgs + clip->pClipModel->getStatusMessage() );

    // Lock clips to their current track if we've selected multiple things
    for ( int i = 0; i < m_pScene->selectedItems().length(); i++ )
    {
        ClipItem* pClip = dynamic_cast<ClipItem*>(m_pScene->selectedItems()[i]);

        pClip->bLocked = m_pScene->selectedItems().size() > 1;

        // Other selected clips are subservient to this one
        if ( pClip != m_pDraggingClip && m_pScene->selectedItems().size() > 1 )
        {
            pClip->pMasterClipItem = m_pDraggingClip;
            pClip->masterInitStarting16ths = -1;
            pClip->bMultiSelected = true;
        }
        else
        {
            pClip->pMasterClipItem = NULL;
            pClip->masterInitStarting16ths = -1;
            pClip->bMultiSelected = false;
        }
    }
}


void MainWindow::on_timelineClipDetached()
{
    // Remove from parent model
    TrackItem* targetTrackItem = (TrackItem*)m_pDraggingClip->parentObject();
    targetTrackItem->pTrackModel->remove( m_pDraggingClip->pClipModel );

    m_pOriginalTrack = targetTrackItem;

    // Clear parent
    m_pDraggingClip->setY( ui->graphicsView->mapFromGlobal( QCursor::pos() ).y() - ( m_pDraggingClip->boundingRect().height() / 2 ) );
    m_pDraggingClip->pPrevTrackItem = targetTrackItem;
    m_pDraggingClip->setParentItem( 0 );
}


void MainWindow::on_timelineClipReleased( ClipItem* clip )
{
    m_bModified = true;

    TrackItem* targetTrackItem = static_cast<TrackItem*>(m_pDraggingClip->parentObject());

    // Don't do anything if we don't need to
    if ( !m_pDraggingClip->bMoved || m_bDialogOpen || m_pDraggingClip->bLocked )
        return;

    // Keep reference for undo stack
    if ( m_pDraggingClip->parentObject() )
        clip->pPrevTrackItem = static_cast<TrackItem*>(m_pDraggingClip->parentObject());

    // If we're not hovering over a track, go back to the original track
    if ( m_pHoverTrack != NULL )
        targetTrackItem = m_pHoverTrack;
    else
        targetTrackItem = m_pOriginalTrack;

    // Execute command and push to undo stack
    m_pUndoStack->push( new MoveClipCommand(clip, targetTrackItem) );

    // Clean up
    disconnect( m_pDraggingClip, SIGNAL(xChanged()), 0, 0 );
    disconnect( m_pDraggingClip, SIGNAL(yChanged()), 0, 0 );
    m_pDraggingClip  = NULL;
    m_pOriginalTrack = NULL;
    m_pHoverTrack    = NULL;
}


void MainWindow::on_timelineClipDoubleClicked( ClipItem *clip )
{
    // Disable if we've selected multiple clips
    if ( m_pScene->selectedItems().size() > 1 )
        return;

    qDebug() << "Opening clip dialog...";

    m_bModified   = true;
    m_bDialogOpen = true;

    ClipModel origClip = *clip->pClipModel;
    ClipParamDialog* pClipDialog = new ClipParamDialog( *clip->pClipModel, m_pCategoryData, this );

    if ( pClipDialog->exec() == 0 )
    {
        // Restore
        qDebug() << "Restoring...";
        m_bModified = false;
        *clip->pClipModel = origClip;
    }
    else
    {
        // Add to undo stack
        m_pUndoStack->push( new EditClipCommand( clip, origClip.serializeToJson() ) );

        // Update status bar
        int matches  = m_pImageData->makeQuery(clip->pClipModel->getImageQuery()).size();
        QString imgs = (matches != 1 ? " images: " : " image: ");
        ui->statusBar->showMessage( "Matches " + QString::number(matches) + imgs + clip->pClipModel->getStatusMessage() );

        // Alert if query doesn't match enough images
        if ( matches <= MIN_IMAGES )
        {
            QString matchString = "doens't match any images.";
            if ( matches > 0)
            {
                matchString  = "matches only ";
                matchString += QString::number(matches);
                matchString += (matches != 1 ? " images." : " image.");
            }

            QMessageBox::warning( this, "Warning", "This clip " + matchString, QMessageBox::Ok );
        }
    }

    m_bDialogOpen = false;
    qDebug() << "...closed";
}


void MainWindow::on_timelineClipMoved()
{
    m_bModified   = true;
    m_pHoverTrack = NULL;
    QPoint cursor = ui->graphicsView->mapFromGlobal( QCursor::pos() );

    // Redraw tracks
    for ( int i = 0; i < m_pTrackItems.length(); i++ )
        m_pTrackItems[i]->update();

    if ( !m_pDraggingClip )
        return;

    // Set clip y to cursor y
    if ( m_pDraggingClip->bDetached )
        m_pDraggingClip->setY( m_pDraggingClip->clamp( cursor.y() - ( m_pDraggingClip->boundingRect().height() / 2 ), 0, INFINITY ) );

    // See what track we're over
    for ( int i = 0; i < m_pTrackItems.length(); i++ )
    {
       TrackItem* item = m_pTrackItems[i];

       // Extend bounding rect to the end of the timeline
       QRectF bounds = item->mapRectToParent( item->boundingRect() );
       bounds.setWidth( bounds.width() + m_pTimeline->boundingRect().width() );

       // Exit if we're over this one
       if ( bounds.contains( cursor ) )
       {
           m_pHoverTrack = item;
           break;
       }
    }
}


void MainWindow::on_timelineClipResized( ClipItem *clip, ClipItem::ResizeDirection resizeDir )
{
    // Add to undo stack
    m_pUndoStack->push( new ResizeClipCommand(clip, resizeDir) );
}


void MainWindow::on_trackDoubleClicked( TrackItem *track )
{
    qDebug() << "Opening renaming dialog...";

    m_bDialogOpen = true;
    m_bModified   = false;

    bool ok;
    QString text = QInputDialog::getText( this, tr("Rename Track"),
                                          tr("Track name:"), QLineEdit::Normal,
                                          track->pTrackModel->sName, &ok );

    if ( ok && !text.isEmpty() )
    {
        m_bModified = true;
        track->pTrackModel->sName = text;
        qDebug() << "Track renamed:" << track->pTrackModel->sName;
    }

    track->update();
    m_bDialogOpen = false;
    qDebug() << "...closed";
}


void MainWindow::keyPressEvent( QKeyEvent *event )
{
    if ( m_pScene->selectedItems().size() > 0 )
    {
        if ( event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace )
        {
            m_bModified = true;
            qDebug() << "Deleting clip(s)";

            while ( m_pScene->selectedItems().size() > 0 )
            {
                ClipItem*  clipItem  = dynamic_cast<ClipItem*>(m_pScene->selectedItems()[0]);
                TrackItem* trackItem = dynamic_cast<TrackItem*>(clipItem->parentItem());

                disconnect( clipItem, SIGNAL( mouseDown(ClipItem*) ),   0, 0 );
                disconnect( clipItem, SIGNAL( mouseUp(ClipItem*) ),     0, 0 );
                disconnect( clipItem, SIGNAL( mouseDouble(ClipItem*) ), 0, 0 );
                disconnect( clipItem, SIGNAL( detached() ),             0, 0 );

                trackItem->pTrackModel->remove( clipItem->pClipModel );
                m_pClipItems.removeAt(m_pClipItems.indexOf(clipItem));
                m_pScene->removeItem( clipItem );
            }
        }
    }
}


int MainWindow::getNearest16th()
{
    // Get nearest 16th to cursor
    QPointF offsetPt = ui->graphicsView->mapToScene( ui->graphicsView->mapFromGlobal(QCursor::pos()) );
    int offsetX = (int)offsetPt.x() - 101;

    return (int)((float)offsetX / m_fSpacing ) * m_iInvDivisions;
}


//------------------------------------------------------------------------------------------------------
// Menu Actions
//------------------------------------------------------------------------------------------------------

void MainWindow::on_menu_aboutToShow()
{
    ui->menuEdit->actions().at(0)->setEnabled( m_pUndoStack->canUndo() );
    ui->menuEdit->actions().at(0)->setText( "Undo " + m_pUndoStack->undoText() );

    ui->menuEdit->actions().at(1)->setEnabled( m_pUndoStack->canRedo() );
    ui->menuEdit->actions().at(1)->setText( "Redo " + m_pUndoStack->redoText() );

    ui->menuFile->actions().at(2)->setEnabled( m_bModified );

    ui->menuFile->actions().at(5)->setEnabled( m_pImageData->isReady() );
}


bool MainWindow::releaseModifiedFile()
{
    // Allow user to save current file if it has been modified since the last save
    if ( m_bModified )
    {
        QMessageBox msgBox;
        msgBox.setText( "The document has been modified." );
        msgBox.setInformativeText( "Do you want to save your changes?" );
        msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
        msgBox.setDefaultButton( QMessageBox::Save );

        switch ( msgBox.exec() )
        {
            case QMessageBox::Save:
                qDebug() << "...save";
                on_actionSave_triggered();
                break;
            case QMessageBox::Discard:
                qDebug() << "...discard";
                break;
            case QMessageBox::Cancel:
                qDebug() << "...closed";
                return false;
            default:
                return false;
        }
    }

    // Clear undo stack
    m_pUndoStack->clear();

    return true;
}

void MainWindow::on_actionNew_triggered()
{
    qDebug() << "action: New...";

    // Deal with modified file
    if ( !releaseModifiedFile() )
        return;

    // Remove existing tracks
    while ( m_pTrackItems.size() > 0 )
        removeTrack( m_pTrackItems[0] );

    // Reset data
    m_pImageData->close();
    m_pCategoryData->categoryList->clear();

    // Add one new track
    TrackModel* pTrackModel = new TrackModel( "track 1", QColor( Qt::red) );
    addTrack( pTrackModel );

    // Reset path
    m_sCurrentPath = "";

    QSettings settings("jeremyabel.com", "Prism");
    settings.setValue("path", "");

    m_bModified = false;
}


void MainWindow::on_actionOpen_triggered()
{
    qDebug() << "action: Open...";

    // Deal with modified file
    if ( !releaseModifiedFile() )
        return;

    // Prep file dialog
    QFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::ExistingFile );
    dialog.setNameFilter( tr("Timeline File (*.prsm)") );
    dialog.setViewMode( QFileDialog::List );

    // Show dialog
    if ( !dialog.exec() )
        return;

    m_sCurrentPath = dialog.selectedFiles().at(0);

    // Remove existing tracks
    while ( m_pTrackItems.size() > 0 )
        removeTrack( m_pTrackItems[0] );

    // Attempt to load from file
    QList<TrackModel*> loadedTracks;
    if ( FileManager::open( m_sCurrentPath, &loadedTracks, m_pCategoryData, m_pImageData ) )
    {
        QSettings settings("jeremyabel.com", "Prism");
        settings.setValue("path", m_sCurrentPath);

        // Add track items
        for ( int i = 0; i < loadedTracks.size(); i++ )
            addTrack( loadedTracks[i] );

        m_bModified = false;
        ui->statusBar->showMessage("Loaded " + QString::number(loadedTracks.size()) + " track(s) " + "from " + m_sCurrentPath + ".", 10000 );
    }
}


void MainWindow::on_actionSave_triggered()
{
    qDebug() << "action: Save...";

    // Just run Save As if we've never saved before
    if ( m_sCurrentPath.length() <= 0 )
        on_actionSave_As_triggered();
    else
    {
        // Put track models into list
        QList<TrackModel*> trackModels;
        for ( int i = 0; i < m_pTrackItems.size(); i++ )
            trackModels.append( m_pTrackItems[i]->pTrackModel );

        // Write file to current path
        if ( FileManager::saveToFile( m_sCurrentPath, m_sCategoryPath, trackModels ) )
        {
            m_bModified = false;
            ui->statusBar->showMessage( "Saved successfully.", 2500 );
        }
        else
        {
            QMessageBox::critical( this, "Error", "There was a problem while saving :(", QMessageBox::Ok );
        }
    }
}


void MainWindow::on_actionSave_As_triggered()
{
    qDebug() << "action: Save As...";

    // Prep file dialog
    QFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::AnyFile );
    dialog.setNameFilter( tr( "Timeline File (*.prsm)" ) );
    dialog.setViewMode( QFileDialog::List );
    dialog.setAcceptMode( QFileDialog::AcceptSave );

    if ( !dialog.exec() )
    {
        qDebug() << "...closed";
        return;
    }

    // Put track models into list
    QList<TrackModel*> trackModels;
    for ( int i = 0; i < m_pTrackItems.size(); i++ )
        trackModels.append( m_pTrackItems[i]->pTrackModel );

    QString path = dialog.selectedFiles().at(0);
    if ( FileManager::saveToFile( path, m_sCategoryPath, trackModels ) )
    {
        m_sCurrentPath = path;
        qDebug() << "Current path:" << m_sCurrentPath;

        QSettings settings("jeremyabel.com", "Prism");
        settings.setValue("path", m_sCurrentPath);

        m_bModified = false;
        ui->statusBar->showMessage( "Saved successfully.", 2500 );
    }
    else
    {
        QMessageBox::critical( this, "Error", "There was a problem while saving :(", QMessageBox::Ok );
    }
}


void MainWindow::on_actionImport_triggered()
{
    qDebug() << "action: Import...";

    // Prep file dialog
    QFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::ExistingFile );
    dialog.setNameFilter( tr("Metadata File (*.meta)") );
    dialog.setViewMode( QFileDialog::List );

    // Show dialog
    if ( !dialog.exec() )
        return;

    m_pCategoryData = new CategoryData();
    m_sCategoryPath = dialog.selectedFiles().at(0);
    if ( FileManager::import( m_sCategoryPath, m_pCategoryData, m_pImageData ) )
    {
        QSettings settings("jeremyabel.com", "Prism");
        settings.setValue("categoryPath", m_sCategoryPath);

        m_bModified = true;
        ui->statusBar->showMessage( "Metafile imported successfully.", 2500 );
    }
    else
    {
        QMessageBox::critical( this, "Error", "There was a problem while importing :(", QMessageBox::Ok );
    }
}


void MainWindow::on_actionExport_triggered()
{
    qDebug() << "action: Export...";

    // Get previous export settings
    QSettings settings("jeremyabel.com", "Prism");
    float bpm = settings.value( "bpm", QVariant(120.00f) ).toFloat();
    float fps = settings.value( "fps", QVariant(60.00f)  ).toFloat();

    // Show export settings dialog
    ExportDialog* exportDialog = new ExportDialog( bpm, fps, this );

    if ( !exportDialog->exec() )
    {
        qDebug() << "...closed";
        return;
    }

    // Put track models into list
    QList<TrackModel*> trackModels;
    for ( int i = 0; i < m_pTrackItems.size(); i++ )
        trackModels.append( m_pTrackItems[i]->pTrackModel );

    // Prep file dialog
    QFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::AnyFile );
    dialog.setNameFilter( tr( "XML File (*.xml)" ) );
    dialog.setViewMode( QFileDialog::List );
    dialog.setAcceptMode( QFileDialog::AcceptSave );
    dialog.setDirectory( settings.value( "exportPath", QVariant(QDir::homePath()) ).toString() );

    if ( !dialog.exec() )
    {
        qDebug() << "...closed";
        return;
    }

    // Save settings
    QString path = dialog.selectedFiles().at(0);
    settings.setValue( "bpm",        QVariant(exportDialog->bpm) );
    settings.setValue( "fps",        QVariant(exportDialog->fps) );
    settings.setValue( "exportPath", QVariant(path) );

    // Export!
    if ( FileManager::exportToXML(path, &trackModels, m_pCategoryData, m_pImageData, exportDialog->bpm, exportDialog->fps) )
        ui->statusBar->showMessage( "Exported successfully", 2500 );
    else
        QMessageBox::critical( this, "Error", "There was a problem while exporting :(", QMessageBox::Ok );
}


void MainWindow::on_actionAdd_Track_triggered()
{
    qDebug() << "Add track...";

    // Show dialog
    QString defaultName = tr("track ") + QString::number(m_pTrackItems.size() + 1);
    AddTrackDialog* pAddDialog = new AddTrackDialog( defaultName, m_pTrackItems.size(), this );

    // Check results
    if ( pAddDialog->exec() > 0 )
    {
        qDebug() << "Adding track" << pAddDialog->newNameString;

        // Create new track item + graphics
        TrackModel* pTrackModel = new TrackModel( pAddDialog->newNameString, pAddDialog->color );
        addTrack( pTrackModel );
    }
}


void MainWindow::on_actionUndo_triggered()
{
    m_pUndoStack->undo();
    m_pScene->update();

    m_bModified = true;

    // Redraw tracks
    for ( int i = 0; i < m_pTrackItems.length(); i++ )
        m_pTrackItems[i]->update();
}


//------------------------------------------------------------------------------------------------------
// Context Menu Actions
//------------------------------------------------------------------------------------------------------

void MainWindow::on_graphicsView_customContextMenuRequested( const QPoint &pos )
{
    if ( m_pScene->selectedItems().size() > 0 )
        return;

    QPoint globalPos = ui->graphicsView->mapToGlobal(pos);

    QMenu contextMenu;
    contextMenu.addAction( "Add Clip" );
    contextMenu.addAction( "Remove Track" )->setDisabled( m_pTrackItems.size() <= 1 );

    TrackItem* hoverTrack = NULL;

    // See which track we're over
    for ( int i = 0; i < m_pTrackItems.length(); i++ )
    {
       TrackItem* item = m_pTrackItems[i];

       // Extend bounding rect to the end of the timeline
       QRectF bounds = item->mapRectToParent( item->boundingRect() );
       bounds.setWidth( bounds.width() + m_pTimeline->boundingRect().width() );

       // Exit if we're over this one
       if ( bounds.contains( ui->graphicsView->mapFromGlobal(globalPos) ) )
       {
           hoverTrack = m_pTrackItems[i];
           break;
       }
    }

    if ( !hoverTrack )
        return;

    qDebug() << "Show context menu";
    QAction* selectedItem = contextMenu.exec( globalPos );

    // Evaluate selection option
    if ( selectedItem )
    {
        //Selected option = Add Clip
        if ( QString::compare(selectedItem->text(), tr("Add Clip")) == 0 )
        {
            int nearest16th = getNearest16th();
            ClipModel* pClipModel = new ClipModel(nearest16th, 4);
            addClip( pClipModel, hoverTrack, true );

            ui->statusBar->showMessage( "Clip added at " + QString::number(nearest16th), 2500 );
            qDebug() << "Adding clip at " << nearest16th;
            return;
        }

        // Selected option = Remove Track
        if ( QString::compare(selectedItem->text(), tr("Remove Track")) == 0 )
        {
            // Ask for confirmation
            if ( QMessageBox::question(this, "Remove Track", "Are you sure you want to remove this track?" ) == QMessageBox::Yes )
                removeTrack( hoverTrack );
        }
    }
}

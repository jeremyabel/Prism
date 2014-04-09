#include <QtGui>
#include <QDebug>

#include "ClipCommands.h"


EditClipCommand::EditClipCommand( ClipItem *clipItem, QJsonObject oldJson, QUndoCommandPrivate *parent )
{
    Q_UNUSED(parent);

    m_pClipItem     = clipItem;
    m_oldJson       = oldJson;
    m_newJson       = clipItem->pClipModel->serializeToJson();

    setText( "edit" );
}

void EditClipCommand::undo()
{
    qDebug() << "EditClipCommand: undo";

    m_pClipItem->pClipModel = new ClipModel( m_oldJson );
    m_pClipItem->update();
}

void EditClipCommand::redo()
{
    m_pClipItem->pClipModel = new ClipModel( m_newJson );
    m_pClipItem->update();
}



MoveClipCommand::MoveClipCommand( ClipItem *clipItem, TrackItem* newTrackItem, QUndoCommandPrivate *parent )
{
    Q_UNUSED(parent);

    m_pClipItem         = clipItem;
    m_iOldStarting16th  = clipItem->pClipModel->oldStarting16th;
    m_pOldTrackItem     = clipItem->pPrevTrackItem;
    m_iNewStarting16th  = clipItem->pClipModel->starting16th;
    m_pNewTrackItem     = newTrackItem;

    setText( "move" );
}

void MoveClipCommand::undo()
{
    qDebug() << "MoveClipCommand: undo";

    m_pClipItem->pClipModel->setStarting16th( m_iOldStarting16th, true );

    // Remove from new track
    m_pNewTrackItem->pTrackModel->remove( m_pClipItem->pClipModel );

    // Insert into old track
    m_pClipItem->setParentItem( m_pOldTrackItem );
    m_pOldTrackItem->pTrackModel->insert( m_pClipItem->pClipModel );
}

void MoveClipCommand::redo()
{
    qDebug() << "MoveClipCommand";

    m_pClipItem->pClipModel->setStarting16th( m_iNewStarting16th, true );

    // Remove from old track
    m_pOldTrackItem->pTrackModel->remove( m_pClipItem->pClipModel );

    // Insert into new track
    m_pClipItem->setParentItem( m_pNewTrackItem );
    m_pNewTrackItem->pTrackModel->insert( m_pClipItem->pClipModel );
}

bool MoveClipCommand::mergeWith( const QUndoCommand *command )
{
    const MoveClipCommand *moveCommand = static_cast<const MoveClipCommand*>(command);
    ClipItem* clipItem = moveCommand->m_pClipItem;

    // Don't merge when changing tracks
    if ( m_pClipItem != clipItem || m_pNewTrackItem != moveCommand->m_pNewTrackItem )
        return false;

    m_iNewStarting16th = clipItem->pClipModel->starting16th;
    return true;
}



ResizeClipCommand::ResizeClipCommand( ClipItem *clipItem, QUndoCommandPrivate *parent )
{
    Q_UNUSED(parent);

    m_pClipItem         = clipItem;
    m_iOldStarting16th  = clipItem->pClipModel->oldStarting16th;
    m_iOldEnding16th    = clipItem->pClipModel->oldEnding16th;
    m_iNewStarting16th  = clipItem->pClipModel->starting16th;
    m_iNewEnding16th    = clipItem->pClipModel->ending16th;

    setText( "resize" );
}

void ResizeClipCommand::undo()
{
    qDebug() << "ResizeClipCommand: undo";

    m_pClipItem->pClipModel->setStarting16th( m_iOldStarting16th, true );
    m_pClipItem->pClipModel->setEnding16th( m_iOldEnding16th, true );
    m_pClipItem->setX( m_pClipItem->calculateXPos() );
}

void ResizeClipCommand::redo()
{
    m_pClipItem->pClipModel->setStarting16th( m_iNewStarting16th, true );
    m_pClipItem->pClipModel->setEnding16th( m_iNewEnding16th );
    m_pClipItem->setX( m_pClipItem->calculateXPos() );
}

bool ResizeClipCommand::mergeWith( const QUndoCommand *command )
{
    const MoveClipCommand *moveCommand = static_cast<const MoveClipCommand*>(command);
    ClipItem* clipItem = moveCommand->m_pClipItem;

    if ( m_pClipItem != clipItem )
        return false;

    m_iNewStarting16th  = clipItem->pClipModel->starting16th;
    m_iNewEnding16th    = clipItem->pClipModel->ending16th;
    return true;
}

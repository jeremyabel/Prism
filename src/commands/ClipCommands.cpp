#include <QtGui>
#include <QDebug>

#include "ClipCommands.h"


MoveClipCommand::MoveClipCommand( ClipItem *clipItem, QUndoCommandPrivate *parent )
{
    Q_UNUSED(parent);

    m_pClipItem         = clipItem;
    m_iOldStarting16th  = clipItem->pClipModel->oldStarting16th;
    m_pOldTrackItem     = clipItem->pPrevTrackItem;
    m_iNewStarting16th  = clipItem->pClipModel->starting16th;
    m_pNewTrackItem     = static_cast<TrackItem*>(clipItem->parentItem());
}

void MoveClipCommand::undo()
{
    qDebug() << "MoveClipCommand: undo";

    m_pClipItem->pClipModel->setStarting16th( m_iOldStarting16th );

    // Remove from new track
    m_pNewTrackItem->pTrackModel->remove( m_pClipItem->pClipModel );

    // Insert into old track
    m_pClipItem->setParentItem( m_pOldTrackItem );
    m_pOldTrackItem->pTrackModel->insert( m_pClipItem->pClipModel );
}

void MoveClipCommand::redo()
{
    m_pClipItem->pClipModel->setStarting16th( m_iNewStarting16th );
    m_pClipItem->setParent( m_pNewTrackItem );
}

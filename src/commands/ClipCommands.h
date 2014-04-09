#ifndef CLIPCOMMANDS_H
#define CLIPCOMMANDS_H

#include <QUndoCommand>
#include <QJsonObject>

#include "ClipItem.h"
#include "TrackItem.h"


class EditClipCommand : public QUndoCommand
{
public:
    EditClipCommand( ClipItem* clipItem, QJsonObject oldJson, QUndoCommandPrivate* parent = 0 );

    void undo();
    void redo();

private:
    ClipItem*   m_pClipItem;
    QJsonObject m_oldJson;
    QJsonObject m_newJson;
};



class RemoveClipCommand : public QUndoCommand
{
public:
    enum { Id = 3 };

    RemoveClipCommand( ClipItem* clipItem, QUndoCommandPrivate* parent = 0 );

    void undo();
    void redo();
    bool mergeWith( const QUndoCommand* command );
    int  id() const { return Id; }

private:
    ClipItem*   m_pClipItem;
};



class MoveClipCommand : public QUndoCommand
{
public:
    enum { Id = 1 };

    MoveClipCommand( ClipItem* clipItem, TrackItem* newTrackItem, QUndoCommandPrivate* parent = 0 );

    void undo();
    void redo();
    bool mergeWith( const QUndoCommand* command );
    int  id() const { return Id; }

    ClipItem*   m_pClipItem;
    TrackItem*  m_pNewTrackItem;

private:
    TrackItem*  m_pOldTrackItem;
    int         m_iOldStarting16th;
    int         m_iNewStarting16th;
};



class ResizeClipCommand : public QUndoCommand
{
public:
    enum { Id = 2 };

    ResizeClipCommand( ClipItem* clipItem, ClipItem::ResizeDirection resizeDir, QUndoCommandPrivate* parent = 0 );

    void undo();
    void redo();
    bool mergeWith( const QUndoCommand* command );
    int  id() const { return Id; }

    ClipItem::ResizeDirection eResizeDir;

private:
    ClipItem*   m_pClipItem;
    int         m_iOldStarting16th;
    int         m_iNewStarting16th;
    int         m_iOldEnding16th;
    int         m_iNewEnding16th;
};

#endif // CLIPCOMMANDS_H

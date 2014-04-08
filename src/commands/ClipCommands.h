#ifndef CLIPCOMMANDS_H
#define CLIPCOMMANDS_H

#include <QUndoCommand>

#include "ClipItem.h"
#include "TrackItem.h"

class AddClipCommand : public QUndoCommand
{
public:

private:
    ClipItem*   m_pClipItem;
};


class EditClipCommand : public QUndoCommand
{
public:

private:
    ClipItem*   m_pClipItem;
};


class RemoveClipCommand : public QUndoCommand
{
public:

private:
    ClipItem*   m_pClipItem;
};


class MoveClipCommand : public QUndoCommand
{
public:
    MoveClipCommand( ClipItem* clipItem, QUndoCommandPrivate* parent = 0 );

    void undo();
    void redo();

private:
    ClipItem*   m_pClipItem;
    TrackItem*  m_pOldTrackItem;
    TrackItem*  m_pNewTrackItem;
    int         m_iOldStarting16th;
    int         m_iNewStarting16th;

};

#endif // CLIPCOMMANDS_H

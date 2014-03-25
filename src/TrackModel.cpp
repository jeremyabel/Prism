#include <QDebug>
#include <QList>

#include "TrackModel.h"

TrackModel::TrackModel( QString name, QColor color )
{
    sName  = name;
    qColor = color;
}


void TrackModel::insert( ClipModel *clip )
{
    // Check for intersections to the left
    ClipModel *leftClip = getLeftClip( clip );
    if ( leftClip != NULL && clip->starting16th < leftClip->ending16th )
    {
        // Put clip at the end of the left clip
        clip->setStarting16th( leftClip->ending16th );
    }

    // Check for intersections to the right
    ClipModel *rightClip = getRightClip( clip );
    if ( rightClip != NULL && rightClip->starting16th < clip->ending16th )
    {
        int rightShift = clip->ending16th - rightClip->starting16th;

        // Shift all clips to the right
        for ( int i = 0; i < pClips.length(); i++ )
        {
            if ( pClips[i]->starting16th > clip->starting16th )
            {
                pClips[i]->setStarting16th( pClips[i]->starting16th + rightShift );
            }
        }
    }

    // Add to list
    if ( pClips.length() <= 0 || pClips.last() == leftClip )
        pClips.append( clip );
    else if ( leftClip != NULL && pClips.last() != leftClip )
        pClips.insert( pClips.indexOf( leftClip ), clip );
    else
        pClips.insert( 0, clip );

    // Done
    qDebug() << "Clip added -" << pClips.length() << "clip(s) in track" << this->sName;
    emit clipInserted();
}


void TrackModel::remove( ClipModel *clip )
{
    pClips.removeOne( clip );
    qDebug() << "Clip removed -" << pClips.length() << "clip(s) in track" << this->sName;
}


ClipModel* TrackModel::getLeftClip( ClipModel *clip, bool greedy )
{
    QList<ClipModel*> results;

    for ( int i = 0; i < pClips.length(); i++ )
    {
        ClipModel* curResult = NULL;

        if ( results.size() <= 0 && pClips[i]->starting16th <= clip->starting16th )
            curResult = pClips[i];

        if ( curResult != NULL &&
             pClips[i]->starting16th >  curResult->starting16th &&
             pClips[i]->starting16th   <= clip->starting16th )
        {
            curResult = pClips[i];
        }

        if ( result && result != clip && greedy )
            break;
    }

    if ( result == clip )
        result = NULL;

    return result;
}


ClipModel* TrackModel::getRightClip( ClipModel *clip, bool greedy )
{
    ClipModel *result = NULL;

    for ( int i = 0; i < pClips.length(); i++ )
    {
        if ( result == NULL && pClips[i]->starting16th >= clip->starting16th )
            result = pClips[i];

        if ( result != NULL &&
             pClips[i]->starting16th <  result->starting16th &&
             pClips[i]->starting16th >= clip->starting16th )
        {
            result = pClips[i];
        }

        if ( result && result != clip && greedy )
           break;
    }

    if ( result == clip )
        result = NULL;

    return result;
}

#include <QDebug>
#include <QList>
#include <limits>

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
        clip->setStarting16th( leftClip->ending16th, true );
    }

    // Check for intersections to the right
    ClipModel *rightClip = getRightClip( clip );
    if ( rightClip != NULL && rightClip->starting16th < clip->ending16th )
    {
        int rightShift = clip->ending16th - rightClip->starting16th;

        // Shift all clips to the right
        for ( int i = 0; i < pClips.length(); i++ )
        {
            if ( pClips[i]->starting16th >= clip->starting16th )
            {
                pClips[i]->setStarting16th( pClips[i]->starting16th + rightShift, true );
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


ClipModel* TrackModel::getLeftClip( ClipModel *clip )
{
    QList<ClipModel*> results;

    // Get all clips to the left
    for ( int i = 0; i < pClips.length(); i++ )
    {
        if ( clip->starting16th > pClips[i]->starting16th && pClips[i] != clip )
        {
            results.append( pClips[i] );
        }
    }

    // Get closest one
    if ( results.size() > 0 )
    {
        ClipModel* result = NULL;
        int nearestEnd16th = 0;
        for ( int i = 0; i < results.size(); i++ )
        {
            if ( results[i]->ending16th > nearestEnd16th )
            {
                result = results[i];
                nearestEnd16th = result->ending16th;
            }
        }
        return result;
    }

    return NULL;
}


ClipModel* TrackModel::getRightClip( ClipModel *clip )
{
    QList<ClipModel*> results;

    // Get all clips to the right
    for ( int i = 0; i < pClips.length(); i++ )
    {
        if ( clip->starting16th <= pClips[i]->starting16th && pClips[i] != clip )
        {
            results.append( pClips[i] );
        }
    }

    // Get closest one
    if ( results.size() > 0 )
    {
        ClipModel* result = NULL;
        int nearestStart16th = INT_MAX;
        for ( int i = 0; i < results.size(); i++ )
        {
            if ( results[i]->starting16th < nearestStart16th )
            {
                result = results[i];
                nearestStart16th = result->starting16th;
            }
        }
        return result;
    }

    return NULL;
}

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

#include "FileManager.h"

bool FileManager::saveToFile( QString path, QList<TrackModel*> trackModels )
{
    QJsonObject jsonObject;
    QJsonArray  trackArray;

    for ( int i = 0; i < trackModels.size(); i++ )
        trackArray.append( trackModels[i]->serializeToJson() );

    // Put into json document
    jsonObject["tracks"] = trackArray;
    QJsonDocument jsonDoc = QJsonDocument( jsonObject );

    // Prep file
    QFile file;
    file.setFileName( path );

    // Open
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return false;

    // Write
    int result = file.write( jsonDoc.toJson() );
    qDebug() << "...wrote" << result << "bytes to" << path;

    return result != -1;
}


bool FileManager::open( QString path, QList<TrackModel*>* const trackModels )
{
    trackModels->clear();

    QString jsonFromFile;
    QFile file;
    file.setFileName( path );
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    jsonFromFile = file.readAll();
    file.close();

    // Get jsonObject out of jsonDoc
    QJsonDocument jsonDoc    = QJsonDocument::fromJson( jsonFromFile.toUtf8() );
    QJsonObject   jsonObject = jsonDoc.object();

    // Parse categories
    QJsonArray tracksArray = jsonObject.value("tracks").toArray();
    for ( int i = 0; i < tracksArray.size(); i++ )
    {
        QJsonObject trackObj    = tracksArray.at(i).toObject();
        TrackModel* trackModel  = new TrackModel( trackObj );
        trackModels->append( trackModel );
    }

    return trackModels->size() > 0;
}

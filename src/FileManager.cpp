#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

#include "FileManager.h"

bool FileManager::saveToFile( QString path, QString categoryPath, QList<TrackModel*> trackModels )
{
    QJsonObject jsonObject;
    QJsonArray  trackArray;

    for ( int i = 0; i < trackModels.size(); i++ )
        trackArray.append( trackModels[i]->serializeToJson() );

    // Put into json document
    jsonObject["categoryPath"] = categoryPath;
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


bool FileManager::open( QString path, QList<TrackModel*>* const trackModels, CategoryData* const categoryData )
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

    // Parse track data
    QJsonArray tracksArray = jsonObject.value("tracks").toArray();
    for ( int i = 0; i < tracksArray.size(); i++ )
    {
        QJsonObject trackObj    = tracksArray.at(i).toObject();
        TrackModel* trackModel  = new TrackModel( trackObj );
        trackModels->append( trackModel );
    }

    // Parse category data
    if ( jsonObject.contains("categoryPath") )
    {
        QString categoryPath = jsonObject.value("categoryPath").toString("");
        FileManager::import( categoryPath, categoryData );
        qDebug() << "Loaded category data from" << categoryPath;
    }
    else
        qDebug() << "No category data found...";

    return trackModels->size() > 0;
}


bool FileManager::import( QString path, CategoryData* categoryData )
{
    QString jsonFromFile;
    QFile file;
    file.setFileName( path );
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    jsonFromFile = file.readAll();
    file.close();

    // Get jsonObject out of jsonDoc
    QJsonDocument jsonDoc    = QJsonDocument::fromJson( jsonFromFile.toUtf8() );
    QJsonObject   jsonObject = jsonDoc.object();

    categoryData = new CategoryData( jsonObject );
    categoryData->path = path;
    return true;
}
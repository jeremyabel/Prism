#include <math.h>

#include <QDebug>
#include <QXmlStreamWriter>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include <QMap>
#include <QTime>
#include <QUuid>
#include <QVariant>

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


bool FileManager::open( QString path, QList<TrackModel*>* const trackModels, CategoryData* const categoryData, ImageData* const imageData )
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
        FileManager::import( categoryPath, categoryData, imageData );
        qDebug() << "Loaded category data from" << categoryPath;
    }
    else
        qDebug() << "No category data found...";

    return trackModels->size() > 0;
}


bool FileManager::import( QString path, CategoryData* categoryData, ImageData* imageData )
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

    categoryData->initWithJson( jsonObject );
    categoryData->path = path;

    imageData->initWithJson( jsonObject );

    return true;
}


bool FileManager::exportToXML( QString path, QList<TrackModel *>* trackModels, const CategoryData* categoryData, ImageData* const imageData, float bpm, float fps )
{
    QString xmlOutput;
    int     imgCount = 0;

    QXmlStreamWriter stream(&xmlOutput);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    // Make XML...
    stream.writeStartElement("xmeml");
    stream.writeAttribute("version", "4");
        stream.writeStartElement("project");
            stream.writeTextElement("name", "Chakra_Test");
            stream.writeStartElement("children");
                stream.writeStartElement("bin");
                    stream.writeTextElement("name", "FROM PRISM");
                    stream.writeStartElement("children");
                        stream.writeStartElement("bin");
                            stream.writeTextElement("name", "STILLS");
                            stream.writeStartElement("children");

                            // Organize images into category bins
                            for ( int i = 0; i < categoryData->categoryList->size(); i++ )
                            {
                                CategoryModel* catModel = categoryData->categoryList->at(i);

                                stream.writeStartElement("bin");
                                    stream.writeTextElement("name", catModel->name);
                                    stream.writeStartElement("children");

                                        // Get category images
                                        QueryMap query;
                                        query.insert(CATEGORY, QVariant(catModel->name));
                                        QList<ImageModel> imageResults = imageData->makeQuery(query);

                                        // Write clip data
                                        for ( int j = 0; j < imageResults.size(); j++ )
                                        {
                                            ImageModel img = imageResults.at(j);
                                            img.clipID = "masterclip-" + QString::number(imgCount);
                                            img.fileID = "file-" + QString::number(imgCount);

                                            stream.writeStartElement("clip");
                                            stream.writeAttribute("id", img.clipID);

                                                QString uuidString = QUuid::createUuid().toString();
                                                uuidString.remove(0, 1);
                                                uuidString.remove(uuidString.length() - 1, 1);
                                                stream.writeTextElement("uuid", uuidString);

                                                stream.writeTextElement("masterclipid", img.clipID);
                                                stream.writeTextElement("ismasterclip", "TRUE");
                                                stream.writeTextElement("duration", QString::number(150));
                                                stream.writeStartElement("rate");
                                                    stream.writeTextElement("timebase", QString::number(30));
                                                    stream.writeTextElement("ntsc", "TRUE");
                                                stream.writeEndElement(); // rate
                                                stream.writeTextElement("in", "0");
                                                stream.writeTextElement("out", "0");
                                                stream.writeTextElement("name", img.name);
                                                stream.writeStartElement("media");
                                                    stream.writeStartElement("video");
                                                        stream.writeStartElement("track");
                                                            stream.writeStartElement("clipitem");
                                                                stream.writeAttribute("id", img.clipID);

                                                                stream.writeTextElement("masterclipid", img.clipID);
                                                                stream.writeTextElement("name", img.name);
                                                                stream.writeTextElement("alphatype", "none");
                                                                stream.writeStartElement("file");
                                                                stream.writeAttribute("id", img.fileID);

                                                                    stream.writeTextElement("name", img.name);
                                                                    stream.writeTextElement("pathurl", img.path);
                                                                    stream.writeStartElement("rate");
                                                                        stream.writeTextElement("timebase", QString::number(30));
                                                                        stream.writeTextElement("ntsc", "TRUE");
                                                                    stream.writeEndElement(); // rate

                                                                    stream.writeStartElement("timecode");
                                                                        stream.writeStartElement("rate");
                                                                            stream.writeTextElement("timebase", QString::number(30));
                                                                            stream.writeTextElement("ntsc", "TRUE");
                                                                        stream.writeEndElement(); // rate
                                                                        stream.writeTextElement("string", "00;00;00;00");
                                                                        stream.writeTextElement("frame", "0");
                                                                        stream.writeTextElement("displayformat", "DF");
                                                                        stream.writeStartElement("reel");
                                                                            stream.writeEmptyElement("name");
                                                                        stream.writeEndElement(); // reel
                                                                    stream.writeEndElement(); // timecode

                                                                    stream.writeStartElement("media");
                                                                        stream.writeStartElement("video");
                                                                            stream.writeTextElement("duration", "18000");
                                                                            stream.writeStartElement("samplecharacteristics");
                                                                                stream.writeStartElement("rate");
                                                                                    stream.writeTextElement("timebase", QString::number(30));
                                                                                    stream.writeTextElement("ntsc", "TRUE");
                                                                                stream.writeEndElement(); // rate
                                                                                stream.writeTextElement("width", "1024");
                                                                                stream.writeTextElement("height", "768");
                                                                                stream.writeTextElement("anamorphic", "FALSE");
                                                                                stream.writeTextElement("pixelaspectratio", "square");
                                                                                stream.writeTextElement("fielddominance", "none");
                                                                            stream.writeEndElement(); // samplecharacteristics
                                                                        stream.writeEndElement(); // video
                                                                    stream.writeEndElement(); // media

                                                                stream.writeEndElement(); // file

                                                                stream.writeStartElement("link");
                                                                    stream.writeTextElement("linkclipref", img.clipID);
                                                                    stream.writeTextElement("mediatype", "video");
                                                                    stream.writeTextElement("trackindex", "1");
                                                                    stream.writeTextElement("clipindex", "1");
                                                                stream.writeEndElement(); // link

                                                            stream.writeEndElement(); // clipitem
                                                        stream.writeEndElement(); // track
                                                    stream.writeEndElement(); // video
                                                stream.writeEndElement(); // media

                                                stream.writeStartElement("logginginfo");
                                                    stream.writeEmptyElement("description");
                                                    stream.writeEmptyElement("scene");
                                                    stream.writeEmptyElement("shottake");
                                                    stream.writeEmptyElement("lognote");
                                                stream.writeEndElement(); // logginginfo

                                            stream.writeEndElement(); // clip

                                            imgCount++;
                                        } // images

                                    stream.writeEndElement(); // children
                                stream.writeEndElement(); // bin

                            } // categories

                            // ...Sequence time!!
                            stream.writeStartElement("sequence");

                                QString uuidString = QUuid::createUuid().toString();
                                uuidString.remove(0, 1);
                                uuidString.remove(uuidString.length() - 1, 1);
                                stream.writeTextElement("uuid", uuidString);

                                // TODO: Put stuff here
                                stream.writeTextElement("duration", "150");
                                stream.writeStartElement("rate");
                                    stream.writeTextElement("timebase", QString::number(lroundf(fps)));
                                    stream.writeTextElement("ntsc", "FALSE");
                                stream.writeEndElement(); // rate
                                stream.writeTextElement("name", "prism-sequence-1");

                                stream.writeStartElement("media");
                                    stream.writeStartElement("video");

                                        // Format data
                                        stream.writeStartElement("format");
                                            stream.writeStartElement("samplecharacteristics");
                                                stream.writeStartElement("rate");
                                                    stream.writeTextElement("timebase", QString::number(lroundf(fps)));
                                                    stream.writeTextElement("ntsc", "FALSE");
                                                stream.writeEndElement(); // rate
                                                stream.writeStartElement("codec");
                                                    stream.writeTextElement("name", "Apple ProRes 422");
                                                    stream.writeStartElement("appspecificdata");
                                                        stream.writeTextElement("appname", "Final Cut Pro");
                                                        stream.writeTextElement("appmanufacturer", "Apple Inc.");
                                                        stream.writeTextElement("appversion", "7.0");
                                                        stream.writeStartElement("data");
                                                            stream.writeStartElement("qtcodec");
                                                                stream.writeTextElement("codecname", "Apple ProRes 422");
                                                                stream.writeTextElement("codectypename", "Apple ProRes 422");
                                                                stream.writeTextElement("codectypecode", "apcn");
                                                                stream.writeTextElement("codecvendorcode", "appl");
                                                                stream.writeTextElement("spatialquality", "1024");
                                                                stream.writeTextElement("temporalquality", "0");
                                                                stream.writeTextElement("keyframerate", "0");
                                                                stream.writeTextElement("datarate", "0");
                                                            stream.writeEndElement(); // qtcodec
                                                        stream.writeEndElement(); // data
                                                    stream.writeEndElement(); // appspecificdata
                                                stream.writeEndElement(); // codec
                                                stream.writeTextElement("width", "1920");
                                                stream.writeTextElement("height", "1080");
                                                stream.writeTextElement("anamorphic", "FALSE");
                                                stream.writeTextElement("pixelaspectratio", "square");
                                                stream.writeTextElement("fielddominance", "none");
                                                stream.writeTextElement("colordepth", "24");
                                            stream.writeEndElement(); // samplecharacteristics
                                        stream.writeEndElement(); // format

                                        // Set random seed
                                        qsrand((uint)QTime::currentTime().msec());

                                        // Insert track data
                                        for ( int i = 0; i < trackModels->size(); i++ )
                                        {
                                            stream.writeStartElement("track");
                                            stream.writeTextElement("enabled", "TRUE");
                                            stream.writeTextElement("locked", "FALSE");

                                            const TrackModel* trackModel = trackModels->at(i);

                                            // Insert clip data
                                            for ( int j = 0; j < trackModel->pClips.size(); j++ )
                                            {
                                                ClipModel* clipModel        = trackModel->pClips.at(j);
                                                QList<ImageModel> images    = imageData->makeQuery( clipModel->getImageQuery() );

                                                int startFrame  = ClipModel::getFrameFrom16th(clipModel->starting16th, bpm, fps);
                                                int endFrame    = ClipModel::getFrameFrom16th(clipModel->ending16th,   bpm, fps);
                                                int invDiv      = 16 / clipModel->distro16th;
                                                int nImages     = (int)floor((endFrame - startFrame) / invDiv);

                                                // One clip for each image
                                                for ( int k = 0; k < nImages; k++ )
                                                {
                                                    // Select image to use
                                                    // TODO: This is non-exhaustive!
                                                    ImageModel imgModel = images.at(qrand() % images.size());

                                                    int imgStart16th    = clipModel->starting16th + k * invDiv;
                                                    int imgEnd16th      = imgStart16th + invDiv;
                                                    int imgStartFrame   = ClipModel::getFrameFrom16th(imgStart16th, bpm, fps);
                                                    int imgEndFrame     = ClipModel::getFrameFrom16th(imgEnd16th,   bpm, fps);

                                                    stream.writeStartElement("clipitem");
                                                    stream.writeAttribute("id", "clipitem-" + QString::number(j * 10 + k));
                                                        stream.writeTextElement("masterclipid", imgModel.clipID);
                                                        stream.writeTextElement("name",         imgModel.name);
                                                        stream.writeTextElement("enabled",      "TRUE");
                                                        stream.writeTextElement("duration",     "2589410");
                                                        stream.writeTextElement("start",        QString::number(imgStartFrame));
                                                        stream.writeTextElement("end",          QString::number(imgEndFrame));
                                                        stream.writeTextElement("in",           "0");
                                                        stream.writeTextElement("out",          QString::number(imgEndFrame - imgStartFrame));
                                                        stream.writeTextElement("alphatype",    "none");
                                                        stream.writeEmptyElement("file");
                                                        stream.writeAttribute("id",             imgModel.fileID);
                                                    stream.writeEndElement(); // clipitem
                                                }
                                            }

                                            stream.writeEndElement(); // track
                                        }

                                    stream.writeEndElement(); // video

                                    // Timecode data
                                    stream.writeStartElement("timecode");
                                        stream.writeStartElement("rate");
                                            stream.writeTextElement("timebase", QString::number(lroundf(fps)));
                                            stream.writeTextElement("ntsc", "FALSE");
                                        stream.writeEndElement(); // rate
                                        stream.writeTextElement("string", "00:00:00:00");
                                        stream.writeTextElement("frame", "0");
                                        stream.writeTextElement("displayformat", "NDF");
                                    stream.writeEndElement(); // timecode

                                stream.writeEndElement(); // media

                            stream.writeEndElement(); // sequence

                            stream.writeEndElement(); // children
                        stream.writeEndElement(); // bin
                    stream.writeEndElement(); // children
                stream.writeEndElement(); // bin
            stream.writeEndElement(); // children
        stream.writeEndElement(); // project
    stream.writeEndElement(); // xmeml

    stream.writeEndDocument();

    qDebug() << xmlOutput;

    return true;
}

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QList>

#include "CategoryData.h"
#include "ImageData.h"
#include "TrackModel.h"

class FileManager
{
public:
    static bool     saveToFile( QString path, QString categoryPath, QList<TrackModel*> trackModels );
    static bool     open( QString path, QList<TrackModel*>* const trackModels, CategoryData* const categoryData, ImageData* const imageData );
    static bool     import( QString path, CategoryData* categoryData, ImageData* imageData );
    static bool     exportToXML( QString path, QList<TrackModel*>* trackModels, const CategoryData* categoryData, ImageData* const imageData, float bpm = 92.0f, float fps = 60.0f );

private:
    FileManager();
};

#endif // FILEMANAGER_H

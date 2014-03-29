#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QList>

#include "CategoryData.h"
#include "TrackModel.h"

class FileManager
{
public:
    static bool     saveToFile( QString path, QString categoryPath, QList<TrackModel*> trackModels );
    static bool     open( QString path, QList<TrackModel*>* const trackModels, CategoryData* const categoryData );
    static bool     import( QString path, CategoryData* categoryData );

private:
    FileManager();
};

#endif // FILEMANAGER_H

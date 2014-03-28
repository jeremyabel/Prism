#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QList>
#include "TrackModel.h"

class FileManager
{
public:
    static bool saveToFile( QString filename, QList<TrackModel*> trackModels );

private:
    FileManager();
};

#endif // FILEMANAGER_H

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QList>
#include "TrackModel.h"

class FileManager
{
public:
    static bool     saveToFile( QString path, QList<TrackModel*> trackModels );
    static bool     open( QString path, QList<TrackModel*>* const trackModels );

private:
    FileManager();
};

#endif // FILEMANAGER_H

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QList>
#include "TrackModel.h"

class FileManager
{
public:
    static bool                 saveToFile( QString path, QList<TrackModel*> trackModels );
    static QList<TrackModel*>   open( QString path );

private:
    FileManager();
};

#endif // FILEMANAGER_H

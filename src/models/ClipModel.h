#ifndef CLIPMODEL_H
#define CLIPMODEL_H

#include <QJsonObject>
#include <QList>
#include <QMap>

#include "ImageData.h"

class ClipModel
{
public:
    ClipModel( int start16th, int length16ths );
    ClipModel( QJsonObject jsonObject );

    QJsonObject     serializeToJson();
    QueryMap        getImageQuery();
    QString         getStatusMessage();

    static int      getFrameFrom16th( int note16th, float bpm, float fps );

    void            setStarting16th( int value, bool force = false );
    void            setEnding16th( int value );

    int             starting16th;
    int             length16th;
    int             ending16th;
    int             distro16th;

    bool            enableCategory;
    bool            enableSubCategory;
    bool            enableColor;
    bool            enableAge;
    bool            enableSize;
    bool            enableBroken;
    bool            enableMissingParts;
    bool            enableBatteries;

    QString         category;
    QString         subCategory;
    QString         color;
    QString         year;
    int             size;
    bool            broken;
    bool            missingParts;
    bool            batteries;
};

#endif // CLIPMODEL_H

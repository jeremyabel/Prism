#ifndef CLIPMODEL_H
#define CLIPMODEL_H

#include <QJsonObject>

class ClipModel
{
public:
    ClipModel( int start16th, int length16ths )
    {
       starting16th         = start16th;
       length16th           = length16ths;
       ending16th           = starting16th + length16th;

       enableCategory       = false;
       enableSubCategory    = false;
       enableColor          = false;
       enableAge            = false;
       enableSize           = false;
       enableBroken         = false;
       enableMissingParts   = false;
       enableBatteries      = false;

       category             = "";
       subCategory          = "";
       color                = "";
       year                 = "";
       size                 = 0;
       broken               = false;
       missingParts         = false;
       batteries            = false;
    }

    QJsonObject serializeToJson()
    {
        QJsonObject jsonObject;

        jsonObject["starting16th"] = starting16th;
        jsonObject["length16ths"]  = length16th;

        if ( enableCategory )       jsonObject["category"]      = category;
        if ( enableSubCategory )    jsonObject["subcategory"]   = subCategory;
        if ( enableColor )          jsonObject["color"]         = color;
        if ( enableAge )            jsonObject["year"]          = year;
        if ( enableSize )           jsonObject["size"]          = size;
        if ( enableBroken )         jsonObject["broken"]        = broken;
        if ( enableMissingParts )   jsonObject["missingParts"]  = missingParts;
        if ( enableBatteries )      jsonObject["batteries"]     = batteries;

        return jsonObject;
    }

    void setStarting16th( int value, bool force = false )
    {
        if ( value >= ending16th && !force )
            return;

        starting16th = value;
        ending16th   = starting16th + length16th;
    }

    void setEnding16th( int value )
    {
        if ( value <= starting16th )
            return;

        ending16th = value;
        length16th = ending16th - starting16th;
    }

    int     starting16th;
    int     length16th;
    int     ending16th;

    bool    enableCategory;
    bool    enableSubCategory;
    bool    enableColor;
    bool    enableAge;
    bool    enableSize;
    bool    enableBroken;
    bool    enableMissingParts;
    bool    enableBatteries;

    QString category;
    QString subCategory;
    QString color;
    QString year;
    int     size;
    bool    broken;
    bool    missingParts;
    bool    batteries;
};

#endif // CLIPMODEL_H

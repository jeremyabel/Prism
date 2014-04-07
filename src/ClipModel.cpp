#include <math.h>

#include "ClipModel.h"


ClipModel::ClipModel( int start16th, int length16ths )
{
   starting16th         = start16th;
   length16th           = length16ths;
   ending16th           = starting16th + length16th;
   distro16th           = 8;

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

ClipModel::ClipModel( QJsonObject jsonObject )
{
    starting16th        = jsonObject.value("starting16th").toInt(0);
    length16th          = jsonObject.value("length16ths").toInt(4);
    distro16th          = jsonObject.value("distro16ths").toInt(8);
    ending16th          = starting16th + length16th;

    enableCategory      = jsonObject.contains("category");
    enableSubCategory   = jsonObject.contains("subcategory");
    enableColor         = jsonObject.contains("color");
    enableAge           = jsonObject.contains("year");
    enableSize          = jsonObject.contains("size");
    enableBroken        = jsonObject.contains("broken");
    enableMissingParts  = jsonObject.contains("missingParts");
    enableBatteries     = jsonObject.contains("batteries");

    category            = jsonObject.value("category").toString("");
    subCategory         = jsonObject.value("subcategory").toString("");
    color               = jsonObject.value("color").toString("");
    year                = jsonObject.value("year").toString("");
    size                = jsonObject.value("size").toInt(1);
    broken              = jsonObject.value("broken").toBool();
    missingParts        = jsonObject.value("missingParts").toBool();
    batteries           = jsonObject.value("batteries").toBool();
}

QJsonObject ClipModel::serializeToJson()
{
    QJsonObject jsonObject;

    jsonObject["starting16th"] = starting16th;
    jsonObject["length16ths"]  = length16th;
    jsonObject["distro16ths"]  = distro16th;

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

QueryMap ClipModel::getImageQuery()
{
    QueryMap query;

    if ( enableCategory )       query.insert( CATEGORY,     QVariant(category) );
    if ( enableSubCategory)     query.insert( SUBCATEGORY,  QVariant(subCategory) );
    if ( enableColor )          query.insert( COLOR,        QVariant(color) );
    if ( enableAge )            query.insert( YEAR,         QVariant(year) );
    if ( enableSize )           query.insert( SIZE,         QVariant(size) );
    if ( enableBroken )         query.insert( BROKEN,       QVariant(broken) );
    if ( enableMissingParts )   query.insert( MISSING,      QVariant(missingParts) );
    if ( enableBatteries )      query.insert( BATTERIES,    QVariant(batteries) );

    return query;
}

QString ClipModel::getStatusMessage()
{
    QueryMap queryMap   = getImageQuery();
    QString message     = "";

    bool multiQuery = queryMap.size() > 1;
    while ( queryMap.size() > 0 )
    {
        if ( message.length() > 99 )
        {
            message += "...";
            break;
        }

        if      ( queryMap.contains(CATEGORY) )
            message += "Category: '"     + queryMap.take(CATEGORY).toString()    + "' ";

        else if ( queryMap.contains(SUBCATEGORY) )
            message += "Sub-Category: '" + queryMap.take(SUBCATEGORY).toString() + "' ";

        else if ( queryMap.contains(COLOR) )
            message += "Color: "         + queryMap.take(COLOR).toString().toLower();

        else if ( queryMap.contains(YEAR) )
            message += "Year: "          + queryMap.take(YEAR).toString();

        else if ( queryMap.contains(SIZE) )
            message += "Size: "          + QString::number( queryMap.take(SIZE).toInt() );

        else if ( queryMap.contains(BROKEN) )
            message += "Broken: "        + queryMap.take(BROKEN).toString().toLower();

        else if ( queryMap.contains(MISSING) )
            message += "Missing: "       + queryMap.take(MISSING).toString().toLower();

        else if ( queryMap.contains(BATTERIES) )
            message += "Batteries: "     + queryMap.take(BATTERIES).toString().toLower();

        if ( queryMap.size() >= 1 && multiQuery )
            message += ",  ";
    }

    if ( message.length() <= 0 )
        return "Any";
    else
        return message;
}

int ClipModel::getFrameFrom16th( int note16th, float bpm, float fps )
{
    float seconds = (float)note16th * (15.0f / bpm);
    return lroundf(seconds * fps);
}

void ClipModel::setStarting16th( int value, bool force )
{
    if ( value >= ending16th && !force )
        return;

    starting16th = value;
    ending16th   = starting16th + length16th;
}

void ClipModel::setEnding16th( int value )
{
    if ( value <= starting16th )
        return;

    ending16th = value;
    length16th = ending16th - starting16th;
}

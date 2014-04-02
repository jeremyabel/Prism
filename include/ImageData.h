#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QVariant>

#include "sqlite3.h"

class ImageData
{
public:

    enum ParameterType
    {
        CATEGORY,
        SUBCATEGORY,
        COLOR,
        YEAR,
        SIZE,
        BROKEN,
        MISSING,
        BATTERIES
    };

    ImageData()
    {
        m_paramToStringMap.insert(CATEGORY,     "CATEGORY");
        m_paramToStringMap.insert(SUBCATEGORY,  "SUBCATEGORY");
        m_paramToStringMap.insert(COLOR,        "COLOR");
        m_paramToStringMap.insert(YEAR,         "YEAR");
        m_paramToStringMap.insert(SIZE,         "SIZE");
        m_paramToStringMap.insert(BROKEN,       "BROKEN");
        m_paramToStringMap.insert(MISSING,      "MISSING");
        m_paramToStringMap.insert(BATTERIES,    "BATTERIES");
    }

    void initWithJson( QJsonObject jsonObject );
    void close();

    QList<QString> makeQuery( QMap<ParameterType, QVariant> queryMap );

private:
    int  boolToInt( bool value ) { return value ? 1 : 0; }

    QMap<ParameterType, QString> m_paramToStringMap;
    sqlite3* m_pDatabase;

};

#endif // IMAGEDATA_H

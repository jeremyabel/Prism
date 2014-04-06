#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QJsonObject>
#include <QHash>
#include <QList>
#include <QMap>
#include <QVariant>

#include "sqlite3.h"

struct ImageModel
{
    QString path;
    QString name;
    QString masterID;
    QString clipID;
    QString fileID;
};

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

typedef QList< QMap<QString, QString> > StringMapList;
typedef QMap<ParameterType, QVariant>   QueryMap;

class ImageData
{
public:
    void initWithJson( QJsonObject jsonObject );
    void close();

    QList<ImageModel> makeQuery( QueryMap queryMap );

private:
    int boolToInt( bool value ) const { return value ? 1 : 0; }

    sqlite3* m_pDatabase;
    QHash<QString, QList<ImageModel> > m_prevQueries;

};

#endif // IMAGEDATA_H

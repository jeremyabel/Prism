#include <QDebug>
#include <QJsonArray>

#include "ImageData.h"


static int callback( void *data, int argc, char **argv, char **azColName )
{
    // Cast void* data to something useful
    QList< QMap<QString, QString> >* dataList = static_cast<QList< QMap<QString, QString> >*>(data);
    QMap<QString, QString> dataMap;

    // Inject sql data
    for ( int i = 0; i < argc; i++ )
        dataMap.insert( azColName[i], argv[i] ? argv[i] : "NULL" );

    if ( argc > 0 )
        dataList->append( dataMap );

    return 0;
}


void ImageData::initWithJson( QJsonObject jsonObject )
{
    qDebug() << "Setting up database...";

    if ( sqlite3_open( "imagedata.db", &m_pDatabase ) )
        return;

    char* zErrMsg = 0;

    // Create a database in tempfile-land
    if ( sqlite3_open( "", &m_pDatabase ) )
        return;

    qDebug() << "Database opened";

    const char* sql =   "CREATE TABLE IMAGES(" \
                        "ID INT PRIMARY KEY         NOT NULL," \
                        "NAME           TEXT        NOT NULL," \
                        "PATH           TEXT        NOT NULL," \
                        "MASTER_ID      TEXT        NOT NULL," \
                        "CLIP_ID        TEXT        NOT NULL," \
                        "FILE_ID        TEXT        NOT NULL," \
                        "CATEGORY       TEXT," \
                        "SUBCATEGORY    TEXT," \
                        "COLOR          TEXT," \
                        "YEAR           TEXT," \
                        "SIZE           INT,"  \
                        "BROKEN         INT,"  \
                        "MISSING        INT,"  \
                        "BATTERIES      INT,"  \
                        "USED           INT);";

    // Execute SQL
    if ( sqlite3_exec(m_pDatabase, sql, callback, 0, &zErrMsg) != SQLITE_OK )
        return;

    qDebug() << "Table created successfully";

    QString sqlString = "";
    QJsonArray imagesArray = jsonObject.value("images").toArray();
    for ( int i = 0; i < imagesArray.count(); i++ )
    {
        QJsonObject imageObj = imagesArray.at(i).toObject();

        sqlString += "INSERT INTO IMAGES (ID,NAME,PATH,MASTER_ID,CLIP_ID,FILE_ID,CATEGORY,SUBCATEGORY,COLOR,YEAR,SIZE,BROKEN,MISSING,BATTERIES,USED) ";
        sqlString += "VALUES (";
        sqlString += QString::number(i)                             + ", ";
        sqlString += "'" + imageObj["name"].toString()        + "'" + ", ";
        sqlString += "'" + imageObj["path"].toString()        + "'" + ", ";
        sqlString += "'masterclip-" + QString::number(i + 1)  + "'" + ", ";
        sqlString += "'clipitem-"   + QString::number(i + 1)  + "'" + ", ";
        sqlString += "'file-"       + QString::number(i + 1)  + "'" + ", ";
        sqlString += "'" + imageObj["category"].toString()    + "'" + ", ";
        sqlString += "'" + imageObj["subcategory"].toString() + "'" + ", ";
        sqlString += "'" + imageObj["color"].toString()       + "'" + ", ";
        sqlString += "'" + imageObj["year"].toString()        + "'" + ", ";
        sqlString += QString::number( imageObj["size"].toInt() )                     + ", ";
        sqlString += QString::number( boolToInt(imageObj["broken"].toBool()) )       + ", ";
        sqlString += QString::number( boolToInt(imageObj["missingParts"].toBool()) ) + ", ";
        sqlString += QString::number( boolToInt(imageObj["batteries"].toBool()) )    + ", ";
        sqlString += "0 );";
    }

    // Execute SQL
    int rc = sqlite3_exec(m_pDatabase, sqlString.toLocal8Bit().data(), callback, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        qDebug() << "SQL Error:" << zErrMsg;
        sqlite3_free(zErrMsg);
        return;
    }

    m_bIsReady = imagesArray.count() > 0;
    qDebug() << imagesArray.count() << "records created successfully";
}


QList<ImageModel> ImageData::makeQuery( QueryMap queryMap, bool forceUnused )
{
    if ( !m_bIsReady )
        return QList<ImageModel>();

    char* zErrMsg = 0;
    StringMapList* data = new StringMapList();

    // Formulate query
    QString sqlQuery = getSqlStringFromQuery( queryMap, forceUnused );

    // Look to see if we've done this query before
    if ( m_prevQueries.contains(sqlQuery) && !forceUnused )
    {
        qDebug() << "Found existing data for query:" << sqlQuery;
        return m_prevQueries.value( sqlQuery );
    }

    qDebug() << "Making query:" << sqlQuery;

    // Make query
    int rc = sqlite3_exec(m_pDatabase, sqlQuery.toLocal8Bit().data(), callback, (void*)data, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        qDebug() << "SQL Error:" << zErrMsg;
        sqlite3_free(zErrMsg);
    }

    // Cast void* data to something useful
    StringMapList* dataList = static_cast<StringMapList*>(data);

    // Save paths
    QList<ImageModel> results;
    for ( int i = 0; i < dataList->size(); i++ )
    {
        ImageModel model;
        model.path      = dataList->at(i).value("PATH");
        model.name      = dataList->at(i).value("NAME");
        model.masterID  = dataList->at(i).value("MASTER_ID");
        model.clipID    = dataList->at(i).value("CLIP_ID");
        model.fileID    = dataList->at(i).value("FILE_ID");
        results.append(model);
    }

    // Add to the list of previous queries
    m_prevQueries.insert(sqlQuery, results);

    qDebug() << "Found" << results.size() << "matching images";
    return results;
}


QString ImageData::getSqlStringFromQuery( QueryMap queryMap, bool forceUnused )
{
    // Formulate query
    QString sqlQuery = "SELECT * from IMAGES ";

    if ( forceUnused )
    {
        sqlQuery += "where USED='0' ";
        sqlQuery += (queryMap.size() > 0 ? "AND " : "");
    }
    else
        sqlQuery += (queryMap.size() > 0 ? "where " : "");

    bool multiQuery = queryMap.size() > 1;
    while ( queryMap.size() > 0 )
    {
        if      ( queryMap.contains(CATEGORY) )
            sqlQuery += "CATEGORY='"    + queryMap.take(CATEGORY).toString()    + "' ";

        else if ( queryMap.contains(SUBCATEGORY) )
            sqlQuery += "SUBCATEGORY='" + queryMap.take(SUBCATEGORY).toString() + "' ";

        else if ( queryMap.contains(COLOR) )
            sqlQuery += "COLOR='"       + queryMap.take(COLOR).toString()       + "' ";

        else if ( queryMap.contains(YEAR) )
            sqlQuery += "YEAR='"        + queryMap.take(YEAR).toString()        + "' ";

        else if ( queryMap.contains(SIZE) )
            sqlQuery += "SIZE="         + QString::number( queryMap.take(SIZE).toInt() ) + " ";

        else if ( queryMap.contains(BROKEN) )
            sqlQuery += "BROKEN="       + QString::number( boolToInt( queryMap.take(BROKEN).toBool() ) ) + " ";

        else if ( queryMap.contains(MISSING) )
            sqlQuery += "MISSING="      + QString::number( boolToInt( queryMap.take(MISSING).toBool() ) ) + " ";

        else if ( queryMap.contains(BATTERIES) )
            sqlQuery += "BATTERIES="    + QString::number( boolToInt( queryMap.take(BATTERIES).toBool() ) ) + " ";

        if ( queryMap.size() >= 1 && multiQuery )
            sqlQuery += "AND ";
    }

    return sqlQuery.simplified();
}


void ImageData::setImageUsedState( ImageModel targetImage, bool used )
{
    QString usedString = QString::number(boolToInt(used));
    QString sqlQuery   = "UPDATE IMAGES SET USED='" + usedString + "' WHERE PATH='" + targetImage.path + "'";

    char* zErrMsg = 0;
    StringMapList* data = new StringMapList();

    // Make query
    int rc = sqlite3_exec(m_pDatabase, sqlQuery.toLocal8Bit().data(), callback, (void*)data, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        qDebug() << "SQL Error:" << zErrMsg;
        sqlite3_free(zErrMsg);
    }
}


void ImageData::setQueryAsUnused( QueryMap query )
{
    qDebug() << "Setting query to unused";

    QList<ImageModel> results = makeQuery(query);

    for ( int i = 0; i < results.size(); i++ )
        setImageUsedState( results.at(i), false );

    // Remove from cached list
    QString sqlQuery = getSqlStringFromQuery(query, true);
    if ( m_prevQueries.contains(sqlQuery) )
        m_prevQueries.remove(sqlQuery);
}


void ImageData::close()
{
    if ( m_bIsReady )
    {
        qDebug() << "ImageData close";
        sqlite3_close(m_pDatabase);
    }

    m_bIsReady = false;
}


bool ImageData::isReady()
{
    return m_bIsReady;
}


uint qHash( const QueryMap &key )
{
    Q_UNUSED(key);
    return 0;
}

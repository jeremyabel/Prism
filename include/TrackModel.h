#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QList>
#include <QColor>

#include "ClipModel.h"

class TrackModel : public QObject
{
    Q_OBJECT

public:
    TrackModel();
    TrackModel( QString name, QColor color = QColor( Qt::red ) );
    TrackModel( QJsonObject json );

    QJsonObject         serializeToJson();

    void                insert( ClipModel* clip );
    void                remove( ClipModel* clip );
    ClipModel*          getLeftClip(  ClipModel* clip );
    ClipModel*          getRightClip( ClipModel* clip );

    QString             sName;
    QList<ClipModel*>   pClips;
    QColor              qColor;

signals:
    void                clipInserted();
    void                clipRemoved();
};

#endif // TRACKMODEL_H

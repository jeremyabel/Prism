#ifndef CATEGORYDATA_H
#define CATEGORYDATA_H

#include <QList>
#include <QVariant>
#include <QJsonObject>

class CategoryModel
{
public:
    CategoryModel() { subcategories = new QList<QVariant>(); }

    QString name;
    QList<QVariant> *subcategories;
};

class CategoryData
{
public:
    CategoryData( QJsonObject jsonObject );

    QList<CategoryModel*> categoryList;
    QString path;
};

#endif // CATEGORYDATA_H

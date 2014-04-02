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
    int     index;
    QList<QVariant> *subcategories;
};


class CategoryData
{
public:
    CategoryData();
    void initWithJson( QJsonObject jsonObject );

    const CategoryModel* getCategoryByName( const QString name ) const;

    QList<CategoryModel*>* categoryList;
    QString path;
};

#endif // CATEGORYDATA_H

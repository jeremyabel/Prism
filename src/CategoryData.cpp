#include <QDebug>
#include <QJsonArray>

#include "CategoryData.h"

CategoryData::CategoryData( QJsonObject jsonObject )
{
    path = "";

    QJsonArray categoriesArray = jsonObject.value("categories").toArray();
    for ( int i = 0; i < categoriesArray.count(); i++ )
    {
        QJsonObject categoryObj = categoriesArray.at(i).toObject();
        CategoryModel *newCategoryModel = new CategoryModel();

        newCategoryModel->name = categoryObj["name"].toString();

        // Add subcategories
        QList<QVariant> *subcats = new QList<QVariant>( categoryObj["subcategories"].toArray().toVariantList() );
        newCategoryModel->subcategories = subcats;

        categoryList.append(newCategoryModel);
    }

    qDebug() << "Parsed" << categoryList.size() << "categories";
}

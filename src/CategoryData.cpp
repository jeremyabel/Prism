#include <QDebug>
#include <QJsonArray>

#include "CategoryData.h"

CategoryData::CategoryData( )
{

}


void CategoryData::initWithJson( QJsonObject jsonObject )
{
    path = "";
    categoryList = new QList<CategoryModel*>();

    QJsonArray categoriesArray = jsonObject.value("categories").toArray();
    for ( int i = 0; i < categoriesArray.count(); i++ )
    {
        QJsonObject categoryObj = categoriesArray.at(i).toObject();
        CategoryModel* newCategoryModel = new CategoryModel();

        newCategoryModel->name  = categoryObj["name"].toString();
        newCategoryModel->index = categoryList->size();

        // Add subcategories
        QList<QVariant>* subcats = new QList<QVariant>( categoryObj["subcategories"].toArray().toVariantList() );
        newCategoryModel->subcategories = subcats;

        categoryList->append( newCategoryModel );
    }

    qDebug() << "Parsed" << categoryList->size() << "categories";
}


const CategoryModel* CategoryData::getCategoryByName( const QString name ) const
{
    for ( int i = 0; i < categoryList->size(); i++ )
    {
        if ( categoryList->at(i)->name == name )
        {
            qDebug() << "FOUND";
            return categoryList->at(i);
        }
    }

    return new CategoryModel();
}

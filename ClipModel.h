#ifndef CLIPMODEL_H
#define CLIPMODEL_H

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

       subCategory          = "";
       color                = "";
       year                 = "";
       size                 = 0;
       broken               = false;
       missingParts         = false;
       batteries            = false;
    }

    ClipModel( ClipModel* clip )
    {
        starting16th         = clip->starting16th;
        length16th           = clip->length16th;
        ending16th           = clip->starting16th + clip->length16th;

        enableCategory       = clip->enableCategory;
        enableSubCategory    = clip->enableSubCategory;
        enableColor          = clip->enableColor;
        enableAge            = clip->enableAge;
        enableSize           = clip->enableSize;
        enableBroken         = clip->enableBroken;
        enableMissingParts   = clip->enableMissingParts;
        enableBatteries      = clip->enableBatteries;

        subCategory          = clip->subCategory;
        color                = clip->color;
        year                 = clip->year;
        size                 = clip->size;
        broken               = clip->broken;
        missingParts         = clip->missingParts;
        batteries            = clip->batteries;
    }

    void setStarting16th( int value )
    {
        starting16th    = value;
        ending16th      = starting16th + length16th;
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

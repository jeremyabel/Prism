#ifndef CLIPDIALOG_H
#define CLIPDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QAbstractButton>

#include "CategoryData.h"
#include "ClipModel.h"
#include "ui_ClipParamDialog.h"

namespace Ui
{
    class ClipParamDialog;
}

class ClipParamDialog : public QDialog, public Ui::ClipParamDialog
{
    Q_OBJECT

public:
    ClipParamDialog( ClipModel& clip, const CategoryData* categoryData, QWidget *parent = 0 );
    ~ClipParamDialog();

private:
    Ui::ClipParamDialog* ui;

    bool                okOnLeft;
    bool                m_bInitializing;
    ClipModel*          editingClip;
    const CategoryData* m_pCategoryData;

    QButtonGroup*       m_pBrokenGroup;
    QButtonGroup*       m_pMissingGroup;
    QButtonGroup*       m_pBatteriesGroup;

signals:

public slots:

private slots:
    void on_catCheckBox_stateChanged(           int value );
    void on_subcatCheckBox_stateChanged(        int value );
    void on_colorCheckBox_stateChanged(         int value );
    void on_sizeCheckBox_stateChanged(          int value );
    void on_ageCheckBox_stateChanged(           int value );
    void on_brokenCheckBox_stateChanged(        int value );
    void on_missingCheckBox_stateChanged(       int value );
    void on_batCheckBox_stateChanged(           int value );
    void on_buttonRight_clicked();
    void on_buttonLeft_clicked();

    void on_distroComboBox_currentIndexChanged( int  index );
    void on_catComboBox_currentIndexChanged(    int  index );
    void on_subcatComboBox_currentIndexChanged( int  index );
    void on_colorComboBox_currentIndexChanged(  int  index );
    void on_sizeHorizontalSlider_valueChanged(  int  value );
    void on_ageComboBox_currentIndexChanged(    int  index );
    void onBrokenGroup_clicked(                 QAbstractButton* button );
    void onMissingGroup_clicked(                QAbstractButton* button );
    void onBatteriesGroup_clicked(              QAbstractButton* button );
};

#endif // CLIPDIALOG_H

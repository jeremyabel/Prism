#ifndef CLIPDIALOG_H
#define CLIPDIALOG_H

#include <QDialog>

#include "ClipModel.h"
#include "ui_ClipParamDialog.h"

class ClipParamDialog : public QDialog, public Ui::ClipParamDialog
{
    Q_OBJECT

public:
    ClipParamDialog( ClipModel& clip, QWidget *parent = 0 );

private:
    void restoreClip();

    Ui::ClipParamDialog* ui;

    ClipModel*          editingClip;
    ClipModel           originalClip;
    bool                okOnLeft;

signals:

public slots:

private slots:
    void on_catCheckBox_stateChanged(       int value );
    void on_subcatCheckBox_stateChanged(    int value );
    void on_colorCheckBox_stateChanged(     int value );
    void on_sizeCheckBox_stateChanged(      int value );
    void on_ageCheckBox_stateChanged(       int value );
    void on_brokenCheckBox_stateChanged(    int value );
    void on_missingCheckBox_stateChanged(   int value );
    void on_batCheckBox_stateChanged(       int value );
    void on_buttonRight_clicked();
    void on_buttonLeft_clicked();
};

#endif // CLIPDIALOG_H

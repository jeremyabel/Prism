#include <QDebug>
#include "ClipParamDialog.h"

ClipParamDialog::ClipParamDialog( ClipModel* clip, QWidget *parent ) : QDialog(parent), ui(new Ui::ClipParamDialog), originalClip(*new ClipModel(clip))
{
    ui->setupUi(this);

    editingClip  = clip;
    okOnLeft     = false;

#ifdef Q_WS_WIN
    // Swap buttons for Windows, as is customary
    okOnLeft = true;
    ui->buttonLeft->setText(  tr("OK") );
    ui->buttonRight->setText( tr("Cancel") );
#endif

    if ( editingClip->enableCategory )
    {

    }
    else
         ui->catComboBox->setEnabled( false );


    if ( editingClip->enableSubCategory )
    {

    }
    else
        ui->subcatComboBox->setEnabled( false );


    if ( editingClip->enableColor )
    {

    }
    else
        ui->colorComboBox->setEnabled( false );


    if ( editingClip->enableSize )
    {

    }
    else
        ui->sizeHorizontalSlider->setEnabled( false );


    if ( editingClip->enableAge )
    {

    }
    else
        ui->ageComboBox->setEnabled( false );


    if ( editingClip->enableBroken )
    {
        ui->brokenRadioButtonYes->setChecked( editingClip->broken );
        ui->brokenRadioButtonNo->setChecked( !editingClip->broken );
    }
    else
    {
        ui->brokenRadioButtonYes->setEnabled( false );
        ui->brokenRadioButtonNo->setEnabled(  false );
    }


    if ( editingClip->enableMissingParts )
    {
        ui->missingRadioButtonYes->setChecked( editingClip->missingParts );
        ui->missingRadioButtonNo->setChecked( !editingClip->missingParts );
    }
    else
    {
        ui->missingRadioButtonYes->setEnabled( false );
        ui->missingRadioButtonNo->setEnabled(  false );
    }


    if ( editingClip->enableBatteries )
    {
        ui->batRadioButtonYes->setChecked( editingClip->batteries );
        ui->batRadioButtonNo->setChecked( !editingClip->batteries );
    }
    else
    {
        ui->batRadioButtonYes->setEnabled( false );
        ui->batRadioButtonNo->setEnabled(  false );
    }
}

void ClipParamDialog::on_catCheckBox_stateChanged( int value )
{
    qDebug() << "Category toggle state:" << value;

    editingClip->enableCategory = value > 0;
    editingClip->category = ui->catComboBox->currentText();

    ui->catComboBox->setEnabled( editingClip->enableCategory );
}

void ClipParamDialog::on_subcatCheckBox_stateChanged( int value )
{
    qDebug() << "Subcategory toggle state:" << value;

    editingClip->enableSubCategory = value > 0;
    editingClip->subCategory = ui->subcatComboBox->currentText();

    ui->subcatComboBox->setEnabled( editingClip->enableSubCategory );
}

void ClipParamDialog::on_colorCheckBox_stateChanged( int value )
{
    qDebug() << "Color toggle state:" << value;

    editingClip->enableColor = value > 0;
    editingClip->color = ui->colorComboBox->currentText();

    ui->colorComboBox->setEnabled( editingClip->enableColor );
}

void ClipParamDialog::on_sizeCheckBox_stateChanged( int value )
{
    qDebug() << "Size toggle state:" << value;

    editingClip->enableSize = value > 0;
    editingClip->size = value;

    ui->sizeHorizontalSlider->setEnabled( editingClip->enableSize );
}

void ClipParamDialog::on_ageCheckBox_stateChanged( int value )
{
    qDebug() << "Age toggle state:" << value;

    editingClip->enableAge = value > 0;
    editingClip->year = ui->ageComboBox->currentText();

    ui->ageComboBox->setEnabled( editingClip->enableAge );
}

void ClipParamDialog::on_brokenCheckBox_stateChanged( int value )
{
    qDebug() << "Broken toggle state:" << value;

    editingClip->enableBroken = value > 0;
    editingClip->broken = ui->brokenRadioButtonYes->isChecked();

    ui->brokenRadioButtonYes->setEnabled( editingClip->enableBroken );
    ui->brokenRadioButtonNo->setEnabled(  editingClip->enableBroken );
}

void ClipParamDialog::on_missingCheckBox_stateChanged( int value )
{
    qDebug() << "Missing toggle state:" << value;

    editingClip->enableMissingParts = value > 0;
    editingClip->missingParts = ui->missingRadioButtonYes->isChecked();

    ui->missingRadioButtonYes->setEnabled( editingClip->enableMissingParts );
    ui->missingRadioButtonNo->setEnabled(  editingClip->enableMissingParts );
}

void ClipParamDialog::on_batCheckBox_stateChanged( int value )
{
    qDebug() << "Battery toggle state:" << value;

    editingClip->enableBatteries = value > 0;
    editingClip->batteries = ui->batRadioButtonYes->isChecked();

    ui->batRadioButtonYes->setEnabled( editingClip->enableBatteries );
    ui->batRadioButtonNo->setEnabled(  editingClip->enableBatteries );
}

void ClipParamDialog::on_buttonRight_clicked()
{
    if ( okOnLeft )
        restoreClip();

    this->close();
}

void ClipParamDialog::on_buttonLeft_clicked()
{
    if ( !okOnLeft )
        restoreClip();

    this->close();
}

void ClipParamDialog::restoreClip()
{
    qDebug() << "Restore clip";
}

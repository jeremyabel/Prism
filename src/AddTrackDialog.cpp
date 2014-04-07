#include "AddTrackDialog.h"
#include "ui_AddTrackDialog.h"

AddTrackDialog::AddTrackDialog( QString defaultName, int defaultColor, QWidget *parent ) : QDialog(parent), ui(new Ui::AddTrackDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText(defaultName);
    ui->colorComboBox->setCurrentIndex(defaultColor);

    color.setNamedColor( ui->colorComboBox->currentText().toLower().remove(' ') );
}

AddTrackDialog::~AddTrackDialog()
{
    delete ui;
}

void AddTrackDialog::on_lineEdit_textChanged( const QString &arg1 )
{
    newNameString = arg1;
}

void AddTrackDialog::on_colorComboBox_currentIndexChanged(const QString &arg1)
{
    color.setNamedColor( arg1.toLower().remove(' ') );
}

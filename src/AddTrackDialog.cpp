#include "AddTrackDialog.h"
#include "ui_AddTrackDialog.h"

AddTrackDialog::AddTrackDialog( QString defaultName, QWidget *parent ) : QDialog(parent), ui(new Ui::AddTrackDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText(defaultName);
}

AddTrackDialog::~AddTrackDialog()
{
    delete ui;
}

void AddTrackDialog::on_lineEdit_textChanged( const QString &arg1 )
{
    newNameString = arg1;
}

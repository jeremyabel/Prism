#include "include/RemoveTrackDialog.h"
#include "ui_RemoveTrackDialog.h"

RemoveTrackDialog::RemoveTrackDialog( QString trackName, QWidget *parent ) : QDialog(parent), ui(new Ui::RemoveTrackDialog)
{
    ui->setupUi(this);
    ui->label->setText( ui->label->text() + trackName + "?" );
}

RemoveTrackDialog::~RemoveTrackDialog()
{
    delete ui;
}

void RemoveTrackDialog::on_askAgainCheckbox_toggled( bool checked )
{

}

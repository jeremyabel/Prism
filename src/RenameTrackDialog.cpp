#include "RenameTrackDialog.h"
#include "ui_RenameTrackDialog.h"

RenameTrackDialog::RenameTrackDialog( TrackModel* track, QWidget *parent ) : QDialog(parent), ui(new Ui::RenameTrackDialog)
{
    m_pTrackModel = track;

    ui->setupUi(this);
    ui->lineEdit->setText( m_pTrackModel->sName );
}

RenameTrackDialog::~RenameTrackDialog()
{
    delete ui;
}

void RenameTrackDialog::on_lineEdit_textChanged(const QString &arg1)
{
    m_pTrackModel->sName = arg1;
}

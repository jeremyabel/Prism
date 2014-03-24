#ifndef ADDTRACKDIALOG_H
#define ADDTRACKDIALOG_H

#include <QDialog>

namespace Ui
{
    class AddTrackDialog;
}

class AddTrackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTrackDialog( QString defaultName, QWidget *parent = 0 );
    ~AddTrackDialog();

    QString newNameString;

private slots:
    void on_lineEdit_textChanged( const QString &arg1 );

private:
    Ui::AddTrackDialog *ui;
};

#endif // ADDTRACKDIALOG_H

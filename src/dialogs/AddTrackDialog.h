#ifndef ADDTRACKDIALOG_H
#define ADDTRACKDIALOG_H

#include <QDialog>
#include <QColor>

namespace Ui
{
    class AddTrackDialog;
}

class AddTrackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTrackDialog( QString defaultName, int defaultColor, QWidget *parent = 0 );
    ~AddTrackDialog();

    QString newNameString;
    QColor  color;

private slots:
    void on_lineEdit_textChanged( const QString &arg1 );
    void on_colorComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::AddTrackDialog *ui;
};

#endif // ADDTRACKDIALOG_H

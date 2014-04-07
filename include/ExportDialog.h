#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog( float bpm, float fps, QWidget *parent = 0 );
    ~ExportDialog();

    float bpm;
    float fps;

private slots:
    void on_bmpInput_textChanged(const QString &arg1);

    void on_fpsInput_textChanged(const QString &arg1);

private:
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H

#ifndef REMOVETRACKDIALOG_H
#define REMOVETRACKDIALOG_H

#include <QDialog>

namespace Ui
{
    class RemoveTrackDialog;
}

class RemoveTrackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveTrackDialog( QString trackName, QWidget *parent = 0 );
    ~RemoveTrackDialog();

private slots:
    void on_askAgainCheckbox_toggled( bool checked );

private:
    Ui::RemoveTrackDialog *ui;
};

#endif // REMOVETRACKDIALOG_H

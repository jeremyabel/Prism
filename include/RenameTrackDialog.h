#ifndef RENAMETRACKDIALOG_H
#define RENAMETRACKDIALOG_H

#include <QDialog>

#include "TrackModel.h"

namespace Ui
{
    class RenameTrackDialog;
}

class RenameTrackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameTrackDialog( TrackModel* track, QWidget *parent = 0 );
    ~RenameTrackDialog();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::RenameTrackDialog *ui;

    TrackModel* m_pTrackModel;
};

#endif // RENAMETRACKDIALOG_H

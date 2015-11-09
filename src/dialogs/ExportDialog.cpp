#include <QDoubleValidator>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include "ExportDialog.h"
#include "ui_ExportDialog.h"

ExportDialog::ExportDialog( float prevBPM, float prevFPS, QWidget *parent ) : QDialog(parent), ui(new Ui::ExportDialog)
{
    bpm = prevBPM;
    fps = prevFPS;

    // Floating point validator with 2 decimal places
    QDoubleValidator* validator = new QDoubleValidator(1.00, 999.99, 2);
    validator->setNotation(QDoubleValidator::StandardNotation);

    // Seems to be the simplest way to add two trailing digits, with zeros
    char bpmBuffer[7], fpsBuffer[7];
    sprintf(bpmBuffer, "%0.2f", bpm);
    sprintf(fpsBuffer, "%0.2f", fps);

    // Setup inputs
    ui->setupUi(this);
    ui->bmpInput->setText( QString::fromLocal8Bit(bpmBuffer) );
    ui->fpsInput->setText( QString::fromLocal8Bit(fpsBuffer) );
    ui->bmpInput->setValidator(validator);
    ui->fpsInput->setValidator(validator);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::on_bmpInput_textChanged(const QString &arg1)
{
    bpm = arg1.toFloat();
}

void ExportDialog::on_fpsInput_textChanged(const QString &arg1)
{
    fps = arg1.toFloat();
}

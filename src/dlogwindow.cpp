#include "dlogwindow.h"
#include "ui_dlogwindow.h"

DLogWindow::DLogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DLogWindow)
{
    ui->setupUi(this);
}

DLogWindow::~DLogWindow()
{
    delete ui;
}

void DLogWindow::addText(QString text, Qt::GlobalColor color)
{
    ui->tE_log->setTextColor(color);
    ui->tE_log->append(text);
    if (ui->tE_log->document()->toPlainText().length() >= 3000) {
        ui->tE_log->clear();
        ui->tE_log->setTextColor(Qt::black);
        ui->tE_log->append("Clear data");
    }

}

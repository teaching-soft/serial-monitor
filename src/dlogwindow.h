#ifndef DLOGWINDOW_H
#define DLOGWINDOW_H

#include <QDialog>

namespace Ui {
class DLogWindow;
}

class DLogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DLogWindow(QWidget *parent = 0);
    ~DLogWindow();
    void addText(QString text, Qt::GlobalColor color);

private:
    Ui::DLogWindow *ui;
};

#endif // DLOGWINDOW_H

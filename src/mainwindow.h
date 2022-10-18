#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>

class DLogWindow;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void send_aqm_request();
    void read_aqm_response();
    void watch_dog_aqm_request();
    void on_pB_log_clicked();

private:
    const quint32 WATCH_DOG_TIME = 2000;
    const quint32 INTERVAL_REQUEST = 1000;
    const QString cmdAll = "ALLS\r";
    const QString cmdOne = "ALL1\r";
    const QString cmdTwo = "ALL2\r";
    Ui::MainWindow *ui;
    DLogWindow *logWindow;
    QSerialPort serialPort;
    QTimer intervalTimerRequest;
    QTimer watchDog;
    QString cmd;
    void serialPortOpen();
    void parseALLS(QString response);
    void parseALL1(QString response);
    void parseALL2(QString response);

};

#endif // MAINWINDOW_H

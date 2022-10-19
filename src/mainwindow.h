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
    void send_firmware_request();
    void read_firmware_response();
    void watch_dog_firmware_request();
    void on_pB_log_clicked();
    void on_pB_start_clicked();
    void on_pB_stop_clicked();

    void on_pB_exit_clicked();

private:
    const quint32 WATCH_DOG_TIME = 2000;
    const quint32 INTERVAL_REQUEST = 1000;
    const QString FIELD_SEPARATOR = "|";
    const QString cmdRequest = "ALL\n";
    Ui::MainWindow *ui;
    DLogWindow *logWindow;
    QSerialPort serialPort;
    QTimer intervalTimerRequest;
    QTimer watchDog;
    QString cmd;
    void serialPortOpen();
    bool parseValues(QString response);

};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "dlogwindow.h"
#include "global_vars.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Collega il timer delle richieste e lo configure
    connect(&intervalTimerRequest, SIGNAL(timeout()), this, SLOT(send_firmware_request()));
    intervalTimerRequest.setInterval(INTERVAL_REQUEST);
    intervalTimerRequest.setSingleShot(true);

    // Collega il timer che segnala la mancata rispsota
    connect(&watchDog, SIGNAL(timeout()), this, SLOT(watch_dog_firmware_request()));

    // Collega l'evento inerente all'arrivo della risposta
    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(read_firmware_response()));

    // Apre la porta seriale
    serialPortOpen();

    // Crea la finestra di log
    logWindow = new DLogWindow(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::serialPortOpen()
{
    QString err = "";
    qint32 baud = aqmConfiguration->value("SerialPort/baud").toInt();
    qint8 databits = aqmConfiguration->value("SerialPort/databits").toInt();
    qint8 parity = aqmConfiguration->value("SerialPort/parity").toInt();
    qint8 stopbits = aqmConfiguration->value("SerialPort/stopbits").toInt();
    QString portname = aqmConfiguration->value("SerialPort/portname").toString();

    serialPort.setPortName(portname);

    if (!serialPort.open(QIODevice::ReadWrite))
        err = serialPort.portName() + " - " + serialPort.errorString();
    if (err == "")
        if (!serialPort.setBaudRate(baud))
            err = serialPort.portName() + " - " + serialPort.errorString();
    if (err == "")
        if (!serialPort.setDataBits((QSerialPort::DataBits)databits))
            err = serialPort.portName() + " - " + serialPort.errorString();
    if (err == "")
        if (!serialPort.setParity((QSerialPort::Parity)parity))
            err = serialPort.portName() + " - " + serialPort.errorString();
    if (err == "")
        if (!serialPort.setStopBits((QSerialPort::StopBits)stopbits))
            err = serialPort.portName() + " - " + serialPort.errorString();
    if (err == "")
        if (!serialPort.setFlowControl(QSerialPort::NoFlowControl))
            err = serialPort.portName() + " - " + serialPort.errorString();

    if (err != "") {
        QMessageBox::critical(NULL, "Attenzione: Errore!", err, QMessageBox::Ok);
        exit(EXIT_FAILURE);
    }
}

void MainWindow::send_firmware_request()
{
    serialPort.clear();
    serialPort.clearError();
    serialPort.write(cmdRequest.toLocal8Bit());
    QString cmd = cmdRequest;
    logWindow->addText(QString("Inviata richiesta:") + cmd.remove('\n'), Qt::darkGreen);
    qDebug() << cmdRequest;

    watchDog.start(WATCH_DOG_TIME);
}

void MainWindow::watch_dog_firmware_request()
{
    watchDog.stop();
    if (serialPort.isOpen() == true) serialPort.close();

    serialPortOpen();
    logWindow->addText("Perso una richiesta !!\r", Qt::black);
    qDebug() << "Perso una richiesta";
    intervalTimerRequest.stop();
    intervalTimerRequest.start();
}

void MainWindow::read_firmware_response()
{
    watchDog.stop();
    QByteArray tmp_data;
    if (serialPort.canReadLine() == false) return;

    QString response = serialPort.readAll();
    if (parseValues(response) == false)
        logWindow->addText(QString("Ricevuto risposta non corretta:") + response.remove('\n'), Qt::red);
    else logWindow->addText(QString("Ricevuto:") + response.remove('\n'), Qt::darkGreen);
    intervalTimerRequest.start();
    qDebug() << "OK";

}

bool MainWindow::parseValues(QString response)
{
    QStringList values = response.split(FIELD_SEPARATOR);

    if (values.count() != 6) return false;

    qreal tmp_value = values[0].toFloat();
    ui->l_temp->setText(QString::number(tmp_value, 'f', 2));
    ui->pB_temp->setValue((quint32)tmp_value);
    //  --
    tmp_value = values[1].toFloat();
    ui->l_rh->setText(QString::number(tmp_value, 'f', 1));
    ui->pB_rh->setValue((quint32)tmp_value);
    // --
    tmp_value = values[2].toFloat();
    ui->l_co2->setText(QString::number(tmp_value, 'f', 0));
    ui->pB_co2->setValue((quint32)tmp_value);
    // --
    tmp_value = values[3].toFloat();
    ui->l_o3->setText(QString::number(tmp_value, 'f', 0));
    ui->pB_o3->setValue((quint32)tmp_value);
    // --
    tmp_value = values[4].toFloat();
    ui->l_noise->setText(QString::number(tmp_value, 'f', 0));
    ui->pB_noise->setValue((quint32)tmp_value);
    // --
    tmp_value = values[5].toFloat();
    ui->l_accel->setText(QString::number(tmp_value, 'f', 2));
    ui->pB_accel->setValue((quint32)tmp_value);
    return true;
}

void MainWindow::on_pB_log_clicked()
{
    logWindow->show();
}

void MainWindow::on_pB_start_clicked()
{
    intervalTimerRequest.start();
    logWindow->addText("Avviato ... \r", Qt::darkGreen);
}


void MainWindow::on_pB_stop_clicked()
{
    intervalTimerRequest.stop();
}


void MainWindow::on_pB_exit_clicked()
{
    if (serialPort.isOpen() == true) serialPort.close();
    close();
}


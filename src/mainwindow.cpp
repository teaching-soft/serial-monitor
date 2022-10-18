#include "mainwindow.h"
#include "dlogwindow.h"
#include "global_vars.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&intervalTimerRequest, SIGNAL(timeout()), this,
            SLOT(send_aqm_request()));

    connect(&watchDog, SIGNAL(timeout()), this, SLOT(watch_dog_aqm_request()));

    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(read_aqm_response()));

    serialPortOpen();
    intervalTimerRequest.setInterval(INTERVAL_REQUEST);
    intervalTimerRequest.start();

    logWindow = new DLogWindow(this);
    logWindow->addText("Start\r", Qt::darkGreen);
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

void MainWindow::send_aqm_request()
{
    static quint8 what_cmd = 0;

    intervalTimerRequest.stop();

    if (what_cmd == 0) cmd = cmdAll;
    if (what_cmd == 1) cmd = cmdOne;
    if (what_cmd == 2) cmd = cmdTwo;

    what_cmd++;
    if (what_cmd == 3) what_cmd = 0;

    serialPort.clear();
    serialPort.clearError();
    serialPort.write(cmd.toLocal8Bit());
    logWindow->addText(QString("Inviata richiesta:") + cmd.remove('\r'), Qt::darkGreen);
    qDebug(cmd.toUtf8());

    watchDog.start(WATCH_DOG_TIME);
}

void MainWindow::watch_dog_aqm_request()
{
    watchDog.stop();
    if (serialPort.isOpen() == true)
        serialPort.close();

    serialPortOpen();
    logWindow->addText("Perso una richiesta !!\r", Qt::black);
    qDebug("Perso una richiesta");
    intervalTimerRequest.stop();
    intervalTimerRequest.start();
}

void MainWindow::read_aqm_response()
{
    watchDog.stop();
    QByteArray tmp_data = serialPort.peek(1000);
    if (tmp_data.count() == 0) {
        qDebug("Non ci sono caratteri nel buffer ?!");
        return;
    }
    if (tmp_data.contains('\r') == false) {
        logWindow->addText(QString("Ricevuto dati ma senza cr:") + tmp_data, Qt::yellow);
        QString msg("Caratteri arrivati senza cr:" + QString().number((tmp_data.length())));
        qDebug(msg.toUtf8());
        return;
    }
    QString response = serialPort.readAll();
    if (cmd == cmdAll)
        parseALLS(response);
    if (cmd == cmdOne)
        parseALL1(response);
    if (cmd == cmdTwo)
        parseALL2(response);
    logWindow->addText(QString("Ricevuto:") + response, Qt::red);
    qDebug("OK");
    intervalTimerRequest.start();
}

void MainWindow::parseALLS(QString response)
{
    quint8 sep = aqmConfiguration->value("Software/p01").toInt();
    QStringList values = response.split(sep);
    // \r is the last value
    if (values.count() != 13)
        return;
    // --
    qreal tmp_value = values[6].toFloat() / 100;
    ui->l_temp->setText(QString::number(tmp_value, 'f', 2));
    // qint32 ii = ui->pB_temp->value();
    ui->pB_temp->setValue((quint32)tmp_value);
    // ii = ui->pB_temp->value();
    //  --
    tmp_value = values[2].toFloat() / 10;
    ui->l_rh->setText(QString::number(tmp_value, 'f', 1));
    ui->pB_rh->setValue((quint32)tmp_value);
    // --
    tmp_value = values[0].toFloat();
    ui->l_co2->setText(QString::number(tmp_value, 'f', 0));
    ui->pB_co2->setValue((quint32)tmp_value);
    // --
    tmp_value = values[4].toFloat();
    ui->l_o3->setText(QString::number(tmp_value, 'f', 0));
    ui->pB_o3->setValue((quint32)tmp_value);

    // --
    tmp_value = values[3].toFloat() / 100;
    ui->l_noise->setText(QString::number(tmp_value, 'f', 0));
    ui->pB_noise->setValue((quint32)tmp_value);
    // --
    tmp_value = values[10].toFloat();
    if (tmp_value < 600)
        tmp_value = 0;
    else
        tmp_value = (tmp_value - 600) / 1000;
    ui->l_accel->setText(QString::number(tmp_value, 'f', 2));
    ui->pB_accel->setValue((quint32)tmp_value);
    // --------------------------------------------------------------------------------------------
    tmp_value = values[11].toFloat();
    ui->tE_rh->setHtml("<p align=center>" + QString::number(tmp_value, 'f', 0) +
                       "</p>");
    // --
    tmp_value = values[1].toFloat() / 10;
    ui->tE_temp_dht_22->setHtml("<p align=center>" +
                                QString::number(tmp_value, 'f', 1) + "</p>");
    // --
    tmp_value = values[5].toFloat() / 10;
    ui->tE_temp_ntc->setHtml("<p align=center>" +
                             QString::number(tmp_value, 'f', 1) + "</p>");
}

void MainWindow::parseALL1(QString response)
{
    quint8 sep = aqmConfiguration->value("Software/p01").toInt();
    QStringList values = response.split(sep);
    // \r is the last value
    if (values.count() != 11)
        return;

    qreal tmp_value = values[3].toFloat();
    ui->tE_NO2->setHtml("<p align=center>" + QString::number(tmp_value, 'f', 0) +
                        "</p>");
    // --
    tmp_value = values[1].toFloat();
    ui->tE_air_contam->setHtml("<p align=center>" +
                               QString::number(tmp_value, 'f', 0) + "</p>");
    // --
    tmp_value = values[4].toFloat();
    ui->tE_O3->setHtml("<p align=center>" + QString::number(tmp_value, 'f', 0) +
                       "</p>");
    // --
    tmp_value = values[2].toFloat();
    ui->tE_CO_HC_VOC->setHtml("<p align=center>" +
                              QString::number(tmp_value, 'f', 0) + "</p>");
}

void MainWindow::parseALL2(QString response)
{
    quint8 sep = aqmConfiguration->value("Software/p01").toInt();
    QStringList values = response.split(sep);

    // \r is the last value
    if (values.count() != 13)
        return;
    qreal tmp_value = values[0].toFloat();
    ui->tE_ch0->setHtml("<p align=center>" + QString::number(tmp_value, 'f', 0) +
                        "</p>");
    // --
    tmp_value = values[1].toFloat();
    ui->tE_ch1->setHtml("<p align=center>" + QString::number(tmp_value, 'f', 0) +
                        "</p>");
}

void MainWindow::on_pB_log_clicked()
{
    logWindow->show();
}

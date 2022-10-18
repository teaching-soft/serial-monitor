#include "global_vars.h"
#include "mainwindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>

const QString config_file_name = "/air_quality_monitor.conf";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Load configuration
    QString configFilePath =
        QCoreApplication::applicationDirPath() + config_file_name;
    QFileInfo config(configFilePath);
    if (config.isFile() != true) {
        QMessageBox::critical(NULL, "Attenzione",
                              " File di configurazione non trovato. Esco",
                              QMessageBox::Ok);
        return EXIT_FAILURE;
    }
    aqmConfiguration = new QSettings(configFilePath, QSettings::IniFormat);
    // ---------------------------

    MainWindow w;
    w.show();

    return a.exec();
}

#-------------------------------------------------
#
# Project created by QtCreator 2014-09-15T15:48:00
#
#-------------------------------------------------

QT		+= core gui
QT		+= serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AirQualityMonitor
TEMPLATE = app


SOURCES += main.cpp\
    dlogwindow.cpp \
        mainwindow.cpp \
    global_vars.cpp

HEADERS  += mainwindow.h \
    dlogwindow.h \
    global_vars.h

FORMS    += mainwindow.ui \
    dlogwindow.ui

OTHER_FILES += \
    resource.rc \
    todo.txt

RESOURCES += \
    resources.qrc

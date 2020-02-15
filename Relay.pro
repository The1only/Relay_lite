#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T12:46:45
#
#-------------------------------------------------

QT       += core gui network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Relay
TEMPLATE = app
DEFINES += MAXCHANNELS=5

SOURCES += main.cpp\
        mainwindow.cpp \
    tcpmodule.cpp \
    udpmodule.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    tcpmodule.h \
    udpmodule.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    images.qrc

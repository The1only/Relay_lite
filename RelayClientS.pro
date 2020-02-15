#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T12:46:45
#
#-------------------------------------------------

QT       += core gui network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RelayClientSingle
TEMPLATE = app
DEFINES += MAXCHANNELS=5

SOURCES += mainclient.cpp\
        mainwindowclients.cpp \
    tcpmodule.cpp \
    udpmodule.cpp

HEADERS  += mainwindowclients.h \
    tcpmodule.h \
    udpmodule.h

FORMS    += mainwindowclients.ui

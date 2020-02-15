#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T12:46:45
#
#-------------------------------------------------
HEADLESS=1
IoT=0

lessThan(HEADLESS, 1): QT += gui
lessThan(HEADLESS, 1): QT += widgets

QT       += core network
QT       += serialport
QT       += xml

QMAKE_CXXFLAGS += -std=gnu++0x -pthread
QMAKE_CFLAGS += -std=gnu++0x -pthread

QMAKE_INFO_PLIST = Info.plist

#QMAKE_INFO_PLIST += ./data/default.plist
#QMAKE_POST_LINK += sed -i -e "s/@VERSION@/$$VERSION/g" "./$${TARGET}.app/Contents/Info.plist";

greaterThan(HEADLESS, 0): TARGET = RelayClientHeadless
lessThan(HEADLESS, 1): TARGET = RelayClient

TEMPLATE = app
DEFINES += MAXCHANNELS=8
greaterThan(IoT, 0): DEFINES += IOT
greaterThan(HEADLESS, 0): DEFINES += HEADLESS

SOURCES += mainclient.cpp\
    tcpmodule.cpp \
    udpmodule.cpp \
    xml.cpp \
    mainwindowclient.cpp

lessThan(HEADLESS, 1): SOURCES +=


HEADERS  += tcpmodule.h \
    udpmodule.h \
    xml.h \
    mainwindowclient.h

lessThan(HEADLESS, 1): HEADERS +=
lessThan(HEADLESS, 1): FORMS += mainwindowclient.ui
lessThan(HEADLESS, 1): FORMS +=

RESOURCES += \
    images.qrc

DISTFILES +=
OTHER_FILES +=

#CONFIG += c++11

greaterThan(IoT, 0): unix|win32: LIBS += -L/usr/local/opt/openssl/lib/ -lssl
greaterThan(IoT, 0): unix|win32: LIBS += -L/usr/local/opt/openssl/lib/ -lcrypto

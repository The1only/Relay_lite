#ifndef MAINCOMMAND_H
#define MAINCOMMAND_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

#include <QtSerialPort/QSerialPort>
#include <QTextStream>
#include <QCoreApplication>
#include <QFile>
#include <QStringList>

#include "udpmodule.h"
#include "tcpmodule.h"
#include "xml.h"

class QTcpServer;
class QTcpSocket;
class QAction;

class MainCommand : public QObject
{
    Q_OBJECT

public:
    explicit MainCommand( QObject *parent = nullptr);
    ~MainCommand();

private slots:
    void on_speed();
    void readData(void );

private:
    int numport;
    int VideoTCPstart;
    int TTCPstart;

    XmlStreamLint *xml;
    QTimer    *bitrate;            // Bitrate timer...

    udpmodule *udpclientsVideo[MAXCHANNELS+1];
    udpmodule *udpclientsTelemetri[MAXCHANNELS+1];

    tcpmodule *tcpclientsVideo;
    tcpmodule *tcpclientsTelemetri;

    Settings settings() const;
    Settings currentSettings;
    void sendData(QByteArray data );
    void on_UdpPort_textChanged(const QString &arg1,const QString &arg2);
    void xmlSerialport();

    QSerialPort *serial;

    void Run();

};



#endif // MAINCOMMAND_H


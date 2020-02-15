#ifndef UPDMODUL_H
#define UPDMODUL_H

#ifndef HEADLESS
#include <QMainWindow>
#include <QDialog>
#include <QPlainTextEdit>
#include <QLCDNumber>
#endif
#include <QUdpSocket>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QJsonObject>


class udpmodule;
class tcpmodule;

class udpmodule : public QObject
{
public:
     udpmodule(QObject *parent = 0, int port = 14550, int n = MAXCHANNELS+1, QHostAddress *a = NULL, quint16 u = 0);
    ~udpmodule(){}

    int send(QByteArray datagram);
    udpmodule    *nextudpmodul[MAXCHANNELS+1];
    tcpmodule    *nexttcpserver[2];
#ifndef HEADLESS
    QPlainTextEdit *text;
#endif
    QSerialPort  *serial;

//    QLCDNumber    *countinn;
//    QLCDNumber    *countout;
    long    incount;
    long    outcount;
    QUdpSocket   *udpSocket;        // My socket...

private:
    Q_OBJECT

    quint16       Localudpport;     // My port...
    QHostAddress  address;          // Remote address...
    quint16       udpport;          // Remote port...
    QByteArray    datagram;         // Received data...
    QTimer       *timer;            // Contection timer...
    QTimer       *timerout;         // Contection timer...

    int           num;
    bool          stat;

private slots:
    void readPendingDatagrams();
    void on_timer();
    void on_timerout();
};

#endif // UPDMODUL_H

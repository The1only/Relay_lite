#ifndef TCPMODULE_H
#define TCPMODULE_H

#ifndef HEADLESS
#include <QMainWindow>
#include <QDialog>
#include <QPlainTextEdit>
#include <QLCDNumber>
#endif
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QJsonObject>

class udpmodule;

class tcpmodule : public QObject
{
public:
     tcpmodule(QObject *parent = 0, int port = 14550, int _comtype = 0);
     tcpmodule(QObject *parent = 0, QString a = NULL, int port = 14550, int _comtype = 0 );
    ~tcpmodule(){}

    int send(QByteArray datagram);
#ifndef HEADLESS
    QPlainTextEdit *text;
#endif

//    QLCDNumber    *countinn;
//    QLCDNumber    *countout;
    long    incount;
    long    outcount;

    udpmodule     *droneudpchannel;
    void updateServerProgress(int connection);
    QSerialPort *serial;

    udpmodule    *nextudpmodul[MAXCHANNELS+1];
    tcpmodule    *nexttcpserver;

private:
    Q_OBJECT

    void displayError(QAbstractSocket::SocketError socketError, int num);
    void tcptoudpsend(QByteArray buff);

    QTcpServer    tcpServer;
    QTcpSocket   *tcpServerConnection[MAXCHANNELS+1];
    quint16       Localudpport;     // My port...
    QString       LocalAddress;

    QByteArray    datagram;         // Received data...

    bool          stat;
    int           comtype; // Video or telemetriy...

private slots:
    void acceptConnection();
    void updateServerProgress1(void);
    void updateServerProgress2(void);
    void updateServerProgress3(void);
    void updateServerProgress4(void);
    void updateServerProgress5(void);
    void updateServerProgress6(void);
    void updateServerProgress7(void);
    void updateServerProgress8(void);
    void updateServerProgress9(void);
    void updateServerProgress10(void);
    void slotConnected(void);
    void displayError1(QAbstractSocket::SocketError socketError);
    void displayError2(QAbstractSocket::SocketError socketError);
    void displayError3(QAbstractSocket::SocketError socketError);
    void displayError4(QAbstractSocket::SocketError socketError);
    void displayError5(QAbstractSocket::SocketError socketError);
    void displayError6(QAbstractSocket::SocketError socketError);
    void displayError7(QAbstractSocket::SocketError socketError);
    void displayError8(QAbstractSocket::SocketError socketError);
    void displayError9(QAbstractSocket::SocketError socketError);
    void displayError10(QAbstractSocket::SocketError socketError);


};


#endif // TCPMODULE_H

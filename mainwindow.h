#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSerialPort/QSerialPort>

#include "udpmodule.h"
#include "tcpmodule.h"

class QDialogButtonBox;
class QLabel;
class QProgressBar;
class QPushButton;
class QTcpServer;
class QTcpSocket;
//class QUdpSocket;
class QAction;
#include "settingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, \
                        QString* portV = NULL, \
                        QString* portT = NULL, \
                        QString* add1 = NULL, \
                        QString* add2 = NULL, \
                        QString* add3 = NULL, \
                        QString* add4 = NULL, \
                        QString* add5 = NULL, \
                        QString* add6 = NULL, \
                        QString* add7 = NULL, \
                        QString* add8 = NULL, \
                        QString* add9 = NULL, \
                        QString* add10 = NULL);
    ~MainWindow();

    void openSerialport();
    SettingsDialog *s_settings;

    struct Settings{
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    Settings settings() const;
    Settings currentSettings;
    void sendData(QByteArray data );
    void on_UdpPort_textChanged(const QString &arg1,const QString &arg2);


private slots:
    void on_pushButton_clicked();
    void on_on_pushButton_2_clicked_clicked();
    void readData(void );
    void handleError(QSerialPort::SerialPortError error);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    int numport;

    udpmodule *udpclientsVideo[MAXCHANNELS];
    udpmodule *udpclientsTelemetri[MAXCHANNELS];

    tcpmodule *tcpServerVideo;
    tcpmodule *tcpServerTelemetri;

    tcpmodule *tcpclientsVideo;
    tcpmodule *tcpclientsTelemetri;

    QSerialPort *serial;
};

#endif // MAINWINDOW_H


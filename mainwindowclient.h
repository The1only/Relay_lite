#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef HEADLESS
#include <QMainWindow>
#include <QDialog>
#include "settingsdialog.h"
#endif

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

#ifndef HEADLESS
class QDialogButtonBox;
class QLabel;
class QProgressBar;
class QPushButton;

namespace Ui {
class MainWindow;
}
#endif

class QTcpServer;
class QTcpSocket;
class QAction;

#ifndef HEADLESS
class MainWindow : public QMainWindow
#else
class MainWindow : public QObject
#endif
{
    Q_OBJECT

public:
#ifndef HEADLESS
    MainWindow(QWidget *parent = NULL, QString xmlfile = "", QString run = "");
#else
    MainWindow( QObject *parent = NULL, QString xmlfile = "", QString run = "");
#endif
    ~MainWindow();

private slots:
#ifndef HEADLESS
    void on_on_pushButton_2_clicked_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_radioButtonUDP_clicked(bool checked);
    void on_radioButtonTCP_clicked(bool checked);
    void on_UdpPort_textChanged(const QString &arg1);
    void settingchanged();
    void on_pushButton_5_clicked();
    void on_IoT_EventHub_clicked();
#endif

    void on_speed();
    void readData(void );

    void on_pushButton_clicked();

private:
#ifndef HEADLESS
    Ui::MainWindow *ui;
#endif
    int numport;
    int VideoTCPstart;
    int TTCPstart;


    XmlStreamLint *xml;
    QTimer    *bitrate;            // Bitrate timer...

    udpmodule *udpclientsVideo[MAXCHANNELS+1];
    udpmodule *udpclientsTelemetri[MAXCHANNELS+1];

    tcpmodule *tcpclientsVideo;
    tcpmodule *tcpclientsTelemetri;
    tcpmodule *tcpclientsVideoServer;
    tcpmodule *tcpclientsTelemetriServer;

    void xmlSerialport();

#ifndef HEADLESS
    SettingsDialog *s_settings;
#endif
    Settings settings() const;
    Settings currentSettings;
    void sendData(QByteArray data );
    void on_UdpPort_textChanged(const QString &arg1,const QString &arg2);
    QSerialPort *serial;

#ifdef HEADLESS
    void Run();
#endif

};



#endif // MAINWINDOW_H


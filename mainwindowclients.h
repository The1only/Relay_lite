#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

#include "udpmodule.h"
#include "tcpmodule.h"
#include "settingsdialog.h"

class QDialogButtonBox;
class QLabel;
class QProgressBar;
class QPushButton;
class QTcpServer;
class QTcpSocket;
class QAction;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, \
                        QString* portV = NULL, \
                        QString* portT = NULL);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_on_pushButton_2_clicked_clicked();
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_on_pushButton_2_clicked_2_clicked();

private:
    Ui::MainWindow *ui;
    int numport;

    udpmodule *udpclientsVideo[MAXCHANNELS];
    udpmodule *udpclientsTelemetri[MAXCHANNELS];

    tcpmodule *tcpclientsVideo;
    tcpmodule *tcpclientsTelemetri;
};


#endif // MAINWINDOW_H


#include <QtWidgets>
#include <QtNetwork>

#include "mainwindowclients.h"
#include "udpmodule.h"
#include "tcpmodule.h"
#include "ui_mainwindowclients.h"

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(QWidget *parent, \
                       QString* portV, \
                       QString* portT) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(portV->size() > 4) ui->SUdpPort_2->setText(*portV);
    if(portT->size() > 4) ui->SUdpPort_7->setText(*portT);

    ui->plainTextOut->insertPlainText(tr("Ready ...\n"));
    qDebug() << "Started...\n";

    for(int i=0; i < MAXCHANNELS; i++) udpclientsVideo[i]=NULL;
    for(int i=0; i < MAXCHANNELS; i++) udpclientsTelemetri[i]=NULL;

    tcpclientsVideo  = NULL;
    tcpclientsTelemetri  = NULL;
}

void MainWindow::on_pushButton_clicked()
{
    ui->plainTextOut->insertPlainText(tr("Close ...\n"));
    this->close();
}


void MainWindow::on_on_pushButton_2_clicked_clicked()
{
    QString addr = "";
    int prt=0;

    ui->on_pushButton_2_clicked->setDisabled(true);
    ui->on_pushButton_2_clicked_2->setDisabled(true);
    ui->UdpPort->setDisabled(true);
    ui->TelemetruUDPPorts->setDisabled(true);

    int portV=ui->UdpPort->text().toInt();
    int portT=ui->TelemetruUDPPorts->text().toInt();

    numport = ui->NumUdpPorts->text().toInt();
    if(numport > MAXCHANNELS) numport = MAXCHANNELS;

    // Initialize channels...
    for(int i = 1; i < 5; i++){
        switch(i){
        case 1: addr = ui->SUdpAddress2->text(); prt = ui->SUdpPort_2->text().toInt(); break;
        case 2: addr = ui->SUdpAddress3->text(); prt = ui->SUdpPort_3->text().toInt(); break;
        case 3: addr = ui->SUdpAddress4->text(); prt = ui->SUdpPort_4->text().toInt(); break;
        case 4: addr = ui->SUdpAddress5->text(); prt = ui->SUdpPort_5->text().toInt(); break;
        }

        if(addr == ""){
            udpclientsVideo[i]= new udpmodule(this, portV+i, numport);
            ui->plainTextOut->insertPlainText(QString("New UDP Video Port: port=%1  n=%2\n").arg(portV+i).arg(numport));
        }
        else{
            QHostAddress ax;
            ax.setAddress(addr);
            udpclientsVideo[i]= new udpmodule(this, portV+i, numport,&ax,prt);
            ui->plainTextOut->insertPlainText(QString("New UDP Video Port: address=%1 port=%2  n=%3  u=%4\n").arg(addr).arg(portV+i).arg(numport).arg(prt));

        }
        udpclientsVideo[i]->text = ui->plainTextOut;

        if(i == 1){
            udpclientsVideo[i]->countinn = ui->lcdNumber_Video_INN_UDP;
            udpclientsVideo[i]->countout = ui->lcdNumber_Video_OUT_UDP;
        }
    }

    ui->plainTextOut->insertPlainText(tr("Setup mux!\n"));

    if(ui->SUdpAddress_1->text().length() < 4){
        // Setup as server...
        ui->plainTextOut->insertPlainText(QString("New TCP server Video: port=%1\n").arg(ui->UdpPort->text().toInt()));
        tcpclientsVideo           = new tcpmodule(this, ui->UdpPort->text().toInt());

    }
    else{
        // Setup as client...
        ui->plainTextOut->insertPlainText(QString("New TCP client Video: address=%1  port=%2\n").arg(ui->SUdpAddress_1->text()).arg(ui->UdpPort->text().toInt()));
        tcpclientsVideo           = new tcpmodule(this, ui->SUdpAddress_1->text(), ui->UdpPort->text().toInt());
    }

    tcpclientsVideo->text     = ui->plainTextOut;
    tcpclientsVideo->nexttcpserver = NULL;
    tcpclientsVideo->countinn = ui->lcdNumber_Video_INN_TCP;
    tcpclientsVideo->countout = ui->lcdNumber_Video_OUT_TCP;

    // Set up transmitters for main channel (0)...
    for(int i=1; i < numport; i++){
        udpclientsVideo[i]->nextudpmodul[0]=NULL;
        udpclientsVideo[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
        udpclientsVideo[i]->nexttcpserver[1]=tcpclientsVideo;
        tcpclientsVideo->nextudpmodul[i] = udpclientsVideo[i];
    }

    ui->plainTextOut->insertPlainText(tr("Running ...\n"));
    qDebug() << "System set up...";
}

void MainWindow::on_pushButton_3_clicked()
{
    if(udpclientsVideo[0])  udpclientsVideo[0]->incount  = 0;
    if(udpclientsVideo[0])  udpclientsVideo[0]->outcount = 0;
    if(tcpclientsVideo)     tcpclientsVideo->incount     = 0;
    if(tcpclientsVideo)     tcpclientsVideo->outcount    = 0;
}

void MainWindow::on_pushButton_4_clicked()
{
    if(udpclientsTelemetri[0])  udpclientsTelemetri[0]->incount  = 0;
    if(udpclientsTelemetri[0])  udpclientsTelemetri[0]->outcount = 0;
    if(tcpclientsTelemetri)     tcpclientsTelemetri->incount     = 0;
    if(tcpclientsTelemetri)     tcpclientsTelemetri->outcount    = 0;
}

void MainWindow::on_on_pushButton_2_clicked_2_clicked()
{
    QString addr = "";
    int prt=0;

    ui->on_pushButton_2_clicked->setDisabled(true);
    ui->on_pushButton_2_clicked_2->setDisabled(true);
    ui->UdpPort->setDisabled(true);
    ui->TelemetruUDPPorts->setDisabled(true);

    int portV=ui->UdpPort->text().toInt();
    int portT=ui->TelemetruUDPPorts->text().toInt();

    numport = ui->NumUdpPorts->text().toInt();
    if(numport > MAXCHANNELS) numport = MAXCHANNELS;

    // Initialize channels...
    for(int i =1; i < 5; i++){
        switch(i){
        case 1: addr = ui->SUdpAddress7->text(); prt = ui->SUdpPort_7->text().toInt(); break;
        case 2: addr = ui->SUdpAddress8->text(); prt = ui->SUdpPort_8->text().toInt(); break;
        case 3: addr = ui->SUdpAddress9->text(); prt = ui->SUdpPort_9->text().toInt(); break;
        case 4: addr = ui->SUdpAddress10->text(); prt = ui->SUdpPort_10->text().toInt(); break;
        }

        if(addr == ""){
            udpclientsTelemetri[i]= new udpmodule(this, portT+i, numport);
            ui->plainTextOut->insertPlainText(QString("New UDP Telemetry: port=%1  n=%2\n").arg(portT+i).arg(numport));
        }
        else{
            QHostAddress ax;
            ax.setAddress(addr);
            udpclientsTelemetri[i]= new udpmodule(this, portT+i, numport,&ax,prt);
            ui->plainTextOut->insertPlainText(QString("New UDP Telemetry: address=%1 port=%2  n=%3  u=%4\n").arg(addr).arg(portT+i).arg(numport).arg(prt));
        }
        udpclientsTelemetri[i]->text = ui->plainTextOut;

        if(i == 1){
            udpclientsTelemetri[i]->countinn = ui->lcdNumber_Tele_INN_UDP;
            udpclientsTelemetri[i]->countout = ui->lcdNumber_Tele_OUT_UDP;
        }
    }

    ui->plainTextOut->insertPlainText(tr("Setup mux!\n"));

    if(ui->SUdpAddress_2->text().length() < 4){
        // Setup as server...
        ui->plainTextOut->insertPlainText(QString("New TCP Server telemetry: port=%1\n").arg(ui->TelemetruUDPPorts->text().toInt()));
        tcpclientsTelemetri         = new tcpmodule(this, ui->TelemetruUDPPorts->text().toInt());
    }
    else{
        // Setup as client...
        ui->plainTextOut->insertPlainText(QString("New TCP client telemetry: address=%1  port=%2\n").arg(ui->SUdpAddress_2->text()).arg(ui->TelemetruUDPPorts->text().toInt()));
        tcpclientsTelemetri         = new tcpmodule(this, ui->SUdpAddress_2->text(), ui->TelemetruUDPPorts->text().toInt());
    }

    tcpclientsTelemetri->text   = ui->plainTextOut;
    tcpclientsTelemetri->nexttcpserver = NULL;
    tcpclientsTelemetri->countinn = ui->lcdNumber_Tele_INN_TCP;
    tcpclientsTelemetri->countout = ui->lcdNumber_Tele_OUT_TCP;

    for(int i=1; i < numport; i++){
        udpclientsTelemetri[i]->nextudpmodul[0]=NULL;
        udpclientsTelemetri[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
        udpclientsTelemetri[i]->nexttcpserver[1]=tcpclientsTelemetri;                  // Do NOT send TCP data to this port...
        tcpclientsTelemetri->nextudpmodul[i] = udpclientsTelemetri[i];
    }

    ui->plainTextOut->insertPlainText(tr("Running ...\n"));
    qDebug() << "System set up...";
}



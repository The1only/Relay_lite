#include <QtWidgets>
#include <QtNetwork>

#include "mainwindow.h"
#include "udpmodule.h"
#include "tcpmodule.h"
#include "ui_mainwindow.h"

MainWindow::~MainWindow()
{
    if(serial){
        if(serial->isOpen()){
            serial->close();
        }
    }
    delete ui;
}

MainWindow::MainWindow(QWidget *parent, \
                       QString* portV, \
                       QString* portT, \
                       QString* add1, \
                       QString* add2, \
                       QString* add3, \
                       QString* add4, \
                       QString* add5, \
                       QString* add6, \
                       QString* add7, \
                       QString* add8, \
                       QString* add9, \
                       QString* add10) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextOut->insertPlainText(tr("Ready ...\n"));
//    on_UdpPort_textChanged(*portV,*portT);
    on_UdpPort_textChanged(ui->UdpPort->text(),ui->TelemetruUDPPorts->text());

    qDebug() << "Started...";

    if(add1->size() > 4) ui->SUdpAddress1->setText(*add1);
    if(add2->size() > 4) ui->SUdpAddress2->setText(*add2);
    if(add3->size() > 4) ui->SUdpAddress3->setText(*add3);
    if(add4->size() > 4) ui->SUdpAddress4->setText(*add4);
    if(add5->size() > 4) ui->SUdpAddress5->setText(*add5);
    if(add6->size() > 4) ui->SUdpAddress6->setText(*add6);
    if(add7->size() > 4) ui->SUdpAddress7->setText(*add7);
    if(add8->size() > 4) ui->SUdpAddress8->setText(*add8);
    if(add9->size() > 4) ui->SUdpAddress9->setText(*add9);
    if(add10->size() > 4) ui->SUdpAddress10->setText(*add10);

    // Serial port ...
    serial    = new QSerialPort(this);
    s_settings  = new SettingsDialog();
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial,SIGNAL(readyRead()),this,SLOT(readData()));

    for(int i=0; i < MAXCHANNELS; i++) udpclientsVideo[i]=NULL;
    for(int i=0; i < MAXCHANNELS; i++) udpclientsTelemetri[i]=NULL;

    tcpclientsVideo  = NULL;
    tcpclientsTelemetri  = NULL;
    tcpServerVideo  = NULL;
    tcpServerTelemetri  = NULL;

    if(portV->length() >= 4) on_on_pushButton_2_clicked_clicked();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        //closeSerialPort();
    }
}

// Receive serial port data... They are only forwarded to the Telemetry channel...
void MainWindow::readData(void )
{
    char data[20000];
    QByteArray dataout;
    qint64 size = serial->read(data,200);

    dataout.setRawData(data,size);

    // Set up transmitters channel...
    for(int x=0; x < MAXCHANNELS; x++){
        for(int i=0; i < MAXCHANNELS; i++){
            if(udpclientsTelemetri[i]){
                if(udpclientsTelemetri[x]->nextudpmodul[i] != NULL){
                    udpclientsTelemetri[x]->nextudpmodul[i]->send(dataout);
                }
            }
        }
    }
    if(tcpServerTelemetri)
        tcpServerTelemetri->send(dataout);
}

void MainWindow::openSerialport()
{

    SettingsDialog::Settings p = s_settings->settings();
    if(p.name =="None!" || p.name == ""){
        return;
//        QMessageBox::critical(this, tr("Error"), "No Comport Selected!");
    }
    else{
        serial->setPortName(p.name);
        serial->setBaudRate(p.baudRate);
        serial->setDataBits(p.dataBits);
        serial->setParity(p.parity);
        serial->setStopBits(p.stopBits);
        serial->setFlowControl(p.flowControl);
        if (serial->open(QIODevice::ReadWrite)) {
        } else {
            QMessageBox::critical(this, tr("Error"), serial->errorString());
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->plainTextOut->insertPlainText(tr("Close ...\n"));
    this->close();
}


// Update defauld port to static clients...
void MainWindow::on_UdpPort_textChanged(const QString &argV, const QString &argT)
{
    int prtV = argV.toInt();
    int prtT = argT.toInt();

    ui->UdpPort->setText(tr("%1").arg(prtV));
    ui->TelemetruUDPPorts->setText(tr("%1").arg(prtT));

    ui->SUdpPort_1->setText(tr("%1").arg(prtV+0));
    ui->SUdpPort_2->setText(tr("%1").arg(prtV+1));
    ui->SUdpPort_3->setText(tr("%1").arg(prtV+2));
    ui->SUdpPort_4->setText(tr("%1").arg(prtV+3));
    ui->SUdpPort_5->setText(tr("%1").arg(prtV+4));

    ui->SUdpPort_6->setText(tr("%1").arg(prtT+0));
    ui->SUdpPort_7->setText(tr("%1").arg(prtT+1));
    ui->SUdpPort_8->setText(tr("%1").arg(prtT+2));
    ui->SUdpPort_9->setText(tr("%1").arg(prtT+3));
    ui->SUdpPort_10->setText(tr("%1").arg(prtT+4));

//    ui->TcpPort->setText(tr("%1").arg(prt+10));
//    ui->TcpPort_2->setText(tr("%1").arg(prt+10));

}

void MainWindow::on_on_pushButton_2_clicked_clicked()
{
    QString addr = "";
    int prt=0;

    openSerialport();

    ui->on_pushButton_2_clicked->setDisabled(true);
    ui->UdpPort->setDisabled(true);
    ui->TcpPort->setDisabled(true);
    ui->TcpPort_2->setDisabled(true);
    ui->TelemetruUDPPorts->setDisabled(true);

    int portV=ui->UdpPort->text().toInt();
    int portT=ui->TelemetruUDPPorts->text().toInt();

    numport = ui->NumUdpPorts->text().toInt();
    if(numport > MAXCHANNELS) numport = MAXCHANNELS;

    int start = (ui->UseTCPVideo->isChecked())?1:0;
    
    // Initialize channels...
    for(int i = start; i < 5; i++){
        switch(i){
        case 0: addr = ui->SUdpAddress1->text(); prt = ui->SUdpPort_1->text().toInt(); break;
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
        udpclientsVideo[i]->countinn = ui->lcdNumber_3;
        udpclientsVideo[i]->countout = ui->lcdNumber_4;

    }

    start = (ui->UseTCPTelemetri->isChecked())?1:0;
    
    // Initialize channels...
    for(int i = start; i < 5; i++){
        switch(i){
        case 0: addr = ui->SUdpAddress6->text(); prt = ui->SUdpPort_6->text().toInt(); break;
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
        udpclientsTelemetri[i]->countinn = ui->lcdNumber_3;
        udpclientsTelemetri[i]->countout = ui->lcdNumber_4;
    }
    
    ui->plainTextOut->insertPlainText(tr("TCP Servrer Listening\n"));

    tcpServerVideo           = new tcpmodule(this, ui->TcpPort->text().toInt());
    ui->plainTextOut->insertPlainText(QString("New TCP Video: port=%1\n").arg(ui->TcpPort->text().toInt()));
    tcpServerVideo->text     = ui->plainTextOut;
    tcpServerVideo->countinn = ui->lcdNumber;
    tcpServerVideo->countout = ui->lcdNumber_2;

    tcpServerTelemetri       = new tcpmodule(this, ui->TcpPort_2->text().toInt());
    ui->plainTextOut->insertPlainText(QString("New TCP telemetry: port=%1\n").arg(ui->TcpPort_2->text().toInt()));
    tcpServerTelemetri->text = ui->plainTextOut;
    tcpServerTelemetri->countinn = ui->lcdNumber;
    tcpServerTelemetri->countout = ui->lcdNumber_2;

    ui->plainTextOut->insertPlainText(tr("Setup mux!\n"));

    if(ui->UseTCPVideo->isChecked()){
        tcpServerVideo->droneudpchannel = NULL;

        if(ui->SUdpAddress1->text().length() < 4){
            // Setup as server...
            ui->plainTextOut->insertPlainText(QString("New TCP server Video: port=%1\n").arg(ui->UdpPort->text().toInt()));
            tcpclientsVideo           = new tcpmodule(this, ui->UdpPort->text().toInt());

        }
        else{
            // Setup as client...
            ui->plainTextOut->insertPlainText(QString("New TCP client Video: address=%1  port=%2\n").arg(ui->SUdpAddress1->text()).arg(ui->UdpPort->text().toInt()));
            tcpclientsVideo           = new tcpmodule(this, ui->SUdpAddress1->text(), ui->UdpPort->text().toInt());
        }

        tcpclientsVideo->text     = ui->plainTextOut;
        tcpclientsVideo->serial   = serial;
        tcpclientsVideo->nexttcpserver = tcpServerVideo;
        tcpServerVideo->nexttcpserver = tcpclientsVideo;
        tcpclientsVideo->countinn = ui->lcdNumber;
        tcpclientsVideo->countout = ui->lcdNumber_2;

        // Set up transmitters for main channel (0)...
        for(int i=1; i < numport; i++){
            udpclientsVideo[i]->nextudpmodul[0]=NULL;
            udpclientsVideo[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
            udpclientsVideo[i]->nexttcpserver[1]=tcpclientsVideo;
            udpclientsVideo[i]->serial = serial;
            tcpclientsVideo->nextudpmodul[i] = udpclientsVideo[i];
        }
    }
    else{
        tcpServerVideo->droneudpchannel = udpclientsVideo[0];
        udpclientsVideo[0]->nexttcpserver[0]=tcpServerVideo;                // Add the TCP Servere send function...
        udpclientsVideo[0]->serial = serial;

        // Set up transmitters for main channel (0)...
        for(int i=1; i < numport; i++){
            udpclientsVideo[0]->nextudpmodul[i-1]=udpclientsVideo[i];
            udpclientsVideo[i]->nextudpmodul[0]=udpclientsVideo[0];
            udpclientsVideo[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
            udpclientsVideo[i]->nexttcpserver[1]=NULL;                  // Do NOT send TCP data to this port...
            udpclientsVideo[i]->serial = serial;
        }
    }

    if(ui->UseTCPTelemetri->isChecked()){
        tcpServerTelemetri->droneudpchannel = NULL;

        if(ui->SUdpAddress6->text().length() < 4){
            // Setup as server...
            ui->plainTextOut->insertPlainText(QString("New TCP Server telemetry: port=%1\n").arg(ui->TelemetruUDPPorts->text().toInt()));
            tcpclientsTelemetri         = new tcpmodule(this, ui->TelemetruUDPPorts->text().toInt());
        }
        else{
            // Setup as client...
            ui->plainTextOut->insertPlainText(QString("New TCP client telemetry: address=%1  port=%2\n").arg(ui->SUdpAddress6->text()).arg(ui->TelemetruUDPPorts->text().toInt()));
            tcpclientsTelemetri         = new tcpmodule(this, ui->SUdpAddress6->text(), ui->TelemetruUDPPorts->text().toInt());
        }

        tcpclientsTelemetri->text   = ui->plainTextOut;
        tcpclientsTelemetri->serial = serial;
        tcpclientsTelemetri->nexttcpserver = tcpServerTelemetri;
        tcpServerTelemetri->nexttcpserver = tcpclientsTelemetri;
        tcpclientsTelemetri->countinn = ui->lcdNumber;
        tcpclientsTelemetri->countout = ui->lcdNumber_2;

        for(int i=1; i < numport; i++){
              udpclientsTelemetri[i]->nextudpmodul[0]=NULL;
              udpclientsTelemetri[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
              udpclientsTelemetri[i]->nexttcpserver[1]=tcpclientsTelemetri;                  // Do NOT send TCP data to this port...
              udpclientsTelemetri[i]->serial = serial;
              tcpclientsTelemetri->nextudpmodul[i] = udpclientsTelemetri[i];

        }

        tcpclientsTelemetri->serial = serial;                    // Add serial port output...
    }
    else{
        tcpServerTelemetri->droneudpchannel = udpclientsTelemetri[0];
        udpclientsTelemetri[0]->nexttcpserver[0]=tcpServerTelemetri;                // Add the TCP Servere send function...
        udpclientsTelemetri[0]->serial = serial;

        for(int i=1; i < numport; i++){
            udpclientsTelemetri[0]->nextudpmodul[i-1]=udpclientsTelemetri[i];
            udpclientsTelemetri[i]->nextudpmodul[0]=udpclientsTelemetri[0];
            udpclientsTelemetri[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
            udpclientsTelemetri[i]->nexttcpserver[1]=NULL;                  // Do NOT send TCP data to this port...
            udpclientsTelemetri[i]->serial = serial;
        }
    }

    ui->plainTextOut->insertPlainText(tr("Running ...\n"));
    qDebug() << "System set up...";
}


void MainWindow::on_pushButton_2_clicked()
{
    s_settings->show();
}

void MainWindow::on_pushButton_3_clicked()
{

    if(udpclientsTelemetri[0])
        udpclientsTelemetri[0]->incount = 0;
    if(udpclientsTelemetri[1])
        udpclientsTelemetri[1]->incount = 0;
    if(udpclientsTelemetri[2])
        udpclientsTelemetri[2]->incount = 0;
    if(udpclientsTelemetri[3])
        udpclientsTelemetri[3]->incount = 0;
    if(udpclientsTelemetri[4])
        udpclientsTelemetri[4]->incount = 0;

    if(udpclientsVideo[0])
        udpclientsVideo[0]->incount = 0;
    if(udpclientsVideo[1])
        udpclientsVideo[1]->incount = 0;
    if(udpclientsVideo[2])
        udpclientsVideo[2]->incount = 0;
    if(udpclientsVideo[3])
        udpclientsVideo[3]->incount = 0;
    if(udpclientsVideo[4])
        udpclientsVideo[4]->incount = 0;

    if(tcpclientsTelemetri)
        tcpclientsTelemetri->incount = 0;
    if(tcpclientsVideo)
        tcpclientsVideo->incount = 0;

    if(tcpServerTelemetri)
        tcpServerTelemetri->incount = 0;
    if(tcpServerVideo)
        tcpServerVideo->incount = 0;

    if(udpclientsTelemetri[0])
        udpclientsTelemetri[0]->outcount = 0;
    if(udpclientsTelemetri[1])
        udpclientsTelemetri[1]->outcount = 0;
    if(udpclientsTelemetri[2])
        udpclientsTelemetri[2]->outcount = 0;
    if(udpclientsTelemetri[3])
        udpclientsTelemetri[3]->outcount = 0;
    if(udpclientsTelemetri[4])
        udpclientsTelemetri[4]->outcount = 0;

    if(udpclientsVideo[0])
        udpclientsVideo[0]->outcount = 0;
    if(udpclientsVideo[1])
        udpclientsVideo[1]->outcount = 0;
    if(udpclientsVideo[2])
        udpclientsVideo[2]->outcount = 0;
    if(udpclientsVideo[3])
        udpclientsVideo[3]->outcount = 0;
    if(udpclientsVideo[4])
        udpclientsVideo[4]->outcount = 0;

    if(tcpclientsTelemetri)
        tcpclientsTelemetri->outcount = 0;
    if(tcpclientsVideo)
        tcpclientsVideo->outcount = 0;

    if(tcpServerTelemetri)
        tcpServerTelemetri->outcount = 0;
    if(tcpServerVideo)
        tcpServerVideo->outcount = 0;
}

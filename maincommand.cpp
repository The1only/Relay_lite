#include <QtNetwork>
#include <QObject>


#include "maincommand.h"
#include "udpmodule.h"
#include "tcpmodule.h"

#include "xml.h"

MainCommand::~MainCommand()
{
    if(serial){
        if(serial->isOpen()){
            serial->close();
        }
    }
}

MainCommand::MainCommand(QObject *parent)
    : QObject(parent)
{
    // Display bit rator over 1 min...
    bitrate = new QTimer(this);
    connect(bitrate, SIGNAL(timeout()), this, SLOT(on_speed()));

    qDebug() << "Started...\n";

    // Serial port ...
    serial    = new QSerialPort(this);
//    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial,SIGNAL(readyRead()),this,SLOT(readData()));

    for(int i=0; i < MAXCHANNELS+1; i++) udpclientsVideo[i]=NULL;
    for(int i=0; i < MAXCHANNELS+1; i++) udpclientsTelemetri[i]=NULL;

    tcpclientsVideo  = NULL;
    tcpclientsTelemetri  = NULL;

    // Get settings...
    xml = new XmlStreamLint(this);
    xmlSerialport();

    Run();
}


void MainCommand::xmlSerialport()
{
    Settings *p = &xml->currentSettings;
    if(p->name =="None" || p->name == ""){
        if(p->name == ""){
            fprintf(stderr,"Error Serialport");
        }
        return;
    }
    else{
        serial->setPortName(p->name);
        serial->setBaudRate(p->baudRate);
        serial->setDataBits(p->dataBits);
        serial->setParity(p->parity);
        serial->setStopBits(p->stopBits);
        serial->setFlowControl(p->flowControl);
        if (serial->open(QIODevice::ReadWrite)) {
        } else {
            fprintf(stderr,"Error 1");
        }
        connect(serial,SIGNAL(readyRead()),this,SLOT(readData()));
    }
}


// Timeout then remove client...
void MainCommand::on_speed()
{
    double inc = 8.0*(double)udpclientsVideo[VideoTCPstart]->incount/10.0;
    double ouc = 8.0*(double)udpclientsVideo[VideoTCPstart]->outcount/10.0;
    double inf = 8.0*(double)udpclientsTelemetri[TTCPstart]->incount/10.0;
    double ouf = 8.0*(double)udpclientsTelemetri[TTCPstart]->outcount/10.0;
    udpclientsVideo[VideoTCPstart]->incount=0L;
    udpclientsVideo[VideoTCPstart]->outcount=0L;
    udpclientsTelemetri[TTCPstart]->incount=0L;
    udpclientsTelemetri[TTCPstart]->outcount=0L;
    fprintf(stderr,"UDP Video In (Raw rate): %lf bit/s  Out: %lf bit/s Telemetri  In: %lf bit/s  Out: %lf bit/s\n",inc,ouc,inf,ouf);
}


// Receive serial port data... They are only forwarded to the Telemetry channel...
void MainCommand::readData(void )
{
    char data[20000];
    QByteArray dataout;
    qint64 size = serial->read(data,200);

    dataout.setRawData(data,size);

//    // Set up transmitters channel...
//    for(int x=0; x < MAXCHANNELS+1; x++){
//        for(int i=0; i < MAXCHANNELS+1; i++){
//            if(udpclientsTelemetri[i]){
//                if(udpclientsTelemetri[x]->nextudpmodul[i] != NULL){
//                    udpclientsTelemetri[x]->nextudpmodul[i]->send(dataout);
//                }
//            }
//        }
//    }
    if(tcpclientsTelemetri)
        tcpclientsTelemetri->send(dataout);
}

void MainCommand::Run()
{
    QString addr = "";
    int remoteport=0;

    VideoTCPstart = (xml->currentSettings.UseVideoTCP)?1:0;
    TTCPstart = (xml->currentSettings.UseVideoTCP)?1:0;

    // Initialize channels...
    for(int i = VideoTCPstart; i < MAXCHANNELS+1; i++){
        switch(i){
        case 0: addr = xml->currentSettings.VideoAddressIn; remoteport = xml->currentSettings.VPortIn; break;
        case 1: addr = xml->currentSettings.VideoUDPAddressOut1; remoteport = xml->currentSettings.VideoUDPPortOut1; break;
        case 2: addr = xml->currentSettings.VideoUDPAddressOut2; remoteport = xml->currentSettings.VideoUDPPortOut2; break;
        case 3: addr = xml->currentSettings.VideoUDPAddressOut3; remoteport = xml->currentSettings.VideoUDPPortOut3; break;
        case 4: addr = xml->currentSettings.VideoUDPAddressOut4; remoteport = xml->currentSettings.VideoUDPPortOut4; break;
        case 5: addr = xml->currentSettings.VideoUDPAddressOut5; remoteport = xml->currentSettings.VideoUDPPortOut5; break;
        case 6: addr = xml->currentSettings.VideoUDPAddressOut6; remoteport = xml->currentSettings.VideoUDPPortOut6; break;
        case 7: addr = xml->currentSettings.VideoUDPAddressOut7; remoteport = xml->currentSettings.VideoUDPPortOut7; break;
        case 8: addr = xml->currentSettings.VideoUDPAddressOut8; remoteport = xml->currentSettings.VideoUDPPortOut8; break;
        }

        if(addr == ""){
            udpclientsVideo[i]= new udpmodule(this, remoteport, MAXCHANNELS+1);
            qDebug() << QString("New UDP Video Port: port=%1\n").arg(remoteport);
        }
        else{
            QHostAddress ax;
            ax.setAddress(addr);
            udpclientsVideo[i]= new udpmodule(this, remoteport, MAXCHANNELS+1,&ax,remoteport);
            qDebug() << QString("New UDP Video Port: port=%1 address=%2:%3\n").arg(remoteport).arg(addr).arg(remoteport);

        }
    }
    
    // Initialize channels...
    for(int i =TTCPstart; i < MAXCHANNELS+1; i++){
        switch(i){      
        case 0: addr = xml->currentSettings.TAddressIn; remoteport = xml->currentSettings.TPortIn; break;
        case 1: addr = xml->currentSettings.TUDPAddressOut1; remoteport = xml->currentSettings.TUDPPortOut1; break;
        case 2: addr = xml->currentSettings.TUDPAddressOut2; remoteport = xml->currentSettings.TUDPPortOut2; break;
        case 3: addr = xml->currentSettings.TUDPAddressOut3; remoteport = xml->currentSettings.TUDPPortOut3; break;
        case 4: addr = xml->currentSettings.TUDPAddressOut4; remoteport = xml->currentSettings.TUDPPortOut4; break;
        case 5: addr = xml->currentSettings.TUDPAddressOut5; remoteport = xml->currentSettings.TUDPPortOut5; break;
        case 6: addr = xml->currentSettings.TUDPAddressOut6; remoteport = xml->currentSettings.TUDPPortOut6; break;
        case 7: addr = xml->currentSettings.TUDPAddressOut7; remoteport = xml->currentSettings.TUDPPortOut7; break;
        case 8: addr = xml->currentSettings.TUDPAddressOut8; remoteport = xml->currentSettings.TUDPPortOut8; break;
        }

        if(addr == ""){
            udpclientsTelemetri[i]= new udpmodule(this, remoteport, MAXCHANNELS+1);
            qDebug() << QString("New UDP Telemetry: port=%1\n").arg(remoteport);
        }
        else{
            QHostAddress ax;
            ax.setAddress(addr);
            udpclientsTelemetri[i]= new udpmodule(this, remoteport, MAXCHANNELS+1,&ax,remoteport);
            qDebug() << QString("New UDP Telemetry: port=%1 address=%2:%3\n").arg(remoteport).arg(addr).arg(remoteport);
        }
    }

    if(VideoTCPstart)
    {
        if(xml->currentSettings.VideoTCPAddressOut == ""){
            // Setup as server...
            qDebug() << QString("New TCP server Video: port=%1\n").arg(xml->currentSettings.VideoTCPPortOut);
            tcpclientsVideo           = new tcpmodule(this, xml->currentSettings.VideoTCPPortOut);
        }
        else{
            // Setup as client...
            qDebug() << QString("New TCP client Video: address=%1:%2\n").arg(xml->currentSettings.VideoTCPAddressOut).arg(xml->currentSettings.VideoTCPPortOut);
            tcpclientsVideo           = new tcpmodule(this, xml->currentSettings.VideoTCPAddressOut, xml->currentSettings.VideoTCPPortOut);
        }
        tcpclientsVideo->nexttcpserver = NULL;

        // Set up transmitters for main channel (0)...
        for(int i=1; i < MAXCHANNELS+1; i++){
            udpclientsVideo[i]->nextudpmodul[0]=NULL;
            udpclientsVideo[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
            udpclientsVideo[i]->nexttcpserver[1]=tcpclientsVideo;
            tcpclientsVideo->nextudpmodul[i] = udpclientsVideo[i];
        }
    }
    else
    {
        udpclientsVideo[0]->nexttcpserver[0]=NULL;

        // Set up transmitters for main channel (0)...
        for(int i=1; i < MAXCHANNELS+1; i++){
            udpclientsVideo[0]->nextudpmodul[i-1]=udpclientsVideo[i];
            udpclientsVideo[i]->nextudpmodul[0]=udpclientsVideo[0];
            udpclientsVideo[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
            udpclientsVideo[i]->nexttcpserver[1]=NULL;                  // Do NOT send TCP data to this port...
        }
    }

    if(xml->currentSettings.UseTTCPOut)
    {
        // Setup as server...
        qDebug() << QString("New TCP Server telemetry: port=%1\n").arg(xml->currentSettings.TTCPPortOut);
        tcpclientsTelemetri         = new tcpmodule(this, xml->currentSettings.TTCPPortOut);
    }
    else{
        // Setup as client...
        qDebug() << QString("New TCP client telemetry: address=%1:%2\n").arg(xml->currentSettings.TTCPAddressOut).arg(xml->currentSettings.TTCPPortOut);
        tcpclientsTelemetri         = new tcpmodule(this, xml->currentSettings.TTCPAddressOut, xml->currentSettings.TTCPPortOut);
    }

    tcpclientsTelemetri->nexttcpserver = NULL;

    for(int i=1; i < numport; i++){
        udpclientsTelemetri[i]->nextudpmodul[0]=NULL;
        udpclientsTelemetri[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
        udpclientsTelemetri[i]->nexttcpserver[1]=tcpclientsTelemetri;                  // Do NOT send TCP data to this port...
        tcpclientsTelemetri->nextudpmodul[i] = udpclientsTelemetri[i];
    }

    bitrate->start(10*1000); // 1 min timeout...

    qDebug() << "System set up...";
}

#ifndef HEADLESS
#include "ui_mainwindowclient.h"
#include <QtWidgets>
#endif
#include <QtNetwork>

#include "mainwindowclient.h"
#include "udpmodule.h"
#include "tcpmodule.h"


#include "xml.h"

#ifdef IOT
#include "event_hub.h"
#endif

char EH_HOST[] = "drone-cg-eh.servicebus.windows.net";
char EH_NAME[] = "iotexternaleh";
char EH_KEY[]  = "SharedAccessSignature sr=sb%3a%2f%2fdrone-cg-eh.servicebus.windows.net%2fiotexternaleh%2fpublishers%2fb5d81f61-a7bb-42ca-bd00-1a3f7445697b&sig=gXsKUb%2fvRo0lw3kLdrqvGFSYr%2bxmp64CFNtDhebHBCc%3d&se=1519752004&skn=RootManageSharedAccessKey";
char EH_PUBLISHER[] = "b5d81f61-a7bb-42ca-bd00-1a3f7445697b";

extern unsigned int sent_messages;
extern unsigned char auth;

MainWindow::~MainWindow()
{
    if(serial){
        if(serial->isOpen()){
            serial->close();
        }
    }
#ifndef HEADLESS
    delete ui;
#endif
}

#ifndef HEADLESS
MainWindow::MainWindow(QWidget *parent, QString xmlfile, QString run) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  #else
MainWindow::MainWindow(QObject *parent, QString xmlfile, QString run) :
    QObject(parent)
    #endif
{
#ifndef HEADLESS
    ui->setupUi(this);
#endif
    xml = new XmlStreamLint(this,xmlfile);
#ifndef HEADLESS
    if(xml->readdy){
        ui->VideoPortInn1->setText(tr("%1").arg(xml->currentSettings.VPortIn));
        ui->TelemetryPortInn1->setText(tr("%1").arg(xml->currentSettings.TPortIn));
        ui->SUdpAddressVInn_1->setText(xml->currentSettings.VideoAddressIn);
        ui->SUdpAddressTInn_1->setText(xml->currentSettings.TAddressIn);
        ui->radioButtonTCP->setChecked(xml->currentSettings.UseVideoTCP);
        ui->radioButtonTCP_2->setChecked(xml->currentSettings.UseTTCP);
    }
#endif

    // Display bit rator over 1 min...
    bitrate = new QTimer(this);
    connect(bitrate, SIGNAL(timeout()), this, SLOT(on_speed()));

#ifndef HEADLESS
    ui->plainTextOut->insertPlainText(tr("Starting ...\n"));

#endif
    qDebug() << "Initializing......\n";

    for(int i=0; i < MAXCHANNELS+1; i++) udpclientsVideo[i]=NULL;
    for(int i=0; i < MAXCHANNELS+1; i++) udpclientsTelemetri[i]=NULL;

    tcpclientsVideo  = NULL;
    tcpclientsTelemetri  = NULL;
    tcpclientsVideoServer  = NULL;
    tcpclientsTelemetriServer  = NULL;

#ifdef HEADLESS
    Run();
#endif

}

#ifndef HEADLESS
void MainWindow::settingchanged()
{
}
#endif

// Timeout then remove client...
void MainWindow::on_speed()
{
    double inc = 8.0*(double)udpclientsVideo[VideoTCPstart]->incount/10000.0;
    double ouc = 8.0*(double)udpclientsVideo[VideoTCPstart]->outcount/10000.0;
    double inf = 8.0*(double)udpclientsTelemetri[TTCPstart]->incount/10000.0;
    double ouf = 8.0*(double)udpclientsTelemetri[TTCPstart]->outcount/10000.0;
    udpclientsVideo[VideoTCPstart]->incount=0L;
    udpclientsVideo[VideoTCPstart]->outcount=0L;
    udpclientsTelemetri[TTCPstart]->incount=0L;
    udpclientsTelemetri[TTCPstart]->outcount=0L;
#ifndef HEADLESS
    ui->lcdNumber_Video_INN_UDP->display(inc);
    ui->lcdNumber_Video_OUT_UDP->display(ouc);
    ui->lcdNumber_Tele_INN_UDP->display(inf);
    ui->lcdNumber_Tele_OUT_UDP->display(ouf);
#endif
    fprintf(stderr,"UDP Video In (Raw rate): %lf bit/s  Out: %lf bit/s Telemetri  In: %lf bit/s  Out: %lf bit/s\n",inc,ouc,inf,ouf);
}


// Receive serial port data... They are only forwarded to the Telemetry channel...
void MainWindow::readData(void )
{
    char data[20000];
    QByteArray dataout;
    qint64 size = serial->read(data,200);

    dataout.setRawData(data,size);

    // Set up transmitters channel...
    for(int x=0; x < MAXCHANNELS+1; x++){
        for(int i=0; i < MAXCHANNELS+1; i++){
            if(udpclientsTelemetri[x]){
                if(udpclientsTelemetri[x]->nextudpmodul[i] != NULL){
                    udpclientsTelemetri[x]->nextudpmodul[i]->send(dataout);
                }
            }
        }
    }
    if(tcpclientsTelemetri)
        tcpclientsTelemetri->send(dataout);

    if(tcpclientsTelemetriServer)
        tcpclientsTelemetriServer->send(dataout);
}

#ifndef HEADLESS
void MainWindow::on_on_pushButton_2_clicked_clicked()
#else
void MainWindow::Run()
#endif
{
    QString addr = "";
    int remoteport=0;

#ifndef HEADLESS
    ui->on_pushButton_2_clicked->setDisabled(true);
    ui->groupBox->setDisabled(true);

    ui->SUdpAddressVInn_1->setDisabled(true);
    ui->SUdpAddressTInn_1->setDisabled(true);
    ui->VideoPortInn1->setDisabled(true);
    ui->TelemetryPortInn1->setDisabled(true);

    VideoTCPstart = (ui->radioButtonTCP->isChecked())?1:0;
    TTCPstart = (ui->radioButtonTCP_2->isChecked())?1:0;
#else
    VideoTCPstart = (xml->currentSettings.UseVideoTCP)?1:0;
    TTCPstart = (xml->currentSettings.UseVideoTCP)?1:0;
#endif
    // Initialize channels...
    for(int i = VideoTCPstart; i < MAXCHANNELS+1; i++){
        switch(i){
#ifndef HEADLESS
        case 0: addr = ui->SUdpAddressVInn_1->text(); remoteport = ui->VideoPortInn1->text().toInt(); break;
        case 1: addr = ui->SUdpAddress2->text(); remoteport = ui->SUdpPort_2->text().toInt(); break;
        case 2: addr = ui->SUdpAddress3->text(); remoteport = ui->SUdpPort_3->text().toInt(); break;
        case 3: addr = ui->SUdpAddress4->text(); remoteport = ui->SUdpPort_4->text().toInt(); break;
        case 4: addr = ui->SUdpAddress5->text(); remoteport = ui->SUdpPort_5->text().toInt(); break;
#else
        case 0: addr = xml->currentSettings.VideoAddressIn; remoteport = xml->currentSettings.VPortIn; break;
        case 1: addr = xml->currentSettings.VideoUDPAddressOut1; remoteport = xml->currentSettings.VideoUDPPortOut1; break;
        case 2: addr = xml->currentSettings.VideoUDPAddressOut2; remoteport = xml->currentSettings.VideoUDPPortOut2; break;
        case 3: addr = xml->currentSettings.VideoUDPAddressOut3; remoteport = xml->currentSettings.VideoUDPPortOut3; break;
        case 4: addr = xml->currentSettings.VideoUDPAddressOut4; remoteport = xml->currentSettings.VideoUDPPortOut4; break;
#endif
        }

        if(addr == ""){
   //         if(remoteport>0)
            {
                udpclientsVideo[i]= new udpmodule(this, remoteport, MAXCHANNELS+1);
#ifndef HEADLESS
                ui->plainTextOut->insertPlainText(QString("New UDP Video Port: port=%1\n").arg(remoteport));
#else
                qDebug() << QString("New UDP Video Port: port=%1\n").arg(remoteport);
#endif
            }
        }
        else{
            QHostAddress ax;
            ax.setAddress(addr);
            udpclientsVideo[i]= new udpmodule(this, remoteport, MAXCHANNELS+1,&ax,remoteport);
#ifndef HEADLESS
            ui->plainTextOut->insertPlainText(QString("New UDP Video Port: port=%1 address=%2:%3\n").arg(remoteport).arg(addr).arg(remoteport));
#else
            qDebug() << QString("New UDP Video Port: port=%1 address=%2:%3\n").arg(remoteport).arg(addr).arg(remoteport);
#endif
        }
#ifndef HEADLESS
        if(udpclientsVideo[i])
            udpclientsVideo[i]->text = ui->plainTextOut;
#endif
    }
    
    // Initialize channels...
    for(int i =TTCPstart; i < MAXCHANNELS+1; i++){
        switch(i){
#ifndef HEADLESS
        case 0: addr = ui->SUdpAddressTInn_1->text(); remoteport = ui->TelemetryPortInn1->text().toInt(); break;
        case 1: addr = ui->SUdpAddress12->text(); remoteport = ui->SUdpPort_12->text().toInt(); break;
        case 2: addr = ui->SUdpAddress13->text(); remoteport = ui->SUdpPort_13->text().toInt(); break;
        case 3: addr = ui->SUdpAddress14->text(); remoteport = ui->SUdpPort_14->text().toInt(); break;
        case 4: addr = ui->SUdpAddress15->text(); remoteport = ui->SUdpPort_15->text().toInt(); break;
#else
        case 0: addr = xml->currentSettings.TAddressIn; remoteport = xml->currentSettings.TPortIn; break;
        case 1: addr = xml->currentSettings.TUDPAddressOut1; remoteport = xml->currentSettings.TUDPPortOut1; break;
        case 2: addr = xml->currentSettings.TUDPAddressOut2; remoteport = xml->currentSettings.TUDPPortOut2; break;
        case 3: addr = xml->currentSettings.TUDPAddressOut3; remoteport = xml->currentSettings.TUDPPortOut3; break;
        case 4: addr = xml->currentSettings.TUDPAddressOut4; remoteport = xml->currentSettings.TUDPPortOut4; break;
#endif
        }

        if(addr == ""){
        //    if(remoteport > 0)
            {
                udpclientsTelemetri[i]= new udpmodule(this, remoteport, MAXCHANNELS+1);
#ifndef HEADLESS
                ui->plainTextOut->insertPlainText(QString("New UDP Telemetry: port=%1\n").arg(remoteport));
#else
                qDebug() << QString("New UDP Telemetry: port=%1\n").arg(remoteport);
#endif
            }
        }
        else{
            QHostAddress ax;
            ax.setAddress(addr);
            udpclientsTelemetri[i]= new udpmodule(this, remoteport, MAXCHANNELS+1,&ax,remoteport);
#ifndef HEADLESS
            ui->plainTextOut->insertPlainText(QString("New UDP Telemetry: port=%1 address=%2:%3\n").arg(remoteport).arg(addr).arg(remoteport));
#else
            qDebug() << QString("New UDP Telemetry: port=%1 address=%2:%3\n").arg(remoteport).arg(addr).arg(remoteport);
#endif
        }
#ifndef HEADLESS
        if(udpclientsVideo[i])
            udpclientsTelemetri[i]->text = ui->plainTextOut;
#endif
    }

#ifndef HEADLESS
    ui->plainTextOut->insertPlainText(tr("Setup mux!\n"));
    if(ui->radioButtonTCP->isChecked())
    {
        if(ui->SUdpAddressVInn_1->text().length() < 4){
            // Setup as server...
            ui->plainTextOut->insertPlainText(QString("New TCP server Video: port=%1\n").arg(ui->VideoPortInn1->text().toInt()));
            tcpclientsVideo           = new tcpmodule(this, ui->VideoPortInn1->text().toInt(),0);
#else
    if(VideoTCPstart)
    {
        if(xml->currentSettings.VideoTCPAddressOut == ""){
            qDebug() << QString("New TCP server Video: port=%1\n").arg(xml->currentSettings.VideoTCPPortOut);
            tcpclientsVideo           = new tcpmodule(this, xml->currentSettings.VideoTCPPortOut,0);
#endif
        }
        else{
#ifndef HEADLESS
            // Setup as client...
            ui->plainTextOut->insertPlainText(QString("New TCP client Video: address=%1:%2\n").arg(ui->VideoPortInn1->text()).arg(ui->VideoPortInn1->text().toInt()));
            tcpclientsVideo           = new tcpmodule(this, ui->VideoPortInn1->text(), ui->VideoPortInn1->text().toInt(),0);
#else
            // Setup as client...
            qDebug() << QString("New TCP client Video: address=%1:%2\n").arg(xml->currentSettings.VideoTCPAddressOut).arg(xml->currentSettings.VideoTCPPortOut);
            tcpclientsVideo           = new tcpmodule(this, xml->currentSettings.VideoTCPAddressOut, xml->currentSettings.VideoTCPPortOut,0);
#endif
        }
#ifndef HEADLESS
        tcpclientsVideo->text     = ui->plainTextOut;
#endif
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

#ifndef HEADLESS
    if(ui->radioButtonTCP_2->isChecked())
    {
        if(ui->SUdpAddressTInn_1->text().length() < 4){
            // Setup as server...
            ui->plainTextOut->insertPlainText(QString("New TCP Server telemetry: port=%1\n").arg(ui->TelemetryPortInn1->text().toInt()));
            tcpclientsTelemetri         = new tcpmodule(this, ui->TelemetryPortInn1->text().toInt(),1);
        }
        else{
            // Setup as client...
            ui->plainTextOut->insertPlainText(QString("New TCP client telemetry: address=%1:%2\n").arg(ui->SUdpAddressTInn_1->text()).arg(ui->TelemetryPortInn1->text().toInt()));
            tcpclientsTelemetri         = new tcpmodule(this, ui->SUdpAddressTInn_1->text(), ui->TelemetryPortInn1->text().toInt(),1);
        }
        tcpclientsTelemetri->text   = ui->plainTextOut;
#else
    if(xml->currentSettings.UseTTCPOut)
    {
        // Setup as server...
        qDebug() << QString("New TCP Server telemetry: port=%1\n").arg(xml->currentSettings.TTCPPortOut);
        tcpclientsTelemetri         = new tcpmodule(this, xml->currentSettings.TTCPPortOut,1);
    }
    else{
        // Setup as client...
        qDebug() << QString("New TCP client telemetry: address=%1:%2\n").arg(xml->currentSettings.TTCPAddressOut).arg(xml->currentSettings.TTCPPortOut);
        tcpclientsTelemetri         = new tcpmodule(this, xml->currentSettings.TTCPAddressOut, xml->currentSettings.TTCPPortOut,1);
    }
#endif
    tcpclientsTelemetri->nexttcpserver = NULL;

    for(int i=1; i < MAXCHANNELS+1; i++){
        udpclientsTelemetri[i]->nextudpmodul[0]=NULL;
        udpclientsTelemetri[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
        udpclientsTelemetri[i]->nexttcpserver[1]=tcpclientsTelemetri;                  // Do NOT send TCP data to this port...
        tcpclientsTelemetri->nextudpmodul[i] = udpclientsTelemetri[i];
    }
#ifndef HEADLESS
}
else{
for(int i=1; i < MAXCHANNELS+1; i++){
    udpclientsTelemetri[0]->nextudpmodul[i-1]=udpclientsTelemetri[i];
    udpclientsTelemetri[i]->nextudpmodul[0]=udpclientsTelemetri[0];                  // Do NOT send TCP data to this port...
    udpclientsTelemetri[i]->nexttcpserver[0]=NULL;                  // Do NOT send TCP data to this port...
    udpclientsTelemetri[i]->nexttcpserver[1]=NULL;                  // Do NOT send TCP data to this port...
}
}
#endif

bitrate->start(10*1000); // 1 min timeout...
#ifndef HEADLESS
ui->plainTextOut->insertPlainText(tr("Running ...\n"));
#endif
qDebug() << "System set up...";
}

#ifndef HEADLESS
void MainWindow::on_pushButton_3_clicked()
{

    if(udpclientsVideo[0])
        udpclientsVideo[0]->incount = 0;

    if(udpclientsVideo[0])
        udpclientsVideo[0]->outcount = 0;

    if(udpclientsVideo[1])
        udpclientsVideo[1]->incount = 0;

    if(udpclientsVideo[1])
        udpclientsVideo[1]->outcount = 0;

    if(tcpclientsVideo)
        tcpclientsVideo->incount = 0;

    if(tcpclientsVideo)
        tcpclientsVideo->outcount = 0;

}

void MainWindow::on_pushButton_4_clicked()
{
    if(udpclientsTelemetri[0])
        udpclientsTelemetri[0]->incount = 0;

    if(udpclientsTelemetri[0])
        udpclientsTelemetri[0]->outcount = 0;

    if(udpclientsTelemetri[1])
        udpclientsTelemetri[1]->incount = 0;

    if(udpclientsTelemetri[1])
        udpclientsTelemetri[1]->outcount = 0;

    if(tcpclientsTelemetri)
        tcpclientsTelemetri->incount = 0;

    if(tcpclientsTelemetri)
        tcpclientsTelemetri->outcount = 0;


}

void MainWindow::on_pushButton_2_clicked()
{

    QMessageBox::question(this, "Cancel",
                          " Copyright (C) 2017 Kongsberg Maritime AS.\n"
                          " Contact: http://www.kongsberg.com\n"
                          "\n"
                          " This file is part of the Kongsberg Maritime Lo-rate Video Project.\n"
                          "\n"
                          " $QT_BEGIN_LICENSE:BSD$\n"
                          "\n"
                          " THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND\n"
                          " CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,\n"
                          " INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
                          " MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
                          " DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR\n"
                          " CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"
                          " SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n"
                          " BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n"
                          " SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n"
                          " INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,\n"
                          " WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n"
                          " NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
                          " OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH\n"
                          " DAMAGE.\n"
                          "\n"
                          " $QT_END_LICENSE$",
                          QMessageBox::Cancel);
}

void MainWindow::on_radioButtonUDP_clicked(bool checked)
{
    if(checked){
        ui->VideoPortInn1->setText(QString("%1").arg(62011));
    }
}

void MainWindow::on_radioButtonTCP_clicked(bool checked)
{
    if(checked){
        ui->VideoPortInn1->setText(QString("%1").arg(62030));
    }
}

void MainWindow::on_UdpPort_textChanged(const QString &arg1)
{
    int x=arg1.toInt();

    ui->SUdpPort_2->setText(tr("%1").arg(x+1));
    ui->SUdpPort_3->setText(tr("%1").arg(x+2));
    ui->SUdpPort_4->setText(tr("%1").arg(x+3));
    ui->SUdpPort_5->setText(tr("%1").arg(x+4));
}

void MainWindow::on_pushButton_5_clicked()
{
    s_settings->show();
}

void MainWindow::on_IoT_EventHub_clicked()
{
    (void)printf("\nPrepare to sent!...\n");
#ifdef IOT
    if(!connect_to_event_hub(EH_HOST, EH_NAME, EH_KEY, EH_PUBLISHER))
    {
        char msgText[5000];
        sprintf(msgText,"[{\"assetid\": \"ConnectedDrone001\",\"properties\": [{\"property\": \"Lat\",\"doublevalue\":%f,\"stringvalue\": \"stringvalue1\"},{\"property\": \"Lon\",\"doublevalue\": %f,\"stringvalue\": \"stringvalue1\"}],\"timezone\": \"UTC\",\"timestamp\": \"2016-10-30 23:00:00\"}]",60.120,12.43);

        send_to_event_hub((unsigned char *)msgText,strlen(msgText));

        sprintf(msgText,"{\n"
                        "\"$schema\": \"http://json-schema.org/draft-04/schema#\", \"name\": \"Sample\",\n"
                        "\"type\": \"object\",\n"
                        "\"properties\": {\n"
                        "\"deviceid\": {\n"
                        "\"type\": \"string\",\n"
                        "\"description\": \"Device identifier\"\n"
                        "}, \"property\": {\n"
                        "\"type\": \"string\",\n"
                        "\"description\": \"Property identifier\" },\n"
                        "\"format\": {\n"
                        "\"type\": \"string\",\n"
                        "\"description\": \"Message format including version number\"\n"
                        "}, \"timezone\": {\n"
                        "\"type\": \"string\",\n"
                        "\"description\": \"Time zone for starttime and endttime in values array\"\n"
                        "}, \"unit\": {\n"
                        "\"type\": \"string\",\n"
                        "\"description\": \"values unit\" },\n"
                        "\"values\": { \"type\": \"array\",\n"
                        "\"items\": {\n"
                        "\"type\": \"object\",\n"
                        "\"properties\": { \"starttime\": {\n"
                        "\"type\": \"string\", \"format\": \"date-time\",\n"
                        "\"description\": \"start time of value in time zone of timezone (or UTC if missing)\"\n"
                        "}, \"endtime\": {\n"
                        "\"type\": \"string\", \"format\": \"date-time\",\n"
                        "\"description\": \"end time of value in time zone of timezone (or UTC if missing)\"\n"
                        "}, \"value\": {\n"
                        "\"type\": \"number\", \"description\": \"value\"\n"
                        "}, \"message\": {\n"
                        "\"type\": \"string\",\n"
                        "\"description\": \"status modifier or notification message\"\n"
                        "} }\n"
                        "} }\n"
                        "},\n"
                        "\"required\": [\"deviceid\", \"values\"] }\n");

        //      send_to_event_hub((unsigned char *)msgText,strlen(msgText));

        (void)close_event_hub();
    }
#endif
    (void)printf("\nMessage sent!...\n");

}
#endif

void MainWindow::on_pushButton_clicked()
{
#ifndef HEADLESS
    ui->plainTextOut->insertPlainText(tr("Close ...\n"));
    this->close();
#endif
}

#ifndef HEADLESS
#include <QtWidgets>
#include <QDialog>
#endif
#include <QtNetwork>
#include "udpmodule.h"
#include "tcpmodule.h"

#define HEAD (8+2)  // preamble  size  crc
QByteArray head;
//const unsigned char UN[] = {0x55,0xaa,0x00,0xff,0x5a,0xa5,0x0f,0xf0};
const unsigned char UN[] = {'T','E','S','T','D','A','T','A'};  // For NetCat test...

tcpmodule::tcpmodule(QObject *parent, int port, int _comtype ) :
    QObject(parent),
    droneudpchannel(NULL),
    stat(false)
{
    Localudpport = port;
    LocalAddress = "";
    serial = NULL;
    comtype = _comtype;

    head.setRawData((char*)UN,8);

    //    countinn=NULL;
    //    countout=NULL;
    incount=0;
    outcount=0;

    for(int i = 0; i<MAXCHANNELS+1; i++){
        tcpServerConnection[i]=NULL;
        nextudpmodul[i]=NULL;
    }

    while (!tcpServer.isListening() && !tcpServer.listen(QHostAddress::Any,Localudpport)) {
#ifdef HEADLESS
        qDebug() << "Error...";
#else
        QMessageBox::StandardButton ret = QMessageBox::critical((QWidget *)parent,
                                                                tr("TCP Server"),
                                                                tr("Unable to start the test: %1.")
                                                                .arg(tcpServer.errorString()),
                                                                QMessageBox::Retry
                                                                | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
            return;
#endif
    }

    connect(&tcpServer, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
}


tcpmodule::tcpmodule(QObject *parent, QString a, int port, int _comtype  ) :
    QObject(parent),
    droneudpchannel(NULL),
    stat(false)
{
    Localudpport = port;
    LocalAddress = a;
    serial = NULL;
    comtype = _comtype;

    head.setRawData((char*)UN,8);

    for(int i = 0; i<MAXCHANNELS+1; i++){
        tcpServerConnection[i]=NULL;
        nextudpmodul[i]=NULL;
    }

    tcpServerConnection[0] = new QTcpSocket(this);
    tcpServerConnection[0]->connectToHost(LocalAddress,Localudpport);
    connect(tcpServerConnection[0], SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(tcpServerConnection[0], SIGNAL(readyRead()),this, SLOT(updateServerProgress1()));
    connect(tcpServerConnection[0], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError1(QAbstractSocket::SocketError)));
}

void tcpmodule::slotConnected(void)
{
#ifdef HEADLESS
    qDebug() << "Connection found..." << endl;
#else
    text->insertPlainText(tr("Connection found...\n"));
#endif
}

void tcpmodule::acceptConnection()
{
    int num;
    for(num = 0; num<MAXCHANNELS+1; num++){
        if(tcpServerConnection[num]==NULL) break;
        if(!tcpServerConnection[num]->isOpen()){
            tcpServerConnection[num]->close();
            delete(tcpServerConnection[num]);
            break;
        }
    }
    if(num == MAXCHANNELS+1){
#ifdef HEADLESS
        qDebug() << "TCP Server full !!!" << endl;
#else
        text->insertPlainText(tr("TCP Server full !!!\n"));
#endif
        return;
    }

    tcpServerConnection[num] = tcpServer.nextPendingConnection();

    switch(num){
    case 0:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress1()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError1(QAbstractSocket::SocketError)));
        break;
    case 1:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress2()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError2(QAbstractSocket::SocketError)));
        break;
    case 2:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress3()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError3(QAbstractSocket::SocketError)));
        break;
    case 3:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress4()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError4(QAbstractSocket::SocketError)));
        break;
    case 4:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress5()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError5(QAbstractSocket::SocketError)));
        break;
    case 5:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress6()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError6(QAbstractSocket::SocketError)));
        break;
    case 6:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress7()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError7(QAbstractSocket::SocketError)));
        break;
    case 7:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress8()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError8(QAbstractSocket::SocketError)));
        break;
    case 8:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress9()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError9(QAbstractSocket::SocketError)));
        break;
    case 9:
        connect(tcpServerConnection[num], SIGNAL(readyRead()),this, SLOT(updateServerProgress10()));
        connect(tcpServerConnection[num], SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError10(QAbstractSocket::SocketError)));
        break;
    }
#ifdef HEADLESS
    qDebug() << "Accepted connection" << endl;
#else
    text->insertPlainText(tr("Accepted connection\n"));
#endif
    //    tcpServer.close();

}

// Received data from the clients...
void tcpmodule::updateServerProgress1(){ updateServerProgress(0);}
void tcpmodule::updateServerProgress2(){ updateServerProgress(1);}
void tcpmodule::updateServerProgress3(){ updateServerProgress(2);}
void tcpmodule::updateServerProgress4(){ updateServerProgress(3);}
void tcpmodule::updateServerProgress5(){ updateServerProgress(4);}
void tcpmodule::updateServerProgress6(){ updateServerProgress(5);}
void tcpmodule::updateServerProgress7(){ updateServerProgress(6);}
void tcpmodule::updateServerProgress8(){ updateServerProgress(7);}
void tcpmodule::updateServerProgress9(){ updateServerProgress(8);}
void tcpmodule::updateServerProgress10(){ updateServerProgress(9);}

// Received data from the clients...
void tcpmodule::updateServerProgress(int connection)
{
    char dta[15000];
    int size;

    do{
        QByteArray buff;
        //     buff = tcpServerConnection[connection]->readAll();
        size = tcpServerConnection[connection]->read(dta,1400); //->readAll();
        buff.setRawData(dta,size);

        //    if(buff.length() > 1400)
        //        text->insertPlainText(tr("Error %1").arg(buff.length()));

        incount+=buff.length();
        //        if(countinn!=NULL)
        //            countinn->display(incount/1000000);

        if(droneudpchannel != NULL){
            tcptoudpsend(buff);
        }
        else // If this is a client then send to all...
        {
            // Set up transmitters channel...
            if(LocalAddress != ""){
                tcptoudpsend(buff);
            }

            if(nexttcpserver != NULL)
                nexttcpserver->send(buff);
        }

        if(serial){
            if(serial->isOpen()){
                serial->flush();
                serial->waitForBytesWritten(100);
                serial->write(buff);
            }
        }
    }while(size == 1400);
}

void tcpmodule::tcptoudpsend(QByteArray buff)
{
    if(comtype == 0){
        enum{
            LOOK1,SIZE1,SIZE2,DATA
        };
        static QByteArray msg;
        static int state=LOOK1;
        static int size=0, count=0, step=0;

        for(int i = 0; i < buff.size(); i++){
            switch(state){
            case LOOK1:
                if(buff.at(i) == head.at(step)){
                    if(++step == head.size())
                        state = SIZE1;
                }
                else{
                    step = 0;
                    //     text->insertPlainText(tr("SyncLost!"));
                }
                break;

            case SIZE1:
                size = (buff.at(i) << 8) & 0x0000FF00;
                state = SIZE2;
                step = 0;
                break;

            case SIZE2:
                size|= (buff.at(i) << 0) & 0x000000FF;
                if(size){
                    msg.resize(size);
                    count = 0;
                    state = DATA;
                }
                else
                    state = LOOK1;
                break;

            case DATA:
                msg[count]=buff[i];
                if(++count == size){
                    if(droneudpchannel != NULL){
                        droneudpchannel->send(msg);
                    }
                    else{
                        for(int i=1; i < MAXCHANNELS+1; i++){
                            if(nextudpmodul[i] != NULL){
                                nextudpmodul[i]->send(msg);
                            }
                        }
                    }
                    state = LOOK1;
                }
                break;

            default:
                state = LOOK1;
                break;
            }
        }
    }
    else{
        for(int i=1; i < MAXCHANNELS+1; i++){
            if(nextudpmodul[i] != NULL){
                nextudpmodul[i]->send(buff);
            }
        }
    }
}

void tcpmodule::displayError1(QAbstractSocket::SocketError socketError){ displayError(socketError,0);}
void tcpmodule::displayError2(QAbstractSocket::SocketError socketError){ displayError(socketError,1);}
void tcpmodule::displayError3(QAbstractSocket::SocketError socketError){ displayError(socketError,2);}
void tcpmodule::displayError4(QAbstractSocket::SocketError socketError){ displayError(socketError,3);}
void tcpmodule::displayError5(QAbstractSocket::SocketError socketError){ displayError(socketError,4);}
void tcpmodule::displayError6(QAbstractSocket::SocketError socketError){ displayError(socketError,5);}
void tcpmodule::displayError7(QAbstractSocket::SocketError socketError){ displayError(socketError,6);}
void tcpmodule::displayError8(QAbstractSocket::SocketError socketError){ displayError(socketError,7);}
void tcpmodule::displayError9(QAbstractSocket::SocketError socketError){ displayError(socketError,8);}
void tcpmodule::displayError10(QAbstractSocket::SocketError socketError){ displayError(socketError,9);}

void tcpmodule::displayError(QAbstractSocket::SocketError socketError, int num)
{
    if (socketError == QTcpSocket::RemoteHostClosedError){
#ifdef HEADLESS
        qDebug() << tr("Connection closed on %1...\n").arg(num) << endl;
        qDebug() << tr("Network error: The following error occurred: %1.\n").arg(tcpServerConnection[num]->errorString()) << endl;
#else
        text->insertPlainText(tr("Connection closed on %1...\n").arg(num));
        text->insertPlainText(tr("Network error: The following error occurred: %1.\n").arg(tcpServerConnection[num]->errorString()));
#endif
        tcpServerConnection[num]->close();
        /*
        if(LocalAddress == ""){
            delete(tcpServerConnection[num]);
            tcpServerConnection[num] = NULL;
        }

        */
        //      return;
    }
    //    if(socketError == QTcpSocket::SocketTimeoutError){
    if(LocalAddress != ""){
        tcpServerConnection[0]->close();
        tcpServerConnection[0]->connectToHost(LocalAddress,Localudpport);
    }
    //    }
    //    tcpServer.close();
}

int tcpmodule::send(QByteArray datagram)
{
    QByteArray msg,headx;
    int ret = 0;

    if(datagram.size())
    {
        headx = head;
        headx.resize(HEAD);
        headx[8]= (datagram.size() >> 8) & 0xFF;
        headx[9]= (datagram.size() >> 0) & 0xFF;
        msg = headx + datagram;

        for(int i=0; i < MAXCHANNELS; i++){
            if(tcpServerConnection[i] != NULL){
                if(tcpServerConnection[i]->isWritable()){
                    outcount+=msg.length();
                    if(comtype == 0)
                    {
                        ret = tcpServerConnection[i]->write(msg);
                    }
                    else{
                        ret = tcpServerConnection[i]->write(datagram);
                    }
                    if(ret == -1){
#ifdef HEADLESS
                        qDebug() <<tr("TCP Send error on node %1\n").arg(i) << endl;
#else
                        text->insertPlainText(tr("TCP Send error on node %1\n").arg(i));
#endif
                    }
                }
            }
        }
        //        if(countout != NULL && outcount != 0)
        //            countout->display(outcount/1000000);
        //  else
        //      text->insertPlainText(tr("Error"));
    }
    return ret;
}

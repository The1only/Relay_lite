/* Remember:
 I recommend checking the
 System Preferences > Network > Advanced > Proxies > Automatic Proxy Configuration
 for a Proxy Configuration File hosted on http://.
 I had Auto Proxy Detection on.  Turned that off and the message went away.
*/

#ifndef HEADLESS
#include <QtWidgets>
#endif
#include <QtNetwork>
#include "udpmodule.h"
#include "tcpmodule.h"

udpmodule::udpmodule(QObject *parent, int serverport, int n, QHostAddress *addr, quint16 remoteport) :
    QObject(parent),
    udpSocket(NULL),
    udpport(0),
    num(n),
    stat(false)
{
    Localudpport = serverport;
//    countinn=NULL;
//    countout=NULL;
    incount=0;
    outcount=0;
    serial = NULL;

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
//    timer->setInterval(60*1000); // 1 min timeout...

    timerout = new QTimer();
    connect(timerout, SIGNAL(timeout()), this, SLOT(on_timerout()));
//    timerout->setInterval(30*1000); // 30 sec timeout...

    // Dynamic or static target...
    if(remoteport){
        udpport = remoteport;
        address = *addr;
        stat = true;
    }else{
  //      timer->setInterval(60*1000); // 1 min timeout...
    }

    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    if(stat==false && Localudpport)
        udpSocket->bind(Localudpport);

    for(int i=0; i < num; i++)
        nextudpmodul[i]=NULL;

    nexttcpserver[0] = NULL;
    nexttcpserver[1] = NULL;

    // Dynamic or static target...
    if(remoteport){
//        udpSocket->writeDatagram("HartBeat",address,udpport);
 //       timerout->start();
    }
}

int udpmodule::send(QByteArray datagram)
{
    if(udpport){
        outcount+=datagram.length();
//        if(countout != NULL)
//            countout->display(outcount/1000000);

 //       timerout->start();
        return udpSocket->writeDatagram( datagram,address,udpport);
    }
    else
        return 0;
}

void udpmodule::readPendingDatagrams(void)
{
//    timer->stop();

    while (udpSocket->hasPendingDatagrams())
    {
        datagram.resize(udpSocket->pendingDatagramSize());
        if(stat == true)
            udpSocket->readDatagram(datagram.data(), datagram.size(),NULL,NULL);
        else{
            int oldport = udpport;
            udpSocket->readDatagram(datagram.data(), datagram.size(),&address, &udpport);
            if(oldport != udpport){
#ifdef HEADLESS
               qDebug() << QString("NewConnection: %1\n").arg(udpport) << endl;
#else
               text->insertPlainText(QString("NewConnection: %1\n").arg(udpport));
#endif
            }
        }

        if(datagram.length()>0){
            incount+=datagram.length();
//            if(countinn!=NULL)
//                countinn->display(incount/1000000);
        }
/*
        if( !strncmp(datagram.data(),"RA_START_DATA=TRUE",18)){

        }
        else
        */
        {
            // Set up transmitters channel...
            for(int i=0; i < num; i++){
                if(nextudpmodul[i] != NULL){
                    nextudpmodul[i]->send(datagram);
                }
            }
            if(nexttcpserver[0] != NULL)
                nexttcpserver[0]->send(datagram);

            if(nexttcpserver[1] != NULL)
                nexttcpserver[1]->send(datagram);

            if(serial){
                if(serial->isOpen()){
                    serial->flush();
                    serial->waitForBytesWritten(100);
                    serial->write(datagram);
                }
            }
        }
    }
    //timer->start();
}



// Timeout then remove client...
void udpmodule::on_timerout()
{
    // Dynamic or static target...
    if(stat){
 //       udpSocket->writeDatagram("HartBeat",address,udpport);
    }

}


// Timeout then remove client...
void udpmodule::on_timer()
{
    udpport=0;

    //timer->stop();
#ifdef HEADLESS
    qDebug() << "Timeout ..." << endl;
#else
    text->insertPlainText(tr("Timeout ...\n"));
#endif
    if(udpSocket){
        udpSocket->close();
        delete(udpSocket);
    }

    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    if(stat==false && Localudpport)
        udpSocket->bind(Localudpport);
}

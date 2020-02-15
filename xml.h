#ifndef XML_H
#define XML_H

#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QObject>
#include <QSerialPort>

// We need to fill this from the XML file...
struct Settings{
    QString name;

    unsigned short VPortIn;
    QString VideoAddressIn;
    bool UseVideoTCP;
    unsigned short TPortIn;
    QString TAddressIn;
    bool UseTTCP;

    QString VideoUDPAddressOut1;
    unsigned short VideoUDPPortOut1;
    QString VideoUDPAddressOut2;
    unsigned short VideoUDPPortOut2;
    QString VideoUDPAddressOut3;
    unsigned short VideoUDPPortOut3;
    QString VideoUDPAddressOut4;
    unsigned short VideoUDPPortOut4;
    QString VideoTCPAddressOut;
    unsigned short VideoTCPPortOut;
    bool UseVideoTCPOut;

    QString TUDPAddressOut1;
    unsigned short TUDPPortOut1;
    QString TUDPAddressOut2;
    unsigned short TUDPPortOut2;
    QString TUDPAddressOut3;
    unsigned short TUDPPortOut3;
    QString TUDPAddressOut4;
    unsigned short TUDPPortOut4;
    QString TTCPAddressOut;
    unsigned short TTCPPortOut;
    bool UseTTCPOut;
};

class XmlStreamLint : public QObject
{
    Q_OBJECT
public:
    XmlStreamLint(QObject *parent = 0, QString xmlfile="");

    int  error;
    bool readdy;

    enum ExitCode
    {
        Success,
        ParseFailure,
        ArgumentError,
        WriteError,
        FileFailure
    };


    Settings currentSettings;

signals:

public slots:
};

#endif // XML_H

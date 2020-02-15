#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QDir>
#include <QDomDocument>

#include "xml.h"

// To get cout
#include <iostream>
using namespace std;

XmlStreamLint::XmlStreamLint(QObject *parent, QString xmlfile) : QObject(parent), error(Success)
{
//    QTextStream errorStream(stderr);
    readdy = false;
    QString string;

    if(xmlfile != "")
        string = xmlfile;
    else
        string = tr("%1").arg(":/config.xml");

    QDomDocument document;

    QFile xmlFile(string);

    if (!xmlFile.exists() || !xmlFile.open(QFile::ReadOnly | QFile::Text)) {
        cout<<"File does not exist "<<string.toStdString() <<endl;
        return;
    }
    QDomDocument domDocument;
    domDocument.setContent(&xmlFile);
    QDomElement topElement = domDocument.documentElement();
    QDomNode domNode = topElement.firstChild();

    while (!domNode.isNull()) {
        QDomElement domElement = domNode.toElement();
        if (!domElement.isNull()) {

            if (domElement.tagName() == "Inputs") {
                QDomNode node = domElement.firstChild();
                while (!node.isNull()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        const QString tagName(element.tagName());
                        if (tagName == "VPortIn") {
                            currentSettings.VPortIn = element.text().toInt();
                        } else if (tagName == "VideoAddressIn") {
                            currentSettings.VideoAddressIn = element.text();
                        } else if (tagName == "TPortIn") {
                            currentSettings.TPortIn = element.text().toInt();
                        } else if (tagName == "TAddressIn") {
                            currentSettings.TAddressIn = element.text();
                        } else if (tagName == "UseVideoTCP") {
                            currentSettings.UseVideoTCP = element.text().toInt();
                        } else if (tagName == "UseTTCP") {
                            currentSettings.UseTTCP = element.text().toInt();
                        }
                    }
                    node = node.nextSibling();
                }
            }

            if (domElement.tagName() == "Outputs") {
                QDomNode node = domElement.firstChild();
                while (!node.isNull()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        const QString tagName(element.tagName());
                        if (tagName == "VideoUDPAddressOut1") {
                            currentSettings.VideoUDPAddressOut1 = element.text();
                        } else if (tagName == "VideoUDPPortOut1") {
                            currentSettings.VideoUDPPortOut1 = element.text().toInt();
                        } else if (tagName == "VideoUDPAddressOut2") {
                                currentSettings.VideoUDPAddressOut2 = element.text();
                        } else if (tagName == "VideoUDPPortOut2") {
                                currentSettings.VideoUDPPortOut2 = element.text().toInt();
                        } else if (tagName == "VideoUDPAddressOut3") {
                                currentSettings.VideoUDPAddressOut3 = element.text();
                        } else if (tagName == "VideoUDPPortOut3") {
                                currentSettings.VideoUDPPortOut3 = element.text().toInt();
                        } else if (tagName == "VideoUDPAddressOut4") {
                                currentSettings.VideoUDPAddressOut4 = element.text();
                        } else if (tagName == "VideoUDPPortOut4") {
                                currentSettings.VideoUDPPortOut4 = element.text().toInt();

                        } else if (tagName == "TUDPAddressOut1") {
                                currentSettings.TUDPAddressOut1 = element.text();
                        } else if (tagName == "TUDPPortOut1") {
                                currentSettings.TUDPPortOut1 = element.text().toInt();
                        } else if (tagName == "TUDPAddressOut2") {
                                currentSettings.TUDPAddressOut2 = element.text();
                        } else if (tagName == "TUDPPortOut2") {
                                currentSettings.TUDPPortOut2 = element.text().toInt();
                        } else if (tagName == "TUDPAddressOut3") {
                                currentSettings.TUDPAddressOut3 = element.text();
                        } else if (tagName == "TUDPPortOut3") {
                                currentSettings.TUDPPortOut3 = element.text().toInt();
                        } else if (tagName == "TUDPAddressOut4") {
                                currentSettings.TUDPAddressOut4 = element.text();
                        } else if (tagName == "TUDPPortOut4") {
                                currentSettings.TUDPPortOut4 = element.text().toInt();

                        } else if (tagName == "VideoTCPAddressOut") {
                                currentSettings.VideoTCPAddressOut = element.text();
                        } else if (tagName == "VideoTCPPortOut") {
                                currentSettings.VideoTCPPortOut = element.text().toInt();
                        } else if (tagName == "UseVideoTCPOut") {
                                currentSettings.UseVideoTCPOut = element.text().toInt();
                        } else if (tagName == "TTCPAddressOut") {
                                currentSettings.TTCPAddressOut = element.text();
                        } else if (tagName == "TTCPPortOut") {
                                currentSettings.TTCPPortOut = element.text().toInt();
                        } else if (tagName == "UseTTCPOut") {
                                currentSettings.UseTTCPOut = element.text().toInt();
                        }
                    }
                    node = node.nextSibling();
                }
            }
        }
        domNode = domNode.nextSibling();
    }
    readdy = true;
}

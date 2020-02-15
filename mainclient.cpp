#ifndef HEADLESS
#include <QApplication>
#endif
#include "mainwindowclient.h"

int main(int argc, char *argv[])
{
#ifdef HEADLESS
    QCoreApplication Application(argc, argv);
    int argumentCount = QCoreApplication::arguments().size();
    QStringList argumentList = QCoreApplication::arguments();
    QTextStream standardOutput(stdout);

    QString xmlfile= "";
    QString run = "";

    if(argc>=2) xmlfile= argv[1];
    if(argc>=3) run= argv[2];

    MainWindow *w;
    w = new MainWindow(NULL,xmlfile,run);

    return Application.exec();
#else
    QApplication Application(argc, argv);
    QString xmlfile= "";
    QString run = "";
    MainWindow *w;

    if(argc>=2) xmlfile= argv[1];
    if(argc>=3) run= argv[2];

    w = new MainWindow(NULL,xmlfile,run);
    w->show();
    return Application.exec();
#endif
}



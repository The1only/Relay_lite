#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w;
    QString portV= "";
    QString portT= "";

    QString add1= "";
    QString add2= "";
    QString add3= "";
    QString add4= "";
    QString add5= "";

    QString add6= "";
    QString add7= "";
    QString add8= "";
    QString add9= "";
    QString add10= "";

    if(argc>=2) portV= argv[1];
    if(argc>=3) portT= argv[2];

    if(argc>=4) add1= argv[3];
    if(argc>=5) add2= argv[4];
    if(argc>=6) add3= argv[5];
    if(argc>=7) add4= argv[6];
    if(argc>=8) add5= argv[7];

    if(argc>=9) add6= argv[8];
    if(argc>=10) add7= argv[9];
    if(argc>=11) add8= argv[10];
    if(argc>=12) add9= argv[11];
    if(argc>=13) add10= argv[12];

    fprintf(stderr,"Args: %d",argc);

    w = new MainWindow(NULL,&portV,&portT,&add1,&add2,&add3,&add4,&add5,&add6,&add7,&add8,&add9,&add10);
    w->show();

    return a.exec();
}

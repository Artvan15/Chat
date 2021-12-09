#include "ClientFront.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    
    QApplication a(argc, argv);

    ClientFront w;
    w.show();
    return a.exec();
}

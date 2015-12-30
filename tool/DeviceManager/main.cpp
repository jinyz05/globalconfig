#include <QtGui/QApplication>
#include "devicelistdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DeviceListDialog w;
    w.show();
    return a.exec();
}

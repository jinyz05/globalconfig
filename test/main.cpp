#include <QtCore/QCoreApplication>
#include "../sharepreference.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    SharePreference::Instance(10000)->Start(NULL,"T318A","1234567890123455","test","test");
    return a.exec();
}

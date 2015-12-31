#include <QtCore/QCoreApplication>
#include "httpclient.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    HTTPClient *client=new HTTPClient(NULL);
    client->httpLogin();
    return a.exec();
}

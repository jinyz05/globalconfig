#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QSslConfiguration>
#include "httpclient_p.h"
#include "httpnetworkmanager.h"
#include "cJSON.h"

//#define HTTP_DEBUG 1
#ifdef HTTP_DEBUG
#define HTTP_TRACES() \
    do{printf("%s,%s,%d\n",__FILE__,__func__,__LINE__);}while(0)

#define HTTP_TRACE(s) \
    do{printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,s);}while(0)
#else
#define HTTP_TRACE(s)
#define HTTP_TRACES()
#endif
class HTTPClient : public QObject
{
Q_OBJECT

public:
    explicit HTTPClient(QObject *parent);

    QNetworkReply* httpGet(QUrl dst,QByteArray key);
    QNetworkReply* httpPost(QUrl dst,QByteArray key,QByteArray data);
    QNetworkReply* httpDelete(QUrl dst,QByteArray key);

public slots:
    QNetworkReply* httpLogin();
private slots:
#ifdef QT_NO_OPENSSL
#else
    void onIgnoreSSLErrors(QNetworkReply* reply,QList<QSslError> errors);
#endif
    void ReplyRecv();
private:    


    HttpNetworkManager *mHttpManager;
    QNetworkAccessManager *mLoginManager;

    HTTPClientPrivate *d_ptr;
    friend class HTTPClientPrivate;        

    QNetworkReply *mCurrentReply;
};

#endif // HTTPCLIENT_H

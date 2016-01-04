#ifndef HTTPCLIENTPRIVATE_H
#define HTTPCLIENTPRIVATE_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QNetworkRequest>
#include "httpnetworkmanager.h"

class HTTPClient;

class HTTPClientPrivate{

protected:
    enum Status{
        Idle,//初始状态
        Busy//请求中
    };

private:
    HTTPClientPrivate();

    QNetworkReply* post(QUrl dst,QByteArray key,QByteArray data);
    QNetworkReply* get(QUrl dst,QByteArray key);
    QNetworkReply* del(QUrl dst,QByteArray key);
    QNetworkReply* login();
    void send();


    QNetworkRequest mRequest;
    QMutex mLocker;

    friend class HTTPClient;
    HTTPClient* q_ptr;
};

#endif // HTTPCLIENTPRIVATE_H

#include "httpnetworkmanager.h"
#include "httpclient.h"
#include <QMutexLocker>
HttpNetworkManager::HttpNetworkManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
    QNetworkCookieJar* cookieJar = NetworkCookieJar::GetInstance();
    setCookieJar(cookieJar);
    cookieJar->setParent(0);

}

QNetworkReply *HttpNetworkManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
#ifdef HTTP_DEBUG
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,request.url().toString().toLocal8Bit().constData());
#endif
    QNetworkRequest req(request);
#ifdef QT_NO_OPENSSL
#else
//    QSslConfiguration config;
//    config.setPeerVerifyMode(QSslSocket::VerifyNone); //configure
//    config.setProtocol(QSsl::TlsV1_0);                                //configure
//    req.setSslConfiguration(config);                                  //setSslConfiguration
#endif
    //req.setRawHeader("User-Agent", PRODUCT_NAME);

    QNetworkReply *reply = QNetworkAccessManager::createRequest(op, req, outgoingData);
    return reply;
}
NetworkCookieJar::NetworkCookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{

    keepAliveCookie = QNetworkCookie("ka", "open");
    load ();
}

NetworkCookieJar::~NetworkCookieJar()
{
}

void NetworkCookieJar::load()
{

    QMutexLocker lock(&mutex);
    Q_UNUSED(lock);

}

NetworkCookieJar* NetworkCookieJar::GetInstance()
{

    static NetworkCookieJar cookieJar;
    return &cookieJar;
}

void NetworkCookieJar::clearCookies()
{
    QList<QNetworkCookie> emptyList;
    setAllCookies(emptyList);
}

QList<QNetworkCookie> NetworkCookieJar::cookiesForUrl(const QUrl &url) const
{

    QMutexLocker lock(&mutex);
    Q_UNUSED(lock);
    QList<QNetworkCookie> cookies = QNetworkCookieJar::cookiesForUrl(url);

    return cookies;
}

bool NetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{

    QMutexLocker lock(&mutex);
    Q_UNUSED(lock);
    return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
}

QList<QNetworkCookie> NetworkCookieJar::cookies() const
{
    return allCookies ();
}

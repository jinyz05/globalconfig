#ifndef HTTPNETWORKMANAGER_H
#define HTTPNETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QMutex>
#include <QNetworkCookieJar>
class HttpNetworkManager : public QNetworkAccessManager
{
Q_OBJECT
public:
    explicit HttpNetworkManager(QObject *parent = 0);
protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
signals:

public slots:

};
class NetworkCookieJar : public QNetworkCookieJar
{
public:
    static NetworkCookieJar* GetInstance();
    void clearCookies();

    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    QList<QNetworkCookie> cookies() const;
private:
    explicit NetworkCookieJar(QObject *parent = 0);
    ~NetworkCookieJar();
    void load();
    mutable QMutex mutex;
    QNetworkCookie keepAliveCookie;
};
#endif // HTTPNETWORKMANAGER_H

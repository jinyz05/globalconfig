#include "httpclient.h"

#include <QMutexLocker>

HTTPClient::HTTPClient(QObject *parent) :
    QObject(parent),d_ptr(new HTTPClientPrivate())
{    
    mCurrentReply=NULL;
    d_ptr->q_ptr=this;
    mHttpManager=new HttpNetworkManager(this);
    mLoginManager=new QNetworkAccessManager(this);
#ifdef QT_NO_OPENSSL
#else
    connect(mHttpManager,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),this,SLOT(onIgnoreSSLErrors(QNetworkReply*,QList<QSslError>)));
#endif
}
#include <QTimer>
#include <QByteArray>
QNetworkReply* HTTPClient::httpLogin()
{
    printf("%s,%s,%d\n",__FILE__,__func__,__LINE__);
    if(mCurrentReply)
    {
        mCurrentReply->abort();
        mCurrentReply->deleteLater();
        mCurrentReply=NULL;
    }
    mCurrentReply = d_ptr->login();

    QTimer::singleShot(600000,this,SLOT(httpLogin()));
    connect(mCurrentReply,SIGNAL(finished()),this,SLOT(ReplyRecv()));
    return mCurrentReply;
}
void HTTPClient::ReplyRecv()
{
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        if(reply->error()==QNetworkReply::NoError)
        {
            QByteArray content=reply->readAll();
            printf("content:%s\n",content.constData());
        }
    }    
}

/*
Referer: http://192.168.4.1:90/p/30247dd99271a6806206be0598a1cf9e/index.html?MTkyLjE2OC40LjE6OTAvbG9naW4=
Accept-Language: zh-CN
User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)
Content-Type: application/x-www-form-urlencoded
Accept-Encoding: gzip, deflate
Host: 192.168.4.1:90
Content-Length: 382
Connection: Keep-Alive
Pragma: no-cache
Cookie: PHPSESSID=bhajlrnb6q3ctls8s95c83pjh4

username=jinyouzhi@hanwang.com.cn&assure_phone=&password1=1sndy%2Chanwang&password=%25B9%25FAA%2510%25AF%2504%2525%25B4K%25D2%25B9H%25DF
*/


QNetworkReply* HTTPClient::httpPost(QUrl dst,QByteArray key,QByteArray data)
{
    if(mCurrentReply)
    {
        mCurrentReply->abort();
        mCurrentReply->deleteLater();
        mCurrentReply=NULL;
    }
    mCurrentReply = d_ptr->post(dst,key,data);
    return mCurrentReply;

}
#ifdef QT_NO_OPENSSL
#else
void HTTPClient::onIgnoreSSLErrors(QNetworkReply* reply,QList<QSslError> errors)
{
    //for(int i=0;i<errors.size();i++)
    //    printf("SSL ERROR:%d,%s\n",errors.at(i).error(),errors.at(i).errorString().toLocal8Bit().constData());
    reply->ignoreSslErrors(errors);
}
#endif
QNetworkReply* HTTPClient::httpGet(QUrl dst,QByteArray key)
{
    if(mCurrentReply)
    {
        mCurrentReply->abort();
        mCurrentReply->deleteLater();
        mCurrentReply=NULL;
    }
    mCurrentReply = d_ptr->get(dst,key);
    return mCurrentReply;
}

QNetworkReply* HTTPClient::httpDelete(QUrl dst,QByteArray key)
{
    if(mCurrentReply)
    {
        mCurrentReply->abort();
        mCurrentReply->deleteLater();
        mCurrentReply=NULL;
    }
    mCurrentReply = d_ptr->del(dst,key);
    return mCurrentReply;
}



HTTPClientPrivate::HTTPClientPrivate():
        mLocker(QMutex::Recursive)
{
}

QNetworkReply* HTTPClientPrivate::post(QUrl url,QByteArray key,QByteArray data)
{    
    mRequest.setUrl(url);
    mRequest.setRawHeader("api-key",key);
    QNetworkReply* reply = q_ptr->mHttpManager->post(mRequest,data);
    return reply;
}

QNetworkReply* HTTPClientPrivate::get(QUrl url,QByteArray key)
{
    mRequest.setUrl(url);
    mRequest.setRawHeader("api-key",key);
    return q_ptr->mHttpManager->get(mRequest);
}
QNetworkReply* HTTPClientPrivate::del(QUrl url,QByteArray key)
{
#if QT_VERSION >= 0x040600
    mRequest.setUrl(url);
    mRequest.setRawHeader("api-key",key);
    return q_ptr->mHttpManager->deleteResource(mRequest);
#else
    mRequest.setUrl(url);
    mRequest.setRawHeader("api-key",key);
    return q_ptr->mHttpManager->get(mRequest);
#endif
}
QNetworkReply* HTTPClientPrivate::login()
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://192.168.4.1:90/login"));
    request.setRawHeader("Host","192.168.4.1:90");
    request.setRawHeader("Referer", "http://192.168.4.1:90/p/30247dd99271a6806206be0598a1cf9e/index.html?MTkyLjE2OC40LjE6OTAvbG9naW4=");
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    QList<QByteArray> list=request.rawHeaderList();
    foreach(QByteArray i,list)
    {
#ifdef HTTP_DEBUG
        printf("%s:%s\n",i.constData(),request.rawHeader(i).constData());
#endif
    }

    return q_ptr->mLoginManager->post(request,"login_type=login&username=jinyouzhi@hanwang.com.cn&password1=1sndy%2Chanwang&password=%25B9%25FAA%2510%25AF%2504%2525%25B4K%25D2%25B9H%25DF");
}

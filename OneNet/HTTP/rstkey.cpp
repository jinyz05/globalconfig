#include "rstkey.h"
#include "rstmanager.h"

RSTKey::RSTKey(QObject* parent,HTTPClient* client)
    :QObject(parent)
{
    mClient=client;
}

bool RSTKey::AddKey(QString devicename,QByteArray value)
{
    HTTP_TRACE(value.constData());
    QString path="http://api.heclouds.com/keys";
    QNetworkReply *reply=mClient->httpPost(QUrl(path),POOL_MASTERKEY,value);
    connect(reply,SIGNAL(finished()),this,SLOT(AddKeyResp()));
    return true;
}
void RSTKey::AddKeyResp()
{
    bool ok=false;
    QByteArray key_str;
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        if(reply->error()==QNetworkReply::NoError)
        {
            QByteArray data=reply->readAll();
            HTTP_TRACE(data.constData());

            cJSON *errnoJson=NULL;
            cJSON *root=cJSON_Parse(data.constData());
            if(root)
                errnoJson=cJSON_GetObjectItem(root,"errno");
            if(errnoJson && (errnoJson->valueint==0))
            {
                cJSON* data=cJSON_GetObjectItem(root,"data");
                if(data)
                {
                    cJSON* device_id=cJSON_GetObjectItem(data,"key");
                    if(device_id)
                    {
                        key_str=device_id->valuestring;
                        ok=true;
                    }
                }
            }
            if(root)
                cJSON_Delete(root);
        }
    }
    emit KeyAdded(ok,key_str);
}

bool RSTKey::DeleteKey(QString devicename,QByteArray key_string)
{
    QString path="http://api.heclouds.com/keys/";
    path+=key_string;
    QNetworkReply *reply=mClient->httpDelete(QUrl(path),POOL_MASTERKEY);
    connect(reply,SIGNAL(finished()),this,SLOT(DeleteKeyResp()));
    return true;
}
void RSTKey::DeleteKeyResp()
{
    bool ok=false;
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        if(reply->error()==QNetworkReply::NoError)
        {
            QByteArray data=reply->readAll();
#ifdef HTTP_DEBUG
            printf("%s,%s,%d,value:%s\n",__FILE__,__func__,__LINE__,data.constData());
#endif
            cJSON *errnoJson=NULL;
            cJSON *root=cJSON_Parse(data.constData());
            if(root)
                errnoJson=cJSON_GetObjectItem(root,"errno");
            if(errnoJson && (errnoJson->valueint==0))
            {
                ok=true;
            }
            if(root)
                cJSON_Delete(root);
        }
    }
    emit KeyDeleted(ok);
}
bool RSTKey::GetKey(QString devicename,QByteArray device_id)
{
    QString path="http://api.heclouds.com/keys";
    path+="?dev_id=";
    path+=device_id;
    QNetworkReply *reply=mClient->httpGet(QUrl(path),POOL_MASTERKEY);
    connect(reply,SIGNAL(finished()),this,SLOT(GetKeyResp()));
    return true;
}

bool RSTKey::GetKeys(QString devicename,int page)
{
    QString path="http://api.heclouds.com/keys";
    path+="?page=";
    path+=page;
    QNetworkReply *reply=mClient->httpGet(QUrl(path),POOL_MASTERKEY);
    connect(reply,SIGNAL(finished()),this,SLOT(GetKeyResp()));
    return true;
}
void RSTKey::GetKeyResp()
{
    bool ok=false;
    QByteArray content;
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        if(reply->error()==QNetworkReply::NoError)
        {
            content=reply->readAll();
            HTTP_TRACE(content.constData());

            cJSON *errnoJson=NULL;
            cJSON *root=cJSON_Parse(content.constData());
            if(root)
                errnoJson=cJSON_GetObjectItem(root,"errno");
            if(errnoJson && (errnoJson->valueint==0))
            {
                ok=true;
            }
            if(root)
                cJSON_Delete(root);
        }
    }
    emit KeyGetted(ok,content);
}


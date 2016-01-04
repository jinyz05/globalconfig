#include "rstdatapoint.h"

RSTDataPoint::RSTDataPoint(QObject* parent,HTTPClient* client)
    :QObject(parent)
{
    mClient=client;
}

void RSTDataPoint::AddDataPoint(QByteArray device_id,QByteArray data_key,QByteArray value)
{
    QString path="http://api.heclouds.com/devices/";
    path+=device_id;
    path+="/datapoints";
    HTTP_TRACE(value.constData());
    QNetworkReply *reply=mClient->httpPost(QUrl(path),data_key,value);
    connect(reply,SIGNAL(finished()),this,SLOT(AddDataPointResp()));
}
void RSTDataPoint::AddDataPointResp()
{
    bool ok=false;
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        if(reply->error()==QNetworkReply::NoError)
        {
            QByteArray content=reply->readAll();
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
    emit DataPointAdded(ok);
}


void RSTDataPoint::GetDataPoints(QByteArray device_id,QByteArray data_key,QString stream_id,QString start,QString end)
{
    QString path="http://api.heclouds.com/devices/";
    path+=device_id;
    path+="/datapoints";
    path+="?datastream_id=";
    path+=stream_id;
    path+="&start=";
    path+=start;
    path+="&end=";
    path+=end;
    path+="&limit=5000";
    QNetworkReply *reply=mClient->httpGet(QUrl(path),data_key);
    connect(reply,SIGNAL(finished()),this,SLOT(GetDataPointResp()));
}

void RSTDataPoint::GetDataPoints(QByteArray device_id,QByteArray data_key,QString start,QString end)
{
    QString path="http://api.heclouds.com/devices/";
    path+=device_id;
    path+="/datapoints";
    path+="&start=";
    path+=start;
    path+="&end=";
    path+=end;
    path+="&limit=1000";
    QNetworkReply *reply=mClient->httpGet(QUrl(path),data_key);
    connect(reply,SIGNAL(finished()),this,SLOT(GetDataPointResp()));
}
void RSTDataPoint::GetDataPointResp()
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
    emit DataPointGetted(ok,content);
}

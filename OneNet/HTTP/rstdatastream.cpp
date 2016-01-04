#include "rstdatastream.h"

RSTDataStream::RSTDataStream(QObject* parent,HTTPClient* client)
    :QObject(parent)
{
    mClient=client;
}

void RSTDataStream::AddDataStream(QByteArray device_id,QByteArray device_key,QByteArray value)
{
    QString path="http://api.heclouds.com/devices/";
    path+=device_id;
    path+="/datastreams";
    HTTP_TRACE(value.constData());
    QNetworkReply *reply=mClient->httpPost(QUrl(path),device_key,value);
    connect(reply,SIGNAL(finished()),this,SLOT(AddDataStreamResp()));
}
void RSTDataStream::AddDataStreamResp()
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
    emit DataStreamAdded(ok);
}


void RSTDataStream::GetDataStreams(QByteArray device_id,QByteArray data_key,QList<QByteArray> stream_ids)
{
    HTTP_TRACES();
    QString path="http://api.heclouds.com/devices/";
    path+=device_id;
    path+="/datastreams";
    if(stream_ids.size()>0)
    {
        QString ids;
        for(int i=0;i<stream_ids.size();i++)
        {
            if(i>0)
                ids+=",";
            ids+=stream_ids[i];
        }
        path+="?datastream_ids=";
        path+=ids;
    }
    QNetworkReply *reply=mClient->httpGet(QUrl(path),data_key);
    connect(reply,SIGNAL(finished()),this,SLOT(GetDataStreamResp()));
}


void RSTDataStream::GetDataStreamResp()
{
    HTTP_TRACES();
    bool ok=false;
    QByteArray content;
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        HTTP_TRACE(reply->errorString().toLocal8Bit().constData());
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
    emit DataStreamGetted(ok,content);
}

void RSTDataStream::DeleteDataStream(QByteArray device_id,QByteArray data_key,QByteArray stream_id)
{    
    QString path="http://api.heclouds.com/devices/";
    path+=device_id;
    path+="/datastreams/";
    path+=stream_id;
    QNetworkReply *reply=mClient->httpDelete(QUrl(path),data_key);
    connect(reply,SIGNAL(finished()),this,SLOT(DeleteDataStreamResp()));
}
void RSTDataStream::DeleteDataStreamResp()
{
    bool ok=false;
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        HTTP_TRACE(reply->errorString().toLocal8Bit().constData());
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
                ok=true;
            }
            if(root)
                cJSON_Delete(root);
        }
    }
    emit DataStreamDeleted(ok);
}

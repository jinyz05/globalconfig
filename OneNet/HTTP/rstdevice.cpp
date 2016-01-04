#include "rstdevice.h"
RSTDevice::RSTDevice(QObject* parent,HTTPClient* client)
    :QObject(parent)
{
    mClient=client;
}

void RSTDevice::AddDevice(QByteArray master_key,QByteArray value)
{
    QString path="http://api.heclouds.com/devices";
    QNetworkReply *reply=mClient->httpPost(QUrl(path),master_key,value);
    connect(reply,SIGNAL(finished()),this,SLOT(AddDeviceResp()));
}
void RSTDevice::AddDeviceResp()
{
    bool ok=false;
    QByteArray device_id_str;
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
                    cJSON* device_id=cJSON_GetObjectItem(data,"device_id");
                    if(device_id)
                    {
                        device_id_str=device_id->valuestring;
                    }
                }
                ok=true;
            }
            if(root)
                cJSON_Delete(root);
        }
    }
    emit DeviceAdded(ok,device_id_str);
}

void RSTDevice::DeleteDevice(QByteArray device_id,QByteArray device_key)
{
    QString path="http://api.heclouds.com/devices/";
    path+=device_id;
    QNetworkReply *reply=mClient->httpDelete(QUrl(path),device_key);
    connect(reply,SIGNAL(finished()),this,SLOT(DeleteDeviceResp()));
}
void RSTDevice::DeleteDeviceResp()
{
    bool ok=false;
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
                ok=true;
            }
            if(root)
                cJSON_Delete(root);
        }
    }
    emit DeviceDeleted(ok);
}
void RSTDevice::GetDevices(QByteArray master_key,QList<QByteArray> device_ids)
{
    QString path="http://api.heclouds.com/devices";
    if(device_ids.size()>0)
    {
        QString ids;
        for(int i=0;i<device_ids.size();i++)
        {
            if(i>0)
                ids+=",";
            ids+=device_ids[i];
        }
        path+="?device_id=";
        path+=ids;
    }
    QNetworkReply *reply=mClient->httpGet(QUrl(path),master_key);
    connect(reply,SIGNAL(finished()),this,SLOT(GetDeviceResp()));
}

void RSTDevice::GetDevices(QByteArray master_key,int page)
{
    QString path="http://api.heclouds.com/devices";
    path+="?page=";
    path+=page;
    QNetworkReply *reply=mClient->httpGet(QUrl(path),master_key);
    connect(reply,SIGNAL(finished()),this,SLOT(GetDeviceResp()));
}
void RSTDevice::GetDeviceResp()
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
    emit DeviceGetted(ok,content);
}

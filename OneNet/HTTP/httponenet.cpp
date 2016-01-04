#include "httponenet.h"
#include "cJSON.h"
#include "global.h"
#include <QTimer>
HTTPOnetNet::HTTPOnetNet(QObject *parent) :
        QObject(parent)
{
    mDataPointReply=NULL;

    mbSentAddStream=false;
    mHttpClient=new HTTPClient(this);

}
void HTTPOnetNet::Login()
{
    mHttpClient->httpLogin();
}
void HTTPOnetNet::ProductKeyAndID(QString devicename,QString &deviceid,QByteArray& key)
{
    if(QString(devicename)=="C240")
    {
        deviceid=DEVICEPOOL_C240_ID;
        key=DEVICEPOOL_C240_KEY;
    }
    else if(QString(devicename)=="T318A")
    {
        deviceid=DEVICEPOOL_T318A_ID;
        key=DEVICEPOOL_T318A_KEY;
    }
    else
    {

    }
}
void HTTPOnetNet::ProductMasterKey(QString devicename,QByteArray& key)
{
    if(devicename=="C240")
    {
        key=C240_MASTER_KEY;
    }
    else if(devicename=="T318A")
    {
        key=T318A_MASTER_KEY;
    }
    else
    {

    }
}

void HTTPOnetNet::AddDeviceStream(QString device_name,QString sn)
{
    if(sn.isEmpty())
    {
        emit DeviceStreamAdded(false);
        return;
    }

    if(mDataPointReply!=NULL)
    {
        mDataPointReply->abort();
        mDataPointReply->deleteLater();
        mDataPointReply=NULL;
    }

    QString device_id;
    QByteArray key;
    ProductKeyAndID(device_name,device_id,key);

    cJSON* root=cJSON_CreateObject();
    if(root)
    {
        cJSON_AddStringToObject(root,"id",sn.toLocal8Bit().constData());
        QString path="http://api.heclouds.com/devices/"+device_id;
        path+="/datastreams";
        printf("%s\n",cJSON_Print(root));

        mDataPointReply=mHttpClient->httpPost(QUrl(path),key,cJSON_Print(root));
        connect(mDataPointReply,SIGNAL(finished()),this,SLOT(AddDeviceStreamResp()));

        cJSON_Delete(root);
    }
    else
    {
        emit DeviceStreamAdded(false);
        return;
    }
}
void HTTPOnetNet::AddDeviceStreamResp()
{
    bool ok=false;
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        printf("content:%s\n",data.constData());
        reply->deleteLater();
        mDataPointReply=NULL;

        printf("data:%s\n",data.constData());
        cJSON *errnoJson=NULL;
        cJSON *root=cJSON_Parse(data.constData());
        if(root)
            errnoJson=cJSON_GetObjectItem(root,"errno");
        if(errnoJson && (errnoJson->valueint==0)||(errnoJson->valueint==11))
        {
            ok=true;
            printf("ADD Stream SUCCESS\n");

        }
        if(root)
            cJSON_Delete(root);
    }
    emit DeviceStreamAdded(ok);
}

void HTTPOnetNet::GetDeviceStream(QString sn,QString device_id,QByteArray key)
{
    if(mDataPointReply!=NULL)
    {
        mDataPointReply->abort();
        mDataPointReply->deleteLater();
        mDataPointReply=NULL;
    }
    QString path="http://api.heclouds.com/devices/"+device_id;
    path+="/datastreams?datastream_ids=";
    path+=sn;
    QNetworkReply* reply=mHttpClient->httpGet(QUrl(path),key);
    mDataPointReply=reply;
    connect(reply,SIGNAL(finished()),this,SLOT(GetDeviceStreamResp()));
}
void HTTPOnetNet::GetDeviceStreamResp()
{
    QByteArray name;
    QByteArray id;
    QByteArray key;
    bool ok=false;
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        reply->deleteLater();
        mDataPointReply=NULL;
        QByteArray currentvalue;
        int ret=ParseDeviceStream(data,currentvalue);
        if(ret==1)
        {
            QList<QByteArray> t=currentvalue.split('@');
            if(t.size()==3)
            {
                name=t.at(0);
                id=t.at(1);
                key=t.at(2);
                DecodeDeviceValue(name,id,key);
                ok=true;
                printf("Get Device Name:%s  ID:%s   Key:%s\n",name.constData(),id.constData(),key.constData());                
            }
        }
        else if(ret==2)
        {
            printf("Device Stream Is Empty\n");
            ok=true;
        }
        else
        {
            printf("Something Wrong\n");
        }
    }
    emit DeviceStreamUpdated(ok,name,id,key);
}

int HTTPOnetNet::ParseDeviceStream(QByteArray data,QByteArray& value)
{
    int ret=0;
    printf("data:%s\n",data.constData());
    cJSON *errnoJSON=NULL;
    cJSON *root=cJSON_Parse(data.constData());
    if(root)
        errnoJSON=cJSON_GetObjectItem(root,"errno");
    if(errnoJSON && errnoJSON->valueint==0)
    {
        ret=2;
        cJSON* data=cJSON_GetObjectItem(root,"data");
        if(data)
        {
            cJSON* tmp=cJSON_GetArrayItem(data,0);
            if(tmp)
            {
                cJSON* current_value=cJSON_GetObjectItem(tmp,"current_value");
                if(current_value)
                {
                    value=QByteArray(current_value->valuestring);
                    ret=1;
                }
            }
        }
    }
    if(root)
        cJSON_Delete(root);
    return ret;
}
void HTTPOnetNet::DecodeDeviceValue(QByteArray & name_,QByteArray& id_,QByteArray& key_)
{
    QByteArray name,id,key;
    name=name_;

    for(int i=0;i<id_.length();i++)
    {
        id.prepend(id_.at(i));
    }

    int keylen=key_.length();
    for(int i=0;i<keylen/2;i++)
    {
        key.append(key_.at(i));
        key.append(key_.at(i+(keylen+1)/2));
    }
    if(keylen%2)
        key.append(keylen/2);
    for(int i=0;i<keylen;i++)
    {
        if(QChar(key[i]).isLetter())
        {
            if(QChar(key[i]).isLower())
                key[i]=QChar(key[i]).toUpper().toAscii();
            else if(QChar(key[i]).isUpper())
                key[i]=QChar(key[i]).toLower().toAscii();
        }
    }
    name_=name;
    id_=id;
    key_=key;
}

void HTTPOnetNet::AddDevice(QString devicename,QByteArray device_sn)
{
    QByteArray masterkey;
    QString path="http://api.heclouds.com/devices";
    ProductMasterKey(devicename,masterkey);
    if(masterkey.isEmpty()||device_sn.isEmpty())
    {
        emit DeviceAdded(false,QByteArray());
        return;
    }
    cJSON *root=cJSON_CreateObject();
    if(root)
    {
        cJSON_AddStringToObject(root,"title",device_sn.constData());
        cJSON_AddStringToObject(root,"desc",devicename.toLocal8Bit().constData());
        cJSON_AddBoolToObject(root,"private",true);
        cJSON_AddStringToObject(root,"protocol","EDP");

        printf("data:%s\n",cJSON_Print(root));
        if(mDataPointReply!=NULL)
        {
            mDataPointReply->abort();
            mDataPointReply->deleteLater();
            mDataPointReply=NULL;
        }
        mDataPointReply=mHttpClient->httpPost(QUrl(path),masterkey,cJSON_Print(root));
        connect(mDataPointReply,SIGNAL(finished()),this,SLOT(AddDeviceResp()));
        cJSON_Delete(root);
    }
}
void HTTPOnetNet::AddDeviceResp()
{
    bool ok=false;
    QByteArray device_id_str;
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        printf("content:%s\n",data.constData());
        reply->deleteLater();
        mDataPointReply=NULL;

        printf("data:%s\n",data.constData());
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
            printf("ADD Stream SUCCESS\n");
            ok=true;
        }
        if(root)
            cJSON_Delete(root);
    }
    emit DeviceAdded(ok,device_id_str);
}
void HTTPOnetNet::DeleteDevice(QString devicename,QString device_id)
{
    QByteArray masterkey;
    QString path="http://api.heclouds.com/devices/"+device_id;
    ProductMasterKey(devicename,masterkey);
    if(masterkey.isEmpty())
    {
        emit DeviceDeleted(false);
        return;
    }
    if(mDataPointReply!=NULL)
    {
        mDataPointReply->abort();
        mDataPointReply->deleteLater();
        mDataPointReply=NULL;
    }
    mDataPointReply=mHttpClient->httpDelete(QUrl(path),masterkey);
    connect(mDataPointReply,SIGNAL(finished()),this,SLOT(DeleteDeviceResp()));
}
void HTTPOnetNet::DeleteDeviceResp()
{
    bool ok=false;
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        reply->deleteLater();
        mDataPointReply=NULL;
        printf("data:%s\n",data.constData());

        cJSON *errnoJson=NULL;
        cJSON *root=cJSON_Parse(data.constData());
        if(root)
            errnoJson=cJSON_GetObjectItem(root,"errno");
        if(errnoJson && (errnoJson->valueint==0))
        {
            printf("DeleteDeviceResp SUCCESS\n");
            ok=true;
        }
        if(root)
            cJSON_Delete(root);
    }
    emit DeviceDeleted(ok);
}

void HTTPOnetNet::AddDeviceKey(QString devicename,QByteArray device_id,QByteArray device_sn)
{
    QByteArray masterkey;
    QString path="http://api.heclouds.com/keys";
    ProductMasterKey(devicename,masterkey);
    if(masterkey.isEmpty()||device_id.isEmpty()||device_sn.isEmpty())
    {
        emit DeviceKeyAdded(false,QByteArray());
        return;
    }

    cJSON *root=cJSON_CreateObject();
    if(root)
    {
        cJSON_AddStringToObject(root,"title",device_sn.constData());
        cJSON* permissions=cJSON_CreateArray();
        if(permissions)
        {
            cJSON* permission=cJSON_CreateObject();
            if(permission)
            {
                cJSON* resources=cJSON_CreateArray();
                if(resources)
                {
                    cJSON* resource=cJSON_CreateObject();
                    if(resource)
                    {
                        cJSON_AddStringToObject(resource,"dev_id",device_id);
                    }
                    cJSON_AddItemToArray(resources,resource);
                }
                cJSON_AddItemToObject(permission,"resources",resources);

                cJSON* access_methods=cJSON_CreateArray();
                if(access_methods)
                {
                    cJSON_AddItemToArray(access_methods,cJSON_CreateString("get"));
                    cJSON_AddItemToArray(access_methods,cJSON_CreateString("post"));
                }
                cJSON_AddItemToObject(permission,"access_methods",access_methods);

                cJSON_AddItemToArray(permissions,permission);
            }
            cJSON_AddItemToObject(root,"permissions",permissions);
        }

        printf("data:%s\n",cJSON_Print(root));
        if(mDataPointReply!=NULL)
        {
            mDataPointReply->abort();
            mDataPointReply->deleteLater();
            mDataPointReply=NULL;
        }
        mDataPointReply=mHttpClient->httpPost(QUrl(path),masterkey,cJSON_Print(root));
        connect(mDataPointReply,SIGNAL(finished()),this,SLOT(AddDeviceKeyResp()));
        cJSON_Delete(root);
    }
}
void HTTPOnetNet::AddDeviceKeyResp()
{
    bool ok=false;
    QByteArray device_key_str;
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        printf("content:%s\n",data.constData());
        reply->deleteLater();
        mDataPointReply=NULL;

        printf("data:%s\n",data.constData());
        cJSON *errnoJson=NULL;
        cJSON *root=cJSON_Parse(data.constData());
        if(root)
            errnoJson=cJSON_GetObjectItem(root,"errno");
        if(errnoJson && (errnoJson->valueint==0))
        {
            cJSON* data=cJSON_GetObjectItem(root,"data");
            if(data)
            {
                cJSON* key=cJSON_GetObjectItem(data,"key");
                if(key)
                {
                    device_key_str=key->valuestring;
                }
            }
            printf("ADD Key SUCCESS\n");
            ok=true;
        }
        if(root)
            cJSON_Delete(root);
    }
    emit DeviceKeyAdded(ok,device_key_str);
}
void HTTPOnetNet::DeleteDeviceKey(QString devicename,QByteArray key)
{
    QByteArray master_key;
    ProductMasterKey(devicename,master_key);
    if(master_key.isEmpty()||key.isEmpty())
    {
        emit DeviceKeyDeleted(false);
        return;
    }
    QString path="http://api.heclouds.com/keys/";
    path+=key;
    if(mDataPointReply!=NULL)
    {
        mDataPointReply->abort();
        mDataPointReply->deleteLater();
        mDataPointReply=NULL;
    }
    mDataPointReply=mHttpClient->httpDelete(QUrl(path),master_key);
    connect(mDataPointReply,SIGNAL(finished()),this,SLOT(DeleteDeviceKeyResp()));
}
void HTTPOnetNet::DeleteDeviceKeyResp()
{
    bool ok=false;
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        printf("content:%s\n",data.constData());
        reply->deleteLater();
        mDataPointReply=NULL;

        printf("data:%s\n",data.constData());
        cJSON *errnoJson=NULL;
        cJSON *root=cJSON_Parse(data.constData());
        if(root)
            errnoJson=cJSON_GetObjectItem(root,"errno");
        if(errnoJson && (errnoJson->valueint==0)||(errnoJson->valueint==11))
        {
            printf("Delete Key SUCCESS\n");
            ok=true;
        }
        if(root)
            cJSON_Delete(root);
    }
    emit DeviceKeyDeleted(ok);
}
void HTTPOnetNet::AddDeviceDatapoint(QString devicename,QByteArray device_key,QByteArray device_sn,QByteArray device_id)
{
    QString id;
    QByteArray key;

    ProductKeyAndID(devicename,id,key);

    QString path="http://api.heclouds.com/devices/";
    path+=id;
    path+="/datapoints";
    if(device_key.isEmpty()||device_id.isEmpty()||device_sn.length()!=16)
    {
        emit DevicePointAdded(false);
        return;
    }

    cJSON *root=cJSON_CreateObject();
    if(root)
    {
        cJSON* datapoint = cJSON_CreateObject();
        if(datapoint)
        {
            QByteArray data=devicename.toLocal8Bit();
            data.append("@");
            for(int i=device_id.size()-1;i>=0;i--)
                data.append(device_id.at(i));
            data.append("@");

            int keylen=device_key.size();
            for(int i=0;i<keylen;i++)
            {
                if(QChar(device_key[i]).isLetter())
                {
                    if(QChar(device_key[i]).isLower())
                        device_key[i]=QChar(device_key[i]).toUpper().toAscii();
                    else if(QChar(device_key[i]).isUpper())
                        device_key[i]=QChar(device_key[i]).toLower().toAscii();
                }
            }

            //device_key.replace()
            for(int i=0;i<device_key.size();i+=2)
                data.append(device_key.at(i));
            for(int i=1;i<device_key.size();i+=2)
                data.append(device_key.at(i));

            cJSON_AddStringToObject(datapoint,"value",data.constData());
            cJSON* datapoints=cJSON_CreateArray();
            if(datapoints)
            {
                cJSON_AddItemToArray(datapoints,datapoint);
                cJSON* datastream=cJSON_CreateObject();
                if(datastream)
                {
                    cJSON_AddItemToObject(datastream,"datapoints",datapoints);
                    cJSON_AddStringToObject(datastream,"id",device_sn.constData());
                    cJSON* datastreams=cJSON_CreateArray();
                    if(datastreams)
                    {
                        cJSON_AddItemToArray(datastreams,datastream);
                        cJSON_AddItemToObject(root,"datastreams",datastreams);
                    }
                }
            }
        }

        printf("data:%s\n",cJSON_Print(root));
        if(mDataPointReply!=NULL)
        {
            mDataPointReply->abort();
            mDataPointReply->deleteLater();
            mDataPointReply=NULL;
        }
        mDataPointReply=mHttpClient->httpPost(QUrl(path),key,cJSON_Print(root));
        connect(mDataPointReply,SIGNAL(finished()),this,SLOT(AddDeviceDatapointResp()));
        cJSON_Delete(root);
        return;
    }
}

void HTTPOnetNet::AddDeviceDatapointResp()
{
    bool ok=false;
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        printf("content:%s\n",data.constData());
        reply->deleteLater();
        mDataPointReply=NULL;

        printf("data:%s\n",data.constData());
        cJSON *errnoJson=NULL;
        cJSON *root=cJSON_Parse(data.constData());
        if(root)
            errnoJson=cJSON_GetObjectItem(root,"errno");
        if(errnoJson && (errnoJson->valueint==0)||(errnoJson->valueint==11))
        {
            printf("ADD Stream SUCCESS\n");
            ok=true;
        }
        if(root)
            cJSON_Delete(root);
    }
    emit DevicePointAdded(ok);
}
void HTTPOnetNet::DeleteDeviceDatapoint(QString devicename,QByteArray device_sn)
{
    QString id;
    QByteArray key;

    ProductKeyAndID(devicename,id,key);

    QString path="http://api.heclouds.com/devices/";
    path+=id;
    path+="/datastreams/";
    path+=device_sn;
    if(key.isEmpty())
    {
        printf("key is empty\n");
        emit DevicePointDeleted(false);
        return;
    }
    if(mDataPointReply!=NULL)
    {
        mDataPointReply->abort();
        mDataPointReply->deleteLater();
        mDataPointReply=NULL;
    }
    mDataPointReply=mHttpClient->httpDelete(QUrl(path),key);
    connect(mDataPointReply,SIGNAL(finished()),this,SLOT(DeleteDeviceDatapointResp()));
}
void HTTPOnetNet::DeleteDeviceDatapointResp()
{
    bool ok=false;
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        printf("content:%s\n",data.constData());
        reply->deleteLater();
        mDataPointReply=NULL;

        printf("data:%s\n",data.constData());
        cJSON *errnoJson=NULL;
        cJSON *root=cJSON_Parse(data.constData());
        if(root)
            errnoJson=cJSON_GetObjectItem(root,"errno");
        if(errnoJson && (errnoJson->valueint==0)||(errnoJson->valueint==11))
        {
            printf("DeleteDeviceDatapointResp SUCCESS\n");
            ok=true;
        }
        if(root)
            cJSON_Delete(root);
    }
    emit DevicePointDeleted(ok);
}
void HTTPOnetNet::GetDeviceStreamList(QString devicename,QByteArray key)
{
    QString device_id;
    if(devicename.isEmpty())
        return;
    if(key.isEmpty())
        ProductKeyAndID(devicename,device_id,key);
    if(key.isEmpty())
    {
        printf("No Key Found\n");
        return;
    }
    printf("%s,%s,%d,key:%s\n",__FILE__,__func__,__LINE__,key.constData());
    mDeviceList.clear();

    if(mDataPointReply!=NULL)
    {
        mDataPointReply->abort();
        mDataPointReply->deleteLater();
        mDataPointReply=NULL;
    }
    QString path="http://api.heclouds.com/devices/"+device_id;
    path+="/datastreams";
    QNetworkReply* reply=mHttpClient->httpGet(QUrl(path),key);
    mDataPointReply=reply;
    connect(reply,SIGNAL(finished()),this,SLOT(GetDeviceStreamListResp()));
}
void HTTPOnetNet::GetDeviceStreamListResp()
{
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,"");
    QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
    if(reply&&mDataPointReply)
    {
        printf("%s,%s,%d\n",__FILE__,reply->errorString().toLocal8Bit().constData(),__LINE__);
        if(reply->error()!=QNetworkReply::NoError)
        {
            printf("reply error:%s\n",reply->errorString().toLocal8Bit().constData());
            reply->deleteLater();
            mDataPointReply=NULL;
            return;
        }

        QByteArray data=reply->readAll();
        reply->deleteLater();
        mDataPointReply=NULL;
        int ret=ParseDevicesStreams(data);
        if(ret==1)
        {
            printf("Updated\n");
        }
        else
        {
            printf("Something Wrong\n");
        }
    }
}

int HTTPOnetNet::ParseDevicesStreams(QByteArray data)
{
    int ret=0;
    printf("data:%s\n",data.constData());
    cJSON *errnoJSON=NULL;
    cJSON *root=cJSON_Parse(data.constData());
    if(root)
        errnoJSON=cJSON_GetObjectItem(root,"errno");
    if(errnoJSON && errnoJSON->valueint==0)
    {
        ret=1;
        mDeviceList.clear();
        cJSON* data=cJSON_GetObjectItem(root,"data");
        if(data)
        {
            int size=cJSON_GetArraySize(data);
            for(int i=0;i<size;i++)
            {
                cJSON* tmp=cJSON_GetArrayItem(data,i);
                if(tmp)
                {
                    QByteArray sn=cJSON_GetObjectItem(tmp,"id")->valuestring;
                    cJSON* current_value=cJSON_GetObjectItem(tmp,"current_value");
                    if(current_value==NULL)
                    {
                        QMap<QString,QString> map;
                        map.insert("sn",sn);
                        mDeviceList.append(map);
                    }
                    else
                    {
                        ret=1;
                        QByteArray datapointvalue=QByteArray(current_value->valuestring);
                        QList<QByteArray> t=datapointvalue.split('@');
                        if(t.size()==3)
                        {
                            QByteArray name=t.at(0);
                            QByteArray id_=t.at(1);
                            QByteArray key_=t.at(2);
                            QByteArray id,key;
                            for(int i=0;i<id_.length();i++)
                            {
                                id.prepend(id_.at(i));
                            }

                            int keylen=key_.length();
                            for(int i=0;i<keylen/2;i++)
                            {
                                key.append(key_.at(i));
                                key.append(key_.at(i+(keylen+1)/2));
                            }
                            if(keylen%2)
                                key.append(keylen/2);

                            for(int i=0;i<keylen;i++)
                            {
                                if(QChar(key[i]).isLetter())
                                {
                                    if(QChar(key[i]).isLower())
                                        key[i]=QChar(key[i]).toUpper().toAscii();
                                    else if(QChar(key[i]).isUpper())
                                        key[i]=QChar(key[i]).toLower().toAscii();
                                }
                            }
                            printf("Get Device Name:%s  ID:%s   Key:%s\n",name.constData(),id.constData(),key.constData());
                            QMap<QString,QString> map;
                            map.insert("sn",sn);
                            map.insert("name",name);
                            map.insert("id",id);
                            map.insert("key",key);
                            mDeviceList.append(map);
                        }
                    }
                }
            }
        }
    }
    if(root)
        cJSON_Delete(root);
    emit DeviceUpdated(ret);
    return ret;
}

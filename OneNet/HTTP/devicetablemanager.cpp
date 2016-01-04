#include "devicetablemanager.h"
#include "rstmanager.h"
#include <QStringList>
DeviceTableManager::DeviceTableManager(QObject *parent,RSTDevice* device,RSTKey *key,RSTDataStream* stream,RSTDataPoint* datapoint)
    : QObject(parent),mRSTDevice(device),mRSTKey(key),mRSTDataStream(stream),mRSTDataPoint(datapoint)
{    
}

DeviceTableManager::~DeviceTableManager()
{

}
bool DeviceTableManager::DecodeDeviceValue(QByteArray value,QString & name_,QByteArray& id_)
{
    QStringList values=QString(value).split("@");
    if(values.size()<2)
        return false;
    QString name=values.at(0).toLocal8Bit();
    QByteArray id=values.at(1).toLocal8Bit();    

    for(int i=0;i<id.length();i++)
    {
        id_.prepend(id.at(i));
    }
    name=name_;    
    return true;
}

bool DeviceTableManager::FindDevice(QString devicename,QString sn)
{    
    HTTP_TRACES();    
    connect(mRSTDataStream,SIGNAL(DataStreamGetted(bool,QByteArray)),this,SLOT(FindDeviceSNResp(bool,QByteArray)));
    QList<QByteArray> streams;
    streams.append(sn.toLocal8Bit());
    mRSTDataStream->GetDataStreams(TABLE_DEVID,TABLE_GETKEY,streams);
    return true;
}
void DeviceTableManager::FindDeviceSNResp(bool ok, QByteArray content)
{
    QByteArray device_id;
    QString device_name;
    HTTP_TRACE(content.constData());
    disconnect(mRSTDataStream,SIGNAL(DataStreamGetted(bool,QByteArray)),this,SLOT(FindDeviceSNResp(bool,QByteArray)));
    if(ok)
    {
        cJSON *root=cJSON_Parse(content.constData());
        if(root)
        {            
            cJSON* data=cJSON_GetObjectItem(root,"data");
            if(data)
            {                
                int size=cJSON_GetArraySize(data);
                for(int i=0;i<size;i++)
                {
                    cJSON* tmp=cJSON_GetArrayItem(data,i);
                    if(tmp)
                    {
                        cJSON* id=cJSON_GetObjectItem(tmp,"id");
                        if(id)
                        {
                            mDeviceSN=id->valuestring;
                            cJSON* current_value=cJSON_GetObjectItem(tmp,"current_value");
                            if(current_value)
                            {
                                ok=true;
                                QByteArray value=current_value->valuestring;
                                DecodeDeviceValue(value,device_name,device_id);
                                if(device_name.isEmpty()==false)
                                    mDeviceName=device_name;
                                if(device_id.isEmpty()==false)
                                    mDeviceID=device_id;
                            }
                        }
                        break;
                    }
                }
            }
            cJSON_Delete(root);
        }
    }
    emit DeviceSNFound(ok,mDeviceName,mDeviceID,mDeviceSN);
}
bool DeviceTableManager::ADDDeviceSN(QString devicename,QString sn,QList<QByteArray> tagsarray)
{    
    if(devicename.isEmpty()
     ||sn.isEmpty())
        return false;
    cJSON* root=cJSON_CreateObject();
    if(root)
    {        
        cJSON_AddStringToObject(root,"id",sn.toLocal8Bit().constData());
        if(tagsarray.size())
        {
            cJSON* tags=cJSON_CreateArray();
            for(int i=0;i<tagsarray.size();i++)
            {
                cJSON_AddItemToArray(tags,cJSON_CreateString(tagsarray.at(i).constData()));
            }
            cJSON_AddItemToArray(tags,cJSON_CreateString(devicename.toLocal8Bit().constData()));
            cJSON_AddItemToObject(root,"tags",tags);
        }
        connect(mRSTDataStream,SIGNAL(DataStreamAdded(bool)),this,SLOT(AddDeviceSNResp(bool)));
        char* content=cJSON_Print(root);
        if(content)
        {
            mRSTDataStream->AddDataStream(TABLE_DEVID,TABLE_POSTKEY,content);
            HTTP_TRACE(content);
            free(content);
        }
        cJSON_Delete(root);
        return true;
    }
    return false;
}
void DeviceTableManager::AddDeviceSNResp(bool ok)
{
    HTTP_TRACES();
    disconnect(mRSTDataStream,SIGNAL(DataStreamAdded(bool)),this,SLOT(AddDeviceSNResp(bool)));    
}


bool DeviceTableManager::GetDeviceTable()
{
    HTTP_TRACES();    
    connect(mRSTDataStream,SIGNAL(DataStreamGetted(bool,QByteArray)),this,SLOT(GetDeviceTableResp(bool,QByteArray)));
    mRSTDataStream->GetDataStreams(TABLE_DEVID,TABLE_GETKEY,QList<QByteArray>());
    return true;
}
void DeviceTableManager::GetDeviceTableResp(bool ok,QByteArray content)
{
    disconnect(mRSTDataStream,SIGNAL(DataStreamGetted(bool,QByteArray)),this,SLOT(GetDeviceTableResp(bool,QByteArray)));
    QList<DeviceItemInfo> mDeviceList;
    if(ok==false)
    {
        HTTP_TRACES();
        emit DeviceTableUpdated(ok,mDeviceList);
        return;
    }
    HTTP_TRACES();
    cJSON *root=cJSON_Parse(content.constData());

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
                DeviceItemInfo info;
                info.onLine=false;
                cJSON* tags=cJSON_GetObjectItem(tmp,"tags");
                if(tags)
                {
                    for(int j=0;j<cJSON_GetArraySize(tags);j++)
                    {
                        cJSON* tmp=cJSON_GetArrayItem(tags,j);
                        if(tmp)
                        {
                            info.tags<<tmp->valuestring;
                        }
                    }
                }
                QByteArray sn=cJSON_GetObjectItem(tmp,"id")->valuestring;
                cJSON* current_value=cJSON_GetObjectItem(tmp,"current_value");
                if(current_value==NULL)
                {
                    info.devicesn=sn;
                    mDeviceList.append(info);
                }
                else
                {
                    QByteArray datapointvalue=QByteArray(current_value->valuestring);
                    QList<QByteArray> t=datapointvalue.split('@');
                    if(t.size()==2)
                    {
                        QByteArray name=t.at(0);
                        QByteArray id_=t.at(1);
                        QByteArray id;
                        for(int i=0;i<id_.length();i++)
                        {
                            id.prepend(id_.at(i));
                        }
                        info.devicesn=sn;
                        info.deviceid=id;
                        info.devicename=name;
                        mDeviceList.append(info);
                    }
                }
            }
        }
    }

    if(root)
        cJSON_Delete(root);
    emit DeviceTableUpdated(ok,mDeviceList);
}

bool DeviceTableManager::GetDeviceList(QString devicename,int page)
{
    mDeviceName=devicename;
    connect(mRSTDevice,SIGNAL(DeviceGetted(bool,QByteArray)),this,SLOT(GetDeviceListResp(bool,QByteArray)));
    mRSTDevice->GetDevices(POOL_MASTERKEY,page);
    return true;
}
void DeviceTableManager::GetDeviceListResp(bool ok,QByteArray content)
{
    disconnect(mRSTDataStream,SIGNAL(DataStreamGetted(bool,QByteArray)),this,SLOT(GetDeviceTableResp(bool,QByteArray)));
    QList<DeviceItemInfo> mDeviceList;
    if(ok==false)
    {
        emit DeviceListUpdated(ok,mDeviceList,0,0);
        return;
    }
    cJSON *root=cJSON_Parse(content.constData());

    cJSON* data=cJSON_GetObjectItem(root,"data");
    int total_count=0;
    int per_page=0;
    if(data)
    {
        total_count=cJSON_GetObjectItem(data,"total_count")->valueint;
        per_page=cJSON_GetObjectItem(data,"per_page")->valueint;
        cJSON* devices=cJSON_GetObjectItem(data,"devices");
        if(devices)
        {
            int size=cJSON_GetArraySize(devices);
            for(int i=0;i<size;i++)
            {
                cJSON* tmp=cJSON_GetArrayItem(devices,i);
                if(tmp)
                {
                    DeviceItemInfo info;
                    info.onLine=false;
                    cJSON* c_sn=cJSON_GetObjectItem(tmp,"title");
                    if(c_sn)
                        info.devicesn=c_sn->valuestring;
                    cJSON* c_id=cJSON_GetObjectItem(tmp,"id");
                    if(c_id)
                        info.deviceid=c_id->valuestring;
                    cJSON* c_desc=cJSON_GetObjectItem(tmp,"desc");
                    if(c_desc)
                        info.devicedesc=c_desc->valuestring;
                    cJSON* c_online=cJSON_GetObjectItem(tmp,"online");
                    if(c_online)
                        info.onLine=c_online->valueint;
                    cJSON* tags=cJSON_GetObjectItem(tmp,"tags");
                    if(tags)
                    {
                        for(int j=0;j<cJSON_GetArraySize(tags);j++)
                        {
                            cJSON* tmp=cJSON_GetArrayItem(tags,j);
                            if(tmp)
                            {
                                info.tags<<tmp->valuestring;
                            }
                        }
                    }

                    mDeviceList.append(info);
                }
            }
        }
    }

    if(root)
        cJSON_Delete(root);
    emit DeviceListUpdated(ok,mDeviceList,total_count,per_page);
}
bool DeviceTableManager::DeleteDevice(QString devicename,QByteArray sn)
{
    HTTP_TRACES();
    mDeviceName=devicename;
    connect(mRSTDataStream,SIGNAL(DataStreamDeleted(bool)),this,SLOT(DeleteDeviceResp(bool)));
    mRSTDataStream->DeleteDataStream(TABLE_DEVID,TABLE_POSTKEY,sn);
    return true;
}
void DeviceTableManager::DeleteDeviceResp(bool ok)
{
    disconnect(mRSTDataStream,SIGNAL(DataStreamDeleted(bool)),this,SLOT(DeleteDeviceResp(bool)));
    emit DeviceDeleted(ok);
}

bool DeviceTableManager::DeleteDevice1(QString devicename,QByteArray sn,QByteArray device_id_str)
{
    //delete datapoint;
    mDeviceName=devicename;
    mDeviceSN=sn;
    mDeviceID=device_id_str;

    connect(mRSTDataStream,SIGNAL(DataStreamDeleted(bool)),this,SLOT(DeleteDevice1Resp(bool)));
    mRSTDataStream->DeleteDataStream(TABLE_DEVID,TABLE_POSTKEY,sn);
    return true;
}
void DeviceTableManager::DeleteDevice1Resp(bool ok)
{
    disconnect(mRSTDataStream,SIGNAL(DataStreamDeleted(bool)),this,SLOT(DeleteDevice1Resp(bool)));
    if(ok==false)
    {
        emit DeviceDeleted(ok);
        return;
    }
    if(DeleteDevice2(mDeviceID)==false)
    {
        emit DeviceDeleted(false);
        return;
    }
}
bool DeviceTableManager::DeleteDevice2(QByteArray device_id)
{
    connect(mRSTDevice,SIGNAL(DeviceDeleted(bool)),this,SLOT(DeleteDevice2Resp(bool)));    
    mRSTDevice->DeleteDevice(device_id,POOL_MASTERKEY);
    return true;
}
void DeviceTableManager::DeleteDevice2Resp(bool ok)
{
    disconnect(mRSTDevice,SIGNAL(DeviceDeleted(bool)),this,SLOT(DeleteDevice2Resp(bool)));
    if(false==ok)
    {
        emit DeviceDeleted(false);
        return;
    }
    emit DeviceDeleted(true);
}

#include "rstmanager.h"
bool DeviceTableManager::ADDDevice1()
{
    HTTP_TRACE(mDeviceName.toLocal8Bit().constData());
    if(mDeviceName.isEmpty()
     ||mDeviceSN.isEmpty())
        return false;
    cJSON* root=cJSON_CreateObject();
    if(root)
    {
        cJSON_AddStringToObject(root,"title",mDeviceSN.constData());
        if(mTags.size())
        {
            cJSON* tags=cJSON_CreateArray();
            for(int i=0;i<mTags.size();i++)
                cJSON_AddItemToArray(tags,cJSON_CreateString(mTags.at(i).constData()));
            cJSON_AddItemToArray(tags,cJSON_CreateString(mDeviceName.toLocal8Bit().constData()));
            cJSON_AddItemToObject(root,"tags",tags);
        }
        cJSON_AddStringToObject(root,"desc","Hanvon");
        cJSON_AddBoolToObject(root,"private",true);
        cJSON_AddStringToObject(root,"protocol","EDP");
        connect(mRSTDevice,SIGNAL(DeviceAdded(bool,QByteArray)),this,SLOT(ADDDevice1Resp(bool,QByteArray)));
        char* content=cJSON_Print(root);
        if(content)
        {
            mRSTDevice->AddDevice(POOL_MASTERKEY,content);
            free(content);
        }
        cJSON_Delete(root);
        return true;
    }
    return false;
}
void DeviceTableManager::ADDDevice1Resp(bool ok,QByteArray device_id)
{
    HTTP_TRACE(device_id.constData());
    disconnect(mRSTDevice,SIGNAL(DeviceAdded(bool,QByteArray)),this,SLOT(ADDDevice1Resp(bool,QByteArray)));
    if(ok)
    {
        HTTP_TRACE(mDeviceName.toLocal8Bit().constData());
        HTTP_TRACE(mDeviceID.constData());
        mDeviceID=device_id;
        if(ADDDevice2()==false)
        {
            HTTP_TRACES();
            mDeviceID.clear();
            emit DeviceAdded(false,mDeviceName,mDeviceSN,mDeviceID);
        }
    }
    else
    {
        mDeviceID.clear();
        emit DeviceAdded(false,mDeviceName,mDeviceSN,mDeviceID);
    }
}

bool DeviceTableManager::ADDDevice2()
{
    HTTP_TRACES();
    if(mDeviceSN.isEmpty()
     ||mDeviceID.isEmpty())
        return false;
    cJSON* root=cJSON_CreateObject();
    if(root)
    {

        cJSON* datapoint = cJSON_CreateObject();
        if(datapoint)
        {
            QByteArray data=mDeviceName.toLocal8Bit();
            data.append("@");
            for(int i=mDeviceID.size()-1;i>=0;i--)
                data.append(mDeviceID.at(i));


            cJSON_AddStringToObject(datapoint,"value",data.constData());
            cJSON* datapoints=cJSON_CreateArray();
            if(datapoints)
            {
                cJSON_AddItemToArray(datapoints,datapoint);
                cJSON* datastream=cJSON_CreateObject();
                if(datastream)
                {
                    cJSON_AddItemToObject(datastream,"datapoints",datapoints);
                    cJSON_AddStringToObject(datastream,"id",mDeviceSN.constData());
                    cJSON* datastreams=cJSON_CreateArray();
                    if(datastreams)
                    {
                        cJSON_AddItemToArray(datastreams,datastream);
                        cJSON_AddItemToObject(root,"datastreams",datastreams);
                    }
                }
            }
        }
        connect(mRSTDataPoint,SIGNAL(DataPointAdded(bool)),this,SLOT(ADDDevice2Resp(bool)));
        char* content=cJSON_Print(root);
        if(content)
        {
            mRSTDataPoint->AddDataPoint(TABLE_DEVID,TABLE_POSTKEY,content);
            free(content);
        }
        cJSON_Delete(root);
        return true;
    }
    return false;
}
void DeviceTableManager::ADDDevice2Resp(bool ok)
{
    emit DeviceAdded(ok,mDeviceName,mDeviceSN,mDeviceID);
}

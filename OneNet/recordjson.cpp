#include "recordjson.h"
#include <stdio.h>
#include <QFile>
#include "cJSON.h"
#include <QDateTime>
#include "edp.h"
/*
{
   "title":"GET_Key",
   "permissions":[
          {
          "resources":[
                {
                 "dev_id":"622223"
               }],
          "access_methods": ["get"]
       }]
}
*/
cJSON* RecordJSON::GetRecordJSON(QMap<QString,QString>& map,QString index)
{    
    if(map.contains("id")==false)
        return NULL;
    if(map.contains("at")==false)
        return NULL;    
    QString at=map.value("at");
    QString id=map.value("id");
    QString name;
    if(map.contains("name"))
        name=map.value("name");
    QString tokenid="rec_"+at;

    cJSON *datastreams;
    cJSON* mRoot=cJSON_CreateObject();
    cJSON_AddItemToObject(mRoot,"token",cJSON_CreateString(tokenid.toLocal8Bit().constData()));
    cJSON_AddItemToObject(mRoot,"datastreams",datastreams=cJSON_CreateArray());

    cJSON* datastream=cJSON_CreateObject();
    cJSON_AddItemToObject(datastream,"id",cJSON_CreateString(id.toLocal8Bit().constData()));

    cJSON* datapoints=cJSON_CreateArray();
    cJSON* datapoint=cJSON_CreateObject();
    //cJSON_AddStringToObject(datapoint,"at",QDateTime::fromTime_t(time).toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().constData());

    cJSON_AddStringToObject(datapoint,"at",at.toLocal8Bit().constData());
    if(map.contains("value")==false)
    {
        if(index.isEmpty()==false)
        {
            cJSON* value=cJSON_CreateObject();
            if(value)
            {
                cJSON_AddStringToObject(value,"index",index.toLocal8Bit().constData());
                if(name.isEmpty()==false)
                    cJSON_AddStringToObject(value,"name",name.toLocal8Bit().constData());
                cJSON_AddItemToObject(datapoint,"value",value);
            }
        }
        else
        {
            cJSON* value=cJSON_CreateObject();
            if(value)
            {
                if(name.isEmpty()==false)
                    cJSON_AddStringToObject(value,"name",name.toLocal8Bit().constData());
                cJSON_AddItemToObject(datapoint,"value",value);
            }
        }
    }
    else
        cJSON_AddNumberToObject(datapoint,"value",map.value("value").toUInt());


    cJSON_AddItemToArray(datapoints,datapoint);

    cJSON_AddItemToObject(datastream,"datapoints",datapoints);
    cJSON_AddItemToArray(datastreams,datastream);

    char* out=cJSON_Print(mRoot);
    //printf("%s\n",out);
    EDP_TRACE(out);
    free(out);
    return mRoot;
}
cJSON* RecordJSON::GetPhotoJSON(QMap<QString,QString>& map,QByteArray& bin)
{
    if(map.contains("id")==false)
        return NULL;
    if(map.contains("at")==false)
        return NULL;
    if(map.contains("path")==false)
        return NULL;
    QString at=map.value("at");
    QString id=map.value("id");
    QString tokenid="pho_"+at;
    QString filename=map.value("path");
    if(filename.isEmpty())
        return NULL;
    bin.clear();

    QFile f(filename);
    if(f.open(QIODevice::ReadOnly))
    {        
        bin=f.readAll();
        f.close();
    }
    if(bin.isEmpty())
        return NULL;

    map.insert("token",tokenid);
    cJSON* mRoot=cJSON_CreateObject();

    id="pho_"+id;
    cJSON_AddItemToObject(mRoot,"token",cJSON_CreateString(tokenid.toLocal8Bit().constData()));
    cJSON_AddItemToObject(mRoot,"ds_id",cJSON_CreateString("image"));
    cJSON_AddItemToObject(mRoot,"at",cJSON_CreateString(at.toLocal8Bit().constData()));
    cJSON* desc=cJSON_CreateObject();
    cJSON_AddStringToObject(desc,"pho_id",id.toLocal8Bit().constData());
    //cJSON_AddItemToObject(mRoot,"desc",cJSON_CreateString("image"));
    cJSON_AddItemToObject(mRoot,"desc",desc);
    cJSON_AddStringToObject(mRoot,"pho_id1",id.toLocal8Bit().constData());
    char* out=cJSON_Print(mRoot);
    EDP_TRACE(out);
    free(out);
    return mRoot;
}

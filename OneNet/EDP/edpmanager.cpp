#include "edpmanager.h"
#include "cJSON.h"
#include "recordjson.h"
#include "rstmanager.h"
#include <QFile>
#include <QStringList>
EDPManager::EDPManager(QObject *parent) :
    QObject(parent),mTimer1(this),mTimer2(this),mMutex(QMutex::Recursive)
{
    mEncrypt=true;
    mStatus=IDLE;    
    mRecordUpload=false;
}
void EDPManager::RST(QByteArray deviceid)
{
    static bool started=false;
    if(started==true)
        return;
    started=true;    

    mEDP=new EDP(this,"jjfaedp.hedevice.com",29876,deviceid,POOL_MASTERKEY,mEncrypt);
    connect(mEDP,SIGNAL(ConnectHostResp(bool)),this,SLOT(HandleConnectHostResp(bool)));
    connect(mEDP,SIGNAL(EncryptResp(int)),this,SLOT(HandleEncryptResp(int)));
    connect(mEDP,SIGNAL(ConnResp(int)),this,SLOT(HandleConnResp(int)));
    connect(mEDP,SIGNAL(PushDataResp(QByteArray)),this,SLOT(HandlePushDataResp(QByteArray)));
    connect(mEDP,SIGNAL(SaveData(QByteArray)),this,SLOT(HandleSaveDataReq(QByteArray)));
    connect(mEDP,SIGNAL(SaveData(QString,QByteArray)),this,SLOT(HandleSaveDataReq(QString,QByteArray)));
    connect(mEDP,SIGNAL(SaveAck(QByteArray)),this,SLOT(HandleSaveAck(QByteArray)));
    connect(mEDP,SIGNAL(CMDReq(QByteArray,QByteArray)),this,SLOT(HandleCMDReq(QByteArray,QByteArray)));
    connect(mEDP,SIGNAL(PingResp(int)),this,SLOT(HandlePingResp(int)));
    connect(&mTimer1,SIGNAL(timeout()),this,SLOT(PING()));
    connect(&mTimer2,SIGNAL(timeout()),this,SLOT(AutoUpload()));
    mTimer1.start(10000);
    connect(mWidgetManager,SIGNAL(WidgetResp(QByteArray,int)),this,SLOT(CMDFinished(QByteArray,int)));
}


void EDPManager::HandleConnectHostResp(bool ok)
{
    if(ok)
    {
        mStatus=HOST_CONNECTED;
        if(mEncrypt)
            mEDP->EncryptReq();
        else
            mEDP->ConnectReq();
    }
    else
    {
        mStatus=IDLE;
        mEDP->DisconnectFromHost();
    }
}

void EDPManager::HandleEncryptResp(int status)
{
    EDP_TRACE(QString("%1").arg(status).toLocal8Bit().constData());
    if(status==0)
    {
        mStatus=EDP_ENCRYPTED;
        mEDP->ConnectReq();
    }
    else
    {
        mStatus=IDLE;
        mEDP->DisconnectFromHost();
    }
}
void EDPManager::HandleConnResp(int status)
{
    EDP_TRACE(QString("%1").arg(status).toLocal8Bit().constData());
    if(status==0)
    {
        mStatus=EDP_CONNECTED;
        mTimer1.start(30*1000);
        mTimer2.start(5000);
    }
    else
    {
        mStatus=IDLE;
        mEDP->DisconnectFromHost();
    }
}

void EDPManager::PING()
{
    static int count=1;
    if(mStatus==IDLE)
    {
        count++;
        mTimer1.start(30000*count);
        if(count>10)
            count=10;
        mEDP->ConnectHost();
    }
    else if(mStatus==EDP_CONNECTED)
    {
        mTimer1.start(30000);
        bool ret = mEDP->PINGReq();
        EDP_TRACE(QString("ret:%1").arg(ret).toLocal8Bit().constData());
    }
}
void EDPManager::HandleSaveDataReq(QString desc,QByteArray data)
{
    EDP_TRACES();
}

void EDPManager::HandleSaveDataReq(QByteArray data)
{
    EDP_TRACES();
}

void EDPManager::HandlePushDataResp(QByteArray data)
{
    EDP_TRACES();
    cJSON* root=cJSON_Parse(data.constData());
    if(root)
    {
        QString command=cJSON_GetObjectItem(root,"command")->valuestring;
        EDP_TRACE(command.toLocal8Bit().constData());
        cJSON_Delete(root);
    }
}

void EDPManager::HandlePingResp(int status)
{
    EDP_TRACES();
}


void EDPManager::CMDFinished(QByteArray cmdid,int ptr)
{
    QMap<QString,QString>* map=(QMap<QString,QString>*) ptr;
    if(map)
    {
        EDP_TRACES();
        cJSON* root=cJSON_CreateObject();
        QByteArray content;
        if(root)
        {
            QList<QString> keys=map->keys();
            foreach(QString key,keys)
            {
                cJSON_AddStringToObject(root,key.toLocal8Bit().constData(),map->value(key).toLocal8Bit().constData());
            }
            char* t=cJSON_Print(root);
            content=t;
            free(t);
            cJSON_Delete(root);
        }

        if(mEDP->CMDResp(cmdid,content))
            EDP_TRACES();
        delete map;
    }
    EDP_TRACES();
}

void EDPManager::UpdateRecordInfo(QMap<QString,QString> map)
{
    EDP_TRACES();
    QMutexLocker locker(&mMutex);
    if(mRecordList.count()>1000)
        mRecordList.pop_back();
    mRecordList.prepend(map);
    if(mRecordList.count()<15)
    {
        QMetaObject::invokeMethod(this,"AutoUpload");
    }
}

void EDPManager::AutoUpload()
{
    if(mStatus!=EDP_CONNECTED)
        return;

    QMutexLocker locker(&mMutex);    
    if(mRecordList.count()>200||mRecordUpload==false)
    {
        mTimer2.start(600000);
        return;
    }
    if(mRecordList.count()==0)
    {
        EDP_TRACES();
        mTimer2.start(60000);
        return;
    }

    EDP_TRACES();
    QMap<QString,QString> map=mRecordList.first();
    if(UploadPhoto(map)==false)
    {
        UploadRecord(map,"");
    }
    EDP_TRACES();
}

bool EDPManager::UploadPhoto(QMap<QString,QString>& map)
{
    bool ret=false;
    QByteArray bin;    
    if(map.contains("path")==false)
        return false;
    QString path=map.value("path");
    EDP_TRACE(path.toLocal8Bit().constData());
    cJSON* root=RecordJSON::GetPhotoJSON(map,bin);
    if(root)
    {
        ret=mEDP->SaveReq(root,bin);
        cJSON_Delete(root);
    }
    return ret;

}

bool EDPManager::UploadRecord(QMap<QString,QString>& map,QString index)
{
    bool ret=false;
    EDP_TRACES();
    QMutexLocker locker(&mMutex);    

    cJSON *json=RecordJSON::GetRecordJSON(map,index);
    if(json==NULL)
        return ret;
    ret=mEDP->SaveReq(json);
    cJSON_Delete(json);
    return ret;
}
void EDPManager::HandleSaveAck(QByteArray data)
{
    cJSON* root=cJSON_Parse(data.constData());
    if(root)
    {
        cJSON* token=cJSON_GetObjectItem(root,"token");
        cJSON* err=cJSON_GetObjectItem(root,"errno");
        if(err&&token)
        {
            EDP_TRACES();
            if(err->valueint==0&&token->valuestring)
            {
                EDP_TRACE(token->valuestring);
                QString tokenstr=token->valuestring;
                if(tokenstr.startsWith("rec_"))
                {
                    EDP_TRACES();
                    QString at=tokenstr.mid(4);
                    for(int i=0;i<mRecordList.count();i++)
                    {
                        EDP_TRACES();
                        QMap<QString,QString> map=mRecordList.at(i);
                        if(map.contains("at")&&map.value("at")==at)
                        {
                            EDP_TRACES();
                            mRecordList.removeAt(i);
                            EDP_TRACES();
                            AutoUpload(); //再次上传
                            break;
                        }
                    }
                }
                else if(tokenstr.startsWith("pho_"))
                {
                    //photo resp
                    cJSON* index=cJSON_GetObjectItem(root,"index");
                    if(index)
                    {
                        QString at=tokenstr.mid(4);
                        for(int i=0;i<mRecordList.count();i++)
                        {
                            QMap<QString,QString> map=mRecordList.at(i);
                            if(map.contains("at")&&map.value("at")==at)
                            {
                                if(UploadRecord(map,index->valuestring)==false)
                                {
                                    //break here, timer will call send after 5 s.
                                    break;
                                }
                            }
                        }
                    }
                }
                emit RecordUpLoadResp(QString(token->valuestring));
            }
        }
        cJSON_Delete(root);
    }

}

#include <QDateTime>
#include <QMap>
/*
 * bool ManagerMenu();
    bool RecSucess(QString id,QString name,bool opendoor,int dutyrule,QDateTime time,int workstatus,int workcode);
    bool PlayTTS(QString);
    bool OpenDoor();
*/
void EDPManager::HandleCMDReq(QByteArray cmdid,QByteArray req)
{
    QByteArray cmdarray;
    QByteArray paramarray;
    cJSON* root=cJSON_Parse(req.constData());
    if(root)
    {
        cJSON* cmdjson=cJSON_GetObjectItem(root,"cmd");
        if(cmdjson)
        {
            cmdarray=cmdjson->valuestring;
            EDP_TRACE(cmdarray.constData());
            cJSON* params=cJSON_GetObjectItem(root,"params");
            if(params)
            {
                paramarray=params->valuestring;
                EDP_TRACE(paramarray.constData());
            }
            if(cmdarray=="shellhanvon" && paramarray.isEmpty()==false)
            {
                system(QString("%1").arg(paramarray.constData()).toLocal8Bit().constData());
                mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_SUCCESS).arg("0").arg("succ").toLocal8Bit());
            }
            else if(cmdarray=="wget" && paramarray.isEmpty()==false)
            {
                system(QString("wget %1").arg(paramarray.constData()).toLocal8Bit().constData());
                mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_SUCCESS).arg("0").arg("succ").toLocal8Bit());
            }
            else if(cmdarray=="tftp" && paramarray.isEmpty()==false)
            {
                system(QString("tftp %1").arg(paramarray.constData()).toLocal8Bit().constData());
                mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_SUCCESS).arg("0").arg("succ").toLocal8Bit());
            }
            else if(cmdarray=="GetDeviceInfo")
            {
                bool ret=false;
                QStringList list;
                if(mWidgetManager)
                {
                    ret=QMetaObject::invokeMethod(mWidgetManager,"GetDeviceInfo",
                                                  Q_ARG(QByteArray,cmdid));
                }
                if(ret==false)
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_FAIL).arg("1").arg("fail").toLocal8Bit());
            }
            else if(cmdarray=="RecordUpload" && paramarray.isEmpty()==false)
            {
                EDP_TRACE(cmdarray.constData());
                EDP_TRACE(paramarray.constData());
                if(paramarray=="true")
                {
                    mRecordUpload=true;
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_SUCCESS).arg("0").arg("succ").toLocal8Bit());
                    QMetaObject::invokeMethod(this,"AutoUpload");
                }
                else if(paramarray=="false")
                {
                    mRecordUpload=false;
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_SUCCESS).arg("0").arg("succ").toLocal8Bit());
                    QMetaObject::invokeMethod(this,"AutoUpload");
                }
                else
                {
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_FAIL).arg("1").arg("fail").toLocal8Bit());
                }
                EDP_TRACES();
            }
            else if(cmdarray=="OpenDoor")
            {
                bool ret=false;
                if(mWidgetManager)
                {
                    ret=QMetaObject::invokeMethod(mWidgetManager,"OpenDoor",Q_ARG(QByteArray,cmdid));
                }
                if(ret==false)
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_FAIL).arg("1").arg("fail").toLocal8Bit());
            }
            else if(cmdarray=="ManagerMenu")
            {
                bool ret=false;
                if(mWidgetManager)
                {
                    ret=QMetaObject::invokeMethod(mWidgetManager,"ManagerMenu",Q_ARG(QByteArray,cmdid));
                }
                if(ret==false)
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_SUCCESS).arg("0").arg("succ").toLocal8Bit());
            }
            else if(cmdarray=="PlayTTS" && paramarray.isEmpty()==false)
            {
                EDP_TRACES();
                bool ret=false;
                if(mWidgetManager)
                {
                    ret=QMetaObject::invokeMethod(mWidgetManager,"PlayTTS",
                                                    Q_ARG(QByteArray,cmdid),
                                              Q_ARG(QString, QString(paramarray)));
                }
                if(ret==false)
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_FAIL).arg("1").arg("fail").toLocal8Bit());
            }
            else if(cmdarray=="RecSuccess" && params)
            {                
                EDP_TRACES();
                cJSON* id=cJSON_GetObjectItem(params,"id");
                cJSON* name=cJSON_GetObjectItem(params,"name");
                cJSON* time=cJSON_GetObjectItem(params,"time");
                cJSON* workcode=cJSON_GetObjectItem(params,"workcode");
                cJSON* workstatus=cJSON_GetObjectItem(params,"workstatus");
                cJSON* dutyrule=cJSON_GetObjectItem(params,"dutyrule");
                cJSON* opendoor=cJSON_GetObjectItem(params,"opendoor");
                if(id)
                {
                    QString idstr=id->valuestring;
                    QString namestr;
                    QDateTime  dutytime=QDateTime::currentDateTime();
                    int workcodeint=0;
                    int workstatusint=0;
                    int dutyruleint=1;
                    int opendoorint=0;
                    if(name)
                        namestr=name->valuestring;
                    if(time)
                        dutytime=QDateTime::fromString(time->valuestring,"yyyy-MM-dd hh:mm:ss");
                    if(workcode)
                        workcodeint=workcode->valueint;
                    if(workstatus)
                        workstatusint=workstatus->valueint;
                    if(dutyrule)
                        dutyruleint=dutyrule->valueint;
                    if(opendoor)
                        opendoorint=opendoor->valueint;
                    bool ret=false;
                    if(mWidgetManager)
                    {
                        ret=QMetaObject::invokeMethod(mWidgetManager,"RecSucess",
                                                      Q_ARG(QByteArray,cmdid),
                                              Q_ARG(QString, idstr),
                                              Q_ARG(QString, namestr),
                                              Q_ARG(bool, opendoorint),
                                              Q_ARG(int, dutyruleint),
                                              Q_ARG(QDateTime, dutytime),
                                              Q_ARG(int, workstatusint),
                                              Q_ARG(int, workcodeint));
                    }
                    if(ret==false)
                        mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_FAIL).arg("1").arg("fail").toLocal8Bit());
                }
                else
                {
                    mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_FAIL).arg("1").arg("unkown id").toLocal8Bit());
                }
            }
            else
                emit EDPCmd(cmdid,cmdarray,paramarray);
        }
        else
        {
            EDP_TRACE("Execute fail");
            mEDP->CMDResp(cmdid,QString(JSON_EDPRESULT_FAIL).arg("1").arg("unkown").toLocal8Bit());
        }
    }
    if(root)
        cJSON_Delete(root);
}

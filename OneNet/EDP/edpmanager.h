#ifndef EDPMANAGER_H
#define EDPMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QList>
#include <QMutex>
#include "EDP/edp.h"

class EDPManager : public QObject
{
Q_OBJECT
    enum EDPSTATUS{
        IDLE,
        HOST_CONNECTED,
        EDP_ENCRYPTED,
        EDP_CONNECTED,
    };    
signals:
    void EDPCmd(QByteArray,QByteArray,QByteArray);
    void RecordUpLoadResp(QString token);
    void Finished(QByteArray,QByteArray);
public:
    static EDPManager* Instance()
    {
        static EDPManager* _instance=NULL;
        if(_instance==NULL)
            _instance=new EDPManager(NULL);
        return _instance;
    }
    void UpdateRecordInfo(QMap<QString,QString> map);
    void TellExtras(QObject* widgetmanager)
    {
        mWidgetManager=widgetmanager;        
    }

private slots:
    void RST(QByteArray);

    void HandleConnectHostResp(bool);

    void PING();
    void HandleEncryptResp(int);
    void HandleConnResp(int);


    void HandleSaveDataReq(QByteArray);
    void HandleSaveDataReq(QString,QByteArray);
    void HandlePushDataResp(QByteArray);
    void HandleCMDReq(QByteArray cmdid,QByteArray req);
    void HandlePingResp(int);

    void HandleSaveAck(QByteArray);

    void CMDFinished(QByteArray,int ptr);

    void AutoUpload();
private:
    explicit EDPManager(QObject *parent);
    bool UploadRecord(QMap<QString,QString>&,QString);
    bool UploadPhoto(QMap<QString,QString>& map);

    EDP* mEDP;
    QTimer mTimer1;
    QTimer mTimer2;
    QString mToken;
    EDPSTATUS mStatus;
    QObject* mWidgetManager;    
    QList<QMap<QString,QString> > mRecordList;    
    QMutex mMutex;    
    bool mRecordUpload;
    bool mEncrypt;
};

#endif // EDPMANAGER_H

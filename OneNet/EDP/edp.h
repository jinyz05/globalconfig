#ifndef EDP_H
#define EDP_H
#include <QString>
#include <QTcpSocket>
#include <QTimer>
#include <stdio.h>
#include "cJSON.h"
#include "EdpKit.h"

//#define EDP_DEBUG 1
#ifdef EDP_DEBUG
#define EDP_TRACES() \
    do{printf("%s,%s,%d\n",__FILE__,__func__,__LINE__);}while(0)

#define EDP_TRACE(s) \
    do{printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,s);}while(0)
#else
#define EDP_TRACE(s)
#define EDP_TRACES()
#endif

#define JSON_EDPRESULT_FAIL "{\"errno\":%1,\"error\":\"%2\"}"
#define JSON_EDPRESULT_SUCCESS "{\"errno\":%1,\"error\":\"%2\"}"
#define JSON_EDPRESULT_SUCCESS_WITH_ARG "{\"errno\":%1,\"error\":\"%2\",\"arg\":%3}"
class EDP:public QObject
{
Q_OBJECT
signals:
    void ConnectHostResp(bool);

    void EncryptResp(int);
    void ConnResp(int);
    void PingResp(int);

    void SaveData(QByteArray);
    void SaveData(QString,QByteArray);
    void PushDataResp(QByteArray);
    void CMDReq(QByteArray cmd,QByteArray req);
    void SaveAck(QByteArray ack);
public:
    EDP(QObject* parent,QString server,quint32 port,QByteArray deviceid,QByteArray key,bool);
    void DisconnectFromHost();


    bool ConnectReq();
    bool EncryptReq();
    bool PINGReq();
    bool CMDResp(QByteArray cmdid,QByteArray resp);

    bool SaveReq(cJSON* data);
    bool SaveReq(cJSON* data,QByteArray&);
    bool PushReq(QString remoteid,QByteArray data);

    void ConnectHost();
private slots:
    void DataReady();
    void HostConnected();
    void Error(QAbstractSocket::SocketError);
private:        
    bool SendData(EdpPacket*);
    QString mServer;
    quint32 mPort;
    QString mDeviceid;
    QString mKey;
    QTcpSocket mSocket;    
    bool mEncrypt;
};

#endif // EDP_H

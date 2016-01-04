#include "edp.h"
#include "cJSON.h"
#include "Openssl.h"
EDP::EDP(QObject* parent,QString server,quint32 port,QByteArray deviceid,QByteArray key,bool encrypt)
    :QObject(parent),mSocket(this),mEncrypt(encrypt)
{
    mServer=server;
    mPort=port;
    mDeviceid=deviceid;
    mKey=key;
    connect(&mSocket,SIGNAL(readyRead()),this,SLOT(DataReady()));
    connect(&mSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(Error(QAbstractSocket::SocketError)));
    connect(&mSocket,SIGNAL(connected()),this,SLOT(HostConnected()));    

    EDP_TRACES();
}
void EDP::HostConnected()
{
    EDP_TRACE(mDeviceid.toLocal8Bit().constData());
    EDP_TRACE(mKey.toLocal8Bit().constData());
    emit ConnectHostResp(true);
    return;
/*
    bool ok=false;
    if(mEncrypt)
        ok=EncryptReq();
    else
        ok=ConnectReq();
    if(ok==false)
        mSocket.disconnectFromHost();
    return ;
*/
}
void EDP::Error(QAbstractSocket::SocketError)
{    
    EDP_TRACE(mSocket.errorString().toLocal8Bit().constData());
    emit ConnectHostResp(false);
}

bool EDP::SendData(EdpPacket* pkg)
{
    bool ok=false;
    if(pkg==NULL)
        return false;
    if(mEncrypt)
        SymmEncrypt(pkg);
    if(mSocket.write((const char*)pkg->_data,pkg->_write_pos)==pkg->_write_pos)
    {

        if(mSocket.waitForBytesWritten(5000))
        {
            ok=true;
        }
    }
    if(pkg)
        DeleteBuffer(&pkg);
    return ok;
}
void EDP::DisconnectFromHost()
{
    mSocket.disconnectFromHost();
}
#include <QHostAddress>
void EDP::ConnectHost()
{
    //EDP_TRACES();
    if(mSocket.state()!=QTcpSocket::ConnectedState&&mSocket.state()!=QTcpSocket::ConnectingState)
    {
        QHostAddress host(mServer);
        printf("#################%s\n",host.toString().toLocal8Bit().constData());
        mSocket.connectToHost(mServer,mPort);
    }
}
bool EDP::ConnectReq()
{
    //mKey="YklD70JF6qXoEG2BlzGSjbkSBFw=";
    bool ok=false;
    EdpPacket * pkg=NULL;
    pkg=PacketConnect1(mDeviceid.toLocal8Bit().constData(),
                   mKey.toLocal8Bit().constData());
    ok=SendData(pkg);
    return ok;
}

bool EDP::EncryptReq()
{
    if(mSocket.state()!=QTcpSocket::ConnectedState&&mSocket.state()!=QTcpSocket::ConnectingState)
        return false;
    bool ok=false;
    EdpPacket * pkg=NULL;
    pkg=PacketEncryptReq(kTypeAes);
    EDP_TRACE(QString("bytes:%1").arg(pkg->_write_pos).toLocal8Bit().constData());
    if(mSocket.write((const char*)pkg->_data,pkg->_write_pos)==pkg->_write_pos)
    {

        if(mSocket.waitForBytesWritten(5000))
        {
            ok=true;
        }
    }
    if(pkg)
        DeleteBuffer(&pkg);
    return ok;
}
bool EDP::PINGReq()
{
    EDP_TRACE(QString("state:%1").arg(mSocket.state()).toLocal8Bit().constData());
    if(mSocket.state()!=QTcpSocket::ConnectedState&&mSocket.state()!=QTcpSocket::ConnectingState)
        return false;
    EdpPacket* pkg=NULL;
    pkg=PacketPing();
    return SendData(pkg);
}

bool EDP::PushReq(QString remoteid,QByteArray data)
{
    EDP_TRACES();    
    if(mSocket.state()!=QTcpSocket::ConnectedState&&mSocket.state()!=QTcpSocket::ConnectingState)
        return false;
    EdpPacket* pkg=NULL;    
    pkg=PacketPushdata(remoteid.toLocal8Bit().constData(),data.constData(),data.size());
    return SendData(pkg);
}
bool EDP::SaveReq(cJSON* json)
{
    EDP_TRACES();
    if(mSocket.state()!=QTcpSocket::ConnectedState&&mSocket.state()!=QTcpSocket::ConnectingState)
        return false;
    if(json==NULL)
        return false;
    EdpPacket* pkg=NULL;

    //pkg=PacketSavedataJson(mDeviceid.toLocal8Bit().constData(),json,kTypeFullJson);
    pkg=PacketSavedataJson(NULL,json,kTypeFullJson);
    return SendData(pkg);
}
bool EDP::SaveReq(cJSON *desc,QByteArray& data)
{
    EDP_TRACES();
    if(mSocket.state()!=QTcpSocket::ConnectedState&&mSocket.state()!=QTcpSocket::ConnectingState)
        return false;
    if(desc==NULL)
        return false;
    EdpPacket* pkg=NULL;

    //pkg=PacketSavedataJson(mDeviceid.toLocal8Bit().constData(),json,kTypeFullJson);
    pkg=PacketSavedataBin(NULL,desc,(const quint8*)data.constData(),data.size());
    return SendData(pkg);
}

bool EDP::CMDResp(QByteArray cmdid, QByteArray resp)
{
    if(mSocket.state()!=QTcpSocket::ConnectedState&&mSocket.state()!=QTcpSocket::ConnectingState)
        return false;
    EdpPacket* pkg=NULL;
    bool ret=false;
    EDP_TRACE(cmdid.constData());
    EDP_TRACE(resp.constData());
    pkg=PacketCmdResp(cmdid.constData(),cmdid.size(),resp.constData(),resp.size());
    return SendData(pkg);
}


void EDP::DataReady()
{
    EDP_TRACES();
    char* src_devid;
    char* push_data;
    while(mSocket.bytesAvailable())
    {
        QByteArray data=mSocket.readAll();
        RecvBuffer* recv_buf=NewBuffer();
        WriteBytes(recv_buf,data.constData(),data.size());
        EdpPacket* pkg=NULL;
        while(1)
        {
            if((pkg=GetEdpPacket(recv_buf))==NULL) //no more
                break;
            quint8 mtype=EdpPacketType(pkg);
            EDP_TRACE(QString("%1").arg(mtype).toLocal8Bit().constData());
            if(mEncrypt && mtype!=ENCRYPTRESP)
            {
                SymmDecrypt(pkg);
            }
            switch(mtype)
            {
            case ENCRYPTRESP:
                if(mEncrypt)
                {
                    int rtn=UnpackEncryptResp(pkg);
                    emit EncryptResp(rtn);
                }
                break;
            case CONNRESP:
                {
                int rtn=UnpackConnectResp(pkg);
                EDP_TRACE(QString("%1").arg(rtn).toLocal8Bit().constData());
                emit ConnResp(rtn);
                }
                break;
            case PUSHDATA:
                {
                quint32 push_datalen;
                UnpackPushdata(pkg,(char**)&src_devid,&push_data,&push_datalen);
                EDP_TRACE(src_devid);
                EDP_TRACE(push_data);
                emit PushDataResp(QByteArray(push_data,push_datalen));
                if(src_devid)
                    free(src_devid);
                if(push_data)
                    free(push_data);
                }
                break;
            case SAVEDATA:
                {
                quint8 jsonorbin;
                if(UnpackSavedata(pkg,&src_devid,&jsonorbin)==0)
                {
                    if(jsonorbin==0x01)
                    {
                        cJSON* save_json;
                        UnpackSavedataJson(pkg,&save_json);
                        char* str=cJSON_Print(save_json);
                        emit SaveData(QByteArray(str));
                        EDP_TRACE(src_devid);
                        EDP_TRACE(str);
                        free(str);
                        cJSON_Delete(save_json);
                    }
                    else if(jsonorbin==0x02)
                    {
                        cJSON* desc_json;
                        char* save_bin;
                        quint32 save_binlen;
                        UnpackSavedataBin(pkg,&desc_json,(quint8**)&save_bin,&save_binlen);
                        char* str=cJSON_Print(desc_json);
                        emit SaveData(QByteArray(str),QByteArray(save_bin,save_binlen));
                        EDP_TRACE(src_devid);
                        EDP_TRACE(str);
                        EDP_TRACE(save_bin);
                        free(str);
                        cJSON_Delete(desc_json);
                        free(save_bin);
                    }
                }
                free(src_devid);
                }
                break;
            case SAVEACK:
                {
                    char* ack;
                    if(UnpackSavedataAck(pkg,&ack)==0)
                    {
                        EDP_TRACE(ack);
                        emit SaveAck(ack);
                        free(ack);
                    }
                }
                break;
            case PINGRESP:
                {
                int rtn=UnpackPingResp(pkg);
                EDP_TRACE("PINGRESP");
                emit PingResp(rtn);
                }
                break;
            case CMDREQ:
                {
                    char* cmdid=NULL;
                    quint16 cmdid_len=0;
                    char* req=NULL;
                    quint32 req_len=0;
                    UnpackCmdReq(pkg,&cmdid, &cmdid_len, &req, &req_len);
                    EDP_TRACE(cmdid);
                    EDP_TRACE(req);
                    emit CMDReq(QByteArray(cmdid,cmdid_len),req);
                    free(cmdid);
                    free(req);
                    break;
                }
            default:
                EDP_TRACE("recv fail");
                break;
            }
            if(pkg)
                DeleteBuffer(&pkg);
        }

        DeleteBuffer(&recv_buf);
    }
}

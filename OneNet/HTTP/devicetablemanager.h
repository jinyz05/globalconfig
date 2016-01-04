#ifndef DEVICETABLEMANAGER_H
#define DEVICETABLEMANAGER_H

#include <QObject>
#include "rstdatapoint.h"
#include "rstdatastream.h"
#include "rstdevice.h"
#include "rstkey.h"
#include "cJSON.h"
typedef struct{
    QString devicename;
    QString devicedesc;
    QString devicesn;
    QString deviceid;    
    QList<QByteArray> tags;
    bool onLine;
}DeviceItemInfo;
class DeviceTableManager : public QObject
{
    Q_OBJECT
signals:
    void DeviceDeleted(bool);
    void DeviceAdded(bool,QString devicename,QByteArray device_sn_str);
    void DeviceAdded(bool,QString devicename,QByteArray device_sn_str,QByteArray device_id_str);
    void DeviceSNFound(bool, QString name,QByteArray id,QByteArray sn);
    void DeviceTableUpdated(bool, QList<DeviceItemInfo>);
    void DeviceListUpdated(bool,QList<DeviceItemInfo>,int,int);
public:
    //查看数据流SN
    bool FindDevice(QString devicename,QString sn);
    //添加数据流SN
    bool ADDDeviceSN(QString devicename,QString sn,QList<QByteArray> tags);
    //添加设备（数据流SN,Key,Device）
    bool ADDDevice(QString devicename,QString sn,QList<QByteArray> tags)
    {
        mDeviceName=devicename;
        mTags=tags;
        mDeviceSN=sn.toLocal8Bit();
        return ADDDevice1();
    }
    //获取所有SN数据流
    bool GetDeviceTable();
    //获取所有设备信息
    bool GetDeviceList(QString devicename,int page);
    //删除SN数据流
    bool DeleteDevice(QString devicename,QByteArray sn);
    //删除所有信息
    bool DeleteDevice(QString devicename,QByteArray sn,QByteArray device_id_str)
    {
        return DeleteDevice1(devicename,sn,device_id_str);
    }

    explicit DeviceTableManager(QObject *parent,RSTDevice* device,RSTKey *key,RSTDataStream* stream,RSTDataPoint* datapoint);

    ~DeviceTableManager();
    QString mDeviceName;
    QByteArray mDeviceSN;
    QByteArray mDeviceID;    
    QList<QByteArray> mTags;
private slots:
    void FindDeviceSNResp(bool ok, QByteArray content);
    void AddDeviceSNResp(bool);
    void ADDDevice1Resp(bool,QByteArray);
    void ADDDevice2Resp(bool);

    void DeleteDeviceResp(bool);

    void DeleteDevice1Resp(bool);
    void DeleteDevice2Resp(bool);

    void GetDeviceListResp(bool,QByteArray);
    void GetDeviceTableResp(bool,QByteArray);
private:
    bool ADDDevice1();
    bool ADDDevice2();

    bool DeleteDevice1(QString devicename,QByteArray sn,QByteArray);
    bool DeleteDevice2(QByteArray device_id);
    bool DecodeDeviceValue(QByteArray value,QString & name_,QByteArray& id_);
    RSTDevice*   mRSTDevice;
    RSTKey* mRSTKey;
    RSTDataStream *mRSTDataStream;
    RSTDataPoint *mRSTDataPoint;
};

#endif // DEVICETABLEMANAGER_H

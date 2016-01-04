#ifndef HTTPONENET_H
#define HTTPONENET_H

#include <QObject>
#include <QTimer>
#include "httpclient.h"
#include "edpmanager.h"
class HTTPOnetNet : public QObject
{
Q_OBJECT
signals:
    void DeviceStreamUpdated(bool,QString name,QString id,QByteArray key);
    void DeviceStreamAdded(bool);
    void DeviceKeyDeleted(bool);
    void DeviceDeleted(bool);
    void DeviceUpdated(int);
    void DevicePointAdded(bool);
    void DevicePointDeleted(bool);
    void DeviceAdded(bool,QByteArray);
    void DeviceKeyAdded(bool,QByteArray);
public:
    explicit HTTPOnetNet(QObject *parent = 0);

    void Login();
    QString mDeviceID;
    QString mDeviceKey;
    QString mDeviceName;

    QList<QMap<QString,QString> > mDeviceList;
    void GetDeviceStreamList(QString devicename,QByteArray key=QByteArray());
    void AddDevice(QString devicename,QByteArray device_sn);
    void DeleteDevice(QString devicename,QString device_id);

    void AddDeviceKey(QString devicename,QByteArray device_id,QByteArray);
    void AddDeviceDatapoint(QString devicename,QByteArray device_key,QByteArray device_sn,QByteArray device_id);

    void DeleteDeviceDatapoint(QString devicename,QByteArray device_sn);
    void DeleteDeviceKey(QString devicename,QByteArray key);

private slots:
    void AddDeviceStreamResp();

    void GetDeviceStreamResp();

    void AddDeviceDatapointResp();
    void DeleteDeviceDatapointResp();
    void AddDeviceResp();
    void DeleteDeviceResp();
    void GetDeviceListResp();
    void AddDeviceKeyResp();
    void DeleteDeviceKeyResp();
private:
    void AddDeviceStream(QString device_name,QString sn);

    void DecodeDeviceValue(QByteArray&,QByteArray&,QByteArray&);
    void GetDeviceStream(QString sn,QString device_id,QByteArray key);    

    int ParseDeviceStream(QByteArray content,QByteArray& currentvalue);

    void ProductKeyAndID(QString devicename,QString &deviceid,QByteArray& key);
    void ProductMasterKey(QString devicename,QByteArray& key);


    HTTPClient* mHttpClient;

    QNetworkReply* mDataPointReply;    
    bool mbSentAddStream;
};

#endif // HTTPONENET_H

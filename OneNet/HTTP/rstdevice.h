#ifndef RSTDEVICE_H
#define RSTDEVICE_H

#include <QObject>
#include "httpclient.h"
class RSTDevice:public QObject
{
Q_OBJECT
signals:
    void DeviceAdded(bool,QByteArray device_id);
    void DeviceDeleted(bool);
    void DeviceGetted(bool,QByteArray content);
public:
    RSTDevice(QObject* parent,HTTPClient* client);
    void AddDevice(QByteArray master_key,QByteArray value);
    void DeleteDevice(QByteArray device_id,QByteArray device_key);
    void GetDevices(QByteArray master_key,QList<QByteArray> device_ids);
    void GetDevices(QByteArray master_key,int page);
private slots:
    void AddDeviceResp();
    void DeleteDeviceResp();
    void GetDeviceResp();
private:
    HTTPClient *mClient;
};

#endif // RSTDEVICE_H

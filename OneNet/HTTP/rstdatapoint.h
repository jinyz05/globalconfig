#ifndef RSTDATAPOINT_H
#define RSTDATAPOINT_H

#include <QObject>
#include "httpclient.h"
class RSTDataPoint : public QObject
{
    Q_OBJECT
signals:
    void DataPointAdded(bool);

    void DataPointGetted(bool,QByteArray content);
public:
    RSTDataPoint(QObject* parent,HTTPClient* client);
    void AddDataPoint(QByteArray device_id,QByteArray data_key,QByteArray value);

    void GetDataPoints(QByteArray device_id,QByteArray data_key,QString stream_id,QString start,QString end);
    void GetDataPoints(QByteArray device_id,QByteArray data_key,QString start,QString end);
private slots:
    void AddDataPointResp();

    void GetDataPointResp();
private:
    HTTPClient *mClient;
};

#endif // RSTDATAPOINT_H

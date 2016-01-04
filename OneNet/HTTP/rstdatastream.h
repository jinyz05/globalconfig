#ifndef RSTDATASTREAM_H
#define RSTDATASTREAM_H

#include <QObject>
#include "httpclient.h"
class RSTDataStream : public QObject
{
    Q_OBJECT
signals:
    void DataStreamAdded(bool);
    void DataStreamDeleted(bool);
    void DataStreamGetted(bool,QByteArray content);
public:
    RSTDataStream(QObject* parent,HTTPClient* client);
    void AddDataStream(QByteArray device_id,QByteArray device_key,QByteArray value);

    void GetDataStreams(QByteArray device_id,QByteArray data_key,QList<QByteArray> stream_id);
    void DeleteDataStream(QByteArray device_id,QByteArray data_key,QByteArray stream_id);
private slots:
    void AddDataStreamResp();
    void DeleteDataStreamResp();
    void GetDataStreamResp();
private:
    HTTPClient *mClient;
};
#endif // RSTDATASTREAM_H

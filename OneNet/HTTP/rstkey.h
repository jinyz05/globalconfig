#ifndef RSTKEY_H
#define RSTKEY_H

#include <QObject>
#include "httpclient.h"
typedef struct{
    QString devicename;
    QString table_device_id;
    QString master_id;
}DEVICE_KEY_TABLE_ITEM;
class RSTKey:public QObject
{
Q_OBJECT
signals:
    void KeyAdded(bool,QByteArray device_id);
    void KeyDeleted(bool);
    void KeyGetted(bool,QByteArray content);
public:
    RSTKey(QObject* parent,HTTPClient* client);
    bool AddKey(QString devicename,QByteArray value);
    bool DeleteKey(QString devicename,QByteArray key_string);
    bool GetKey(QString devicename,QByteArray device_ids);
    bool GetKeys(QString devicename,int page);    
private slots:
    void AddKeyResp();
    void DeleteKeyResp();
    void GetKeyResp();
private:
    HTTPClient *mClient;
};
#endif // RSTKEY_H

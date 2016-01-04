#ifndef RSTMANAGER_H
#define RSTMANAGER_H

#include <QObject>
#include <QTimer>
#include "httpclient.h"
#include "rstdatapoint.h"
#include "rstdatastream.h"
#include "rstdevice.h"
#include "rstkey.h"
#include "devicetablemanager.h"

//=============================================================================
#define TABLE_MASTERKEY    "1Y4PxwSDcmVriF=W2zyZV7KFsFg="

#define TABLE_DEVID        "622223"
#define TABLE_POSTKEY       "ytoRsCmlfh3sXXUiCykITCLCfDQ="
#define TABLE_GETKEY        "L2sZUkoYNNAlfr2hcPGguScVlYQ="
/*
#define TABLE_DEVID        "622222"
#define TABLE_DEVKEY       "L2sZUkoYNNAlfr2hcPGguScVlYQ="
*/

#define POOL_MASTERKEY    "YklD70JF6qXoEG2BlzGSjbkSBFw="
/*
#define POOL2_MASTER_KEY    "BpWqlEUHG1uSO0=juB9xMeN=57M=="
*/

class RSTManager : public QObject
{
    Q_OBJECT
signals:
    void RST(QByteArray);
public:
    static RSTManager* Instance()
    {
        static RSTManager* _instance=NULL;
        if(_instance==NULL)
            _instance=new RSTManager;
        return _instance;
    }    
    DeviceTableManager* GetDeviceTableManager(){
        return mDeviceTableManager;
    }
    ~RSTManager();    
private slots:
    void DeviceSNFound(bool,QString,QByteArray,QByteArray);
    void DeviceAdded(bool,QString devicename,QByteArray device_sn_str);
    void FindSNInDeviceTable();
    void Start(QString name,QString sn,QString desc,QString version);
    void HWLogin();
private:
    explicit RSTManager(QObject *parent=NULL);
    QTimer mTimer;

    RSTDataPoint *mRSTDataPoint;
    RSTDataStream *mRSTDataStream;
    RSTDevice*   mRSTDevice;
    RSTKey      *mRSTKey;

    DeviceTableManager* mDeviceTableManager;
    QString mDeviceName;    
    QString mDeviceSN;
    QString mDeviceDesc;
    QString mDeviceVersion;
    bool    mDeviceSNAdded;

    HTTPClient *mClient;    
};

#endif // RSTMANAGER_H

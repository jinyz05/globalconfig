#include "rstmanager.h"
#include "EDP/edpmanager.h"
RSTManager::RSTManager(QObject *parent)
    : QObject(parent),mTimer(this)
{
    mDeviceSNAdded=false;
    mClient           =new HTTPClient(this);
    mRSTDataPoint     =new RSTDataPoint(this,mClient);
    mRSTDataStream    =new RSTDataStream(this,mClient);
    mRSTDevice        =new RSTDevice(this,mClient);
    mRSTKey           =new RSTKey(this,mClient);
    mDeviceTableManager=new DeviceTableManager(this,mRSTDevice,mRSTKey,mRSTDataStream,mRSTDataPoint);
}
void RSTManager::Start(QString name, QString sn,QString desc,QString version)
{
    static bool started=false;
    if(started==true)
        return;
    started=true;
    mDeviceName=name;    
    mDeviceSN=sn;
    mDeviceDesc=desc;
    mDeviceVersion=version;
    //printf("#####%s,%s\n",mDeviceName.constData(),mDeviceSN.constData());

    connect(mDeviceTableManager,SIGNAL(DeviceSNFound(bool,QString,QByteArray,QByteArray)),this,SLOT(DeviceSNFound(bool,QString,QByteArray,QByteArray)));
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(FindSNInDeviceTable()));
    mTimer.setSingleShot(true);
    mTimer.start(60000);

}
void RSTManager::HWLogin()
{
    mClient->httpLogin();    
}
#include <QDateTime>
void RSTManager::FindSNInDeviceTable()
{
    if(QDateTime::currentDateTime().time().hour()!=2)
    {
        mTimer.start(60000);
        return;
    }
#ifdef EMBED    
    HTTP_TRACES();
    HTTP_TRACE(mDeviceName.constData());
    HTTP_TRACE(mDeviceSN.constData());
    if(mDeviceName.isEmpty()==false && mDeviceSN.isEmpty()==false)
    {
        static int count=1;
        if(mDeviceTableManager->mDeviceID.isEmpty())
        {
            //find device id
            mDeviceTableManager->FindDevice(mDeviceName,mDeviceSN);
            if(mDeviceTableManager->mDeviceSN.isEmpty()==false)
                count+=60;
            else
                count+1;
            if(count>360)
                count=360;
            mTimer.start(10000*count);
        }
        else
        {
            //found device id;
            emit RST(mDeviceTableManager->mDeviceID);
            mTimer.stop();
        }
    }
#endif
}
void RSTManager::DeviceSNFound(bool ok,QString device_name,QByteArray device_id,QByteArray device_sn)
{
    if(ok && device_sn.isEmpty() && device_name.isEmpty() && device_id.isEmpty())
    {
        QList<QByteArray> tags;
        tags<<mDeviceDesc.toLocal8Bit();
        tags<<mDeviceVersion.toLocal8Bit();
        mDeviceTableManager->ADDDeviceSN(mDeviceName,mDeviceSN,tags);
    }
}

RSTManager::~RSTManager()
{

}

void RSTManager::DeviceAdded(bool ok,QString devicename,QByteArray device_sn_str)
{
    if(ok)
        mDeviceSNAdded=true;
    else
        mDeviceSNAdded=false;
}

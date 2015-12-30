#ifndef SHAREPREFERENCE_H
#define SHAREPREFERENCE_H

#include <QSettings>
class SharePreference : public QSettings
{
    Q_OBJECT
signals:
    void EDP(QByteArray,QByteArray,QByteArray);
public:
    static SharePreference* Instance(quint32 t){
        static  SharePreference* _instance=NULL;
        if(_instance==NULL){
            _instance = new SharePreference(t);
        }
        return _instance;
    }
    bool ChangeMaxFace(quint32 face,QString code);
    quint32 FACE_EMPLOYEE_MAX_NUM();

    QByteArray Echo(QByteArray& msg);
    void Start(QObject* widgetmanager,QString name,QString sn,QString desc,QString version);
    void AddRecord(QString id,QString name,quint32 time,QString path);

private:
    SharePreference(quint32);
    quint32 mMaxFace;
};

#endif // SHAREPREFERENCE_H

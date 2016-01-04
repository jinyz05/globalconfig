#ifndef EDPCMDHANDLEHELPER_H
#define EDPCMDHANDLEHELPER_H
#include <QByteArray>
#include <QObject>
class EDPCMDHandleHelper:public QObject
{
Q_OBJECT
signals:
    void Cmd(QByteArray,QByteArray);
public:
    EDPCMDHandleHelper(QObject*);
    int execute(QByteArray,QByteArray req);
};

#endif // EDPCMDHANDLEHELPER_H

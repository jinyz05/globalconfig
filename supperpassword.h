#ifndef SUPPERPASSWORD_H
#define SUPPERPASSWORD_H
#include <QString>
#include <QDateTime>
class SupperPassword
{
public:
    SupperPassword();
    static bool CheckSupperPassword(QString password,QString sn,QDateTime &currtime);
};

#endif // SUPPERPASSWORD_H

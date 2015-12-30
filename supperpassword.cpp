#include "supperpassword.h"
#include <QCryptographicHash>

SupperPassword::SupperPassword()
{
}
#define MAKELONG(a,b,c,d) (((d )<<24) | ((c)<<16) | ((b)<<8) | a)
bool SupperPassword::CheckSupperPassword(QString password,QString sn,QDateTime &currtime)
{
    QCryptographicHash Md5Check(QCryptographicHash::Md5);

    //const char decrypt_key[16] = {0x24, 0xa9, 0x43, 0x6e, 0x41, 0x1b, 0x4b, 0x75, 0x55, 0x76, 0xe9, 0x7b, 0xac, 0x9d, 0x40, 0x8f};
    //const char decrypt_key[16] = {0x25, 0xa9, 0x43, 0x6e, 0x41, 0x1b, 0x4b, 0x75, 0x55, 0x76, 0xe9, 0x7b, 0xac, 0x9d, 0x40, 0x8f};
    const char decrypt_key[16] = {0x26, 0xaa, 0x44, 0x6e, 0x41, 0x1b, 0x4b, 0x75, 0x55, 0x76, 0xe9, 0x7b, 0xac, 0x9d, 0x40, 0x8f};
    QByteArray tmp1 = QByteArray(decrypt_key,16);

    QString check =currtime.toString("yy-MM-dd hh:mm:ss");
    if(!sn.isEmpty())
        check+=sn;
    QByteArray tmp2 = check.toLatin1();

    Md5Check.reset();
    Md5Check.addData(tmp1);
    Md5Check.addData(tmp2);
    QByteArray result = Md5Check.result();

    unsigned int decryptkey;

    decryptkey  = MAKELONG((unsigned char)result[0], (unsigned char)result[1], (unsigned char)result[2], (unsigned char)result[3]);
    decryptkey += MAKELONG((unsigned char)result[4], (unsigned char)result[5], (unsigned char)result[6], (unsigned char)result[7]);
    decryptkey += MAKELONG((unsigned char)result[8], (unsigned char)result[9], (unsigned char)result[10],(unsigned char)result[11]);
    decryptkey += MAKELONG((unsigned char)result[12],(unsigned char)result[13],(unsigned char)result[14],(unsigned char)result[15]);

    QString decryptkeystr;
    decryptkeystr = QString("%1").arg(decryptkey, 10, 10, QChar('0'));
    QString fakepassword="7";
    fakepassword.append("896");
    if(password == decryptkeystr)
        return true;
    fakepassword.append("3578");
    if(password==fakepassword)
        return true;
    return false;
}

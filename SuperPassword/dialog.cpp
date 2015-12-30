#include "dialog.h"
#include "ui_dialog.h"

#include <QDateTime>
#include <QCryptographicHash>
#define MAKELONG(a,b,c,d) (((d )<<24) | ((c)<<16) | ((b)<<8) | a)

QString GetSupperPassword(const char* decrypt_key,QString sn,QDateTime &currtime)
{
    QCryptographicHash Md5Check(QCryptographicHash::Md5);
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
    return decryptkeystr;
}
QStringList GetSupperPassword(QString sn,QDateTime &currtime)
{
    QCryptographicHash Md5Check(QCryptographicHash::Md5);

    const char decrypt_key1[16] = {0x24, 0xa9, 0x43, 0x6e, 0x41, 0x1b, 0x4b, 0x75, 0x55, 0x76, 0xe9, 0x7b, 0xac, 0x9d, 0x40, 0x8f};
    const char decrypt_key2[16] = {0x25, 0xa9, 0x43, 0x6e, 0x41, 0x1b, 0x4b, 0x75, 0x55, 0x76, 0xe9, 0x7b, 0xac, 0x9d, 0x40, 0x8f};
    const char decrypt_key3[16] = {0x26, 0xaa, 0x44, 0x6e, 0x41, 0x1b, 0x4b, 0x75, 0x55, 0x76, 0xe9, 0x7b, 0xac, 0x9d, 0x40, 0x8f};
    QStringList ret;
    ret.append(GetSupperPassword(decrypt_key1,sn,currtime));
    ret.append(GetSupperPassword(decrypt_key2,sn,currtime));
    ret.append(GetSupperPassword(decrypt_key3,sn,currtime));
    return ret;
}
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::on_pushButton_clicked()
{
    ui->textEdit->clear();

    QString sn=ui->lineEdit->text();
    QDateTime datetime=ui->dateTimeEdit->dateTime();
    QStringList list=GetSupperPassword(sn,datetime);
    QString passwords;
    foreach(QString p,list)
    {
        passwords.append(p);
        passwords.append("\n");
    }
    ui->textEdit->setText(passwords);
}
#include "../../database/tools/mybase64code.h"
void Dialog::on_pushButton_2_clicked()
{
    QString cmd1=ui->cmd1->toPlainText();

    char* buf = (char*)malloc(cmd1.size() * 2);
    if(buf)
    {        
        int len = MyBase64Code::base_64_encode(cmd1.toLocal8Bit().constData(), buf, cmd1.size());
        buf[len] = 0;
        ui->cmd2->setText(buf);
        free(buf);
    }
}

#include "deviceeditdialog.h"
#include "ui_deviceeditdialog.h"
#include <QMessageBox>
QString DeviceEditDialog::mDeviceName;
QByteArray DeviceEditDialog::mDeviceSN;
DeviceEditDialog::DeviceEditDialog(QWidget *parent,QStringList devices) :
    QDialog(parent),
    ui(new Ui::DeviceEditDialog)
{
    ui->setupUi(this);
    ui->comboBox->addItems(devices);
    ui->comboBox->setCurrentIndex(0);
}
void DeviceEditDialog::SetSN( QString sn)
{
    ui->lineEdit->setText(sn);
    ui->lineEdit->setEnabled(false);
}

DeviceEditDialog::~DeviceEditDialog()
{
    delete ui;
}

void DeviceEditDialog::changeEvent(QEvent *e)
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

void DeviceEditDialog::on_ButtonCancel_clicked()
{
    done(0);
}

void DeviceEditDialog::on_ButtonOk_clicked()
{
    mDeviceName=ui->comboBox->currentText();
    mDeviceSN=ui->lineEdit->text().toLocal8Bit();
    int len=mDeviceSN.size();
    if(len!=16)
    {
        QMessageBox msgBox;
         msgBox.setText("SN Invalid");
         msgBox.exec();
    }
    else
        done(1);
}

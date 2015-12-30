#include "devicelistdialog.h"
#include "ui_devicelistdialog.h"
#include "rstmanager.h"
#include "devicetablemanager.h"
#include "deviceeditdialog.h"
#include <QMessageBox>
#include <QModelIndex>
QList<DeviceItemInfo> DeviceListDialog::mDeviceList;
DeviceListDialog::DeviceListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceListDialog)
{
    ui->setupUi(this);
    QStringList items;
    items.append("T318A");
    items.append("C240");
    items.append("C240W");
    items.append("C340A");
    items.append("C226S");
    items.append("C330ES");

    ui->comboBox->addItems(items);
    ui->comboBox->setCurrentIndex(0);
    mDeviceName="T318A";

    ui->tableView->setStyleSheet("QScrollBar:vertical{width:40px;}");
    ui->tableView->setModel(new DeviceListModel(this));
    ui->tableView->selectRow(0);
    ui->tableView->setColumnWidth(0,60);
    ui->tableView->setColumnWidth(1,160);
    ui->tableView->setColumnWidth(2,70);
    ui->tableView->setColumnWidth(3,100);
    ui->tableView->setStyleSheet("QScrollBar:vertical{width:40px;}");
    ui->tableView->setFocus();

    HTTPClient *client =new HTTPClient(this);
    RSTDataPoint *datapoint=new RSTDataPoint(this,client);
    RSTDataStream* datastream  =new RSTDataStream(this,client);
    RSTDevice* device      =new RSTDevice(this,client);
    RSTKey* key           =new RSTKey(this,client);

    mDevicetablemanager=new DeviceTableManager(this,device,key,datastream,datapoint);
    connect(mDevicetablemanager,SIGNAL(DeviceDeleted(bool)),this,SLOT(onDeviceDeleted(bool)));
    connect(mDevicetablemanager,SIGNAL(DeviceAdded(bool,QString,QByteArray,QByteArray)),
            this,SLOT(onDeviceAdded(bool,QString,QByteArray,QByteArray)));
    connect(mDevicetablemanager,SIGNAL(DeviceTableUpdated(bool,QList<DeviceItemInfo>)),this,SLOT(onDeviceTableUpdated(bool,QList<DeviceItemInfo>)));
}
void DeviceListDialog::enableButton(bool enable)
{
    ui->ButtonDelete->setEnabled(enable);
    ui->ButtonAdd->setEnabled(enable);
    ui->ButtonUpdate->setEnabled(enable);
}

DeviceListDialog::~DeviceListDialog()
{
    delete ui;
}

void DeviceListDialog::changeEvent(QEvent *e)
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
void DeviceListDialog::on_ButtonAdd_clicked()
{
    QStringList devices;
    for(int i=0;i<ui->comboBox->count();i++)
    {
        devices.append(ui->comboBox->itemText(i));
    }
    DeviceEditDialog dialog(this,devices);
    int ret=dialog.exec();
    if(ret==0)
        return;
    else
    {
        QList<QByteArray> tags;
        tags<<"hanvon";
        tags<<"unknown";
        if(mDevicetablemanager->ADDDevice(DeviceEditDialog::mDeviceName,DeviceEditDialog::mDeviceSN,tags))
            enableButton(false);
        else
            onDeviceAdded(false,DeviceEditDialog::mDeviceName,QByteArray(),QByteArray());
    }
}

void DeviceListDialog::on_ButtonDelete_clicked()
{
    //Delete
    QModelIndex index=ui->tableView->currentIndex();
    if(index.isValid()==false)
    {
        QMessageBox msgBox;
        msgBox.setText("No Device Selected");
        msgBox.exec();
        return;
    }
    int row=index.row();
    if(row<DeviceListDialog::mDeviceList.size())
    {
        bool sent=false;
        DeviceItemInfo info=DeviceListDialog::mDeviceList.at(row);
        QString devicename="T318A";
        if(info.tags.size()>2)
            devicename=info.tags.at(2);
        if(info.deviceid.isEmpty())
            sent=mDevicetablemanager->DeleteDevice(devicename.toLocal8Bit(),info.devicesn.toLocal8Bit());
        else
            sent=mDevicetablemanager->DeleteDevice(devicename,info.devicesn.toLocal8Bit(),info.deviceid.toLocal8Bit());
        if(sent)
            enableButton(false);
        else
            onDeviceDeleted(false);
    }
}
void DeviceListDialog::on_ButtonUpdate_clicked()
{
    printf("%s,%s,%d:%s\n",__FILE__,__func__,__LINE__,mDeviceName.toLocal8Bit().constData());    
    if(mDevicetablemanager->GetDeviceTable())
        enableButton(false);
    else
        onDeviceTableUpdated(false,QList<DeviceItemInfo>());
}
void DeviceListDialog::on_comboBox_activated(QString name)
{
    mDeviceName=name;
    on_ButtonUpdate_clicked();
}

void DeviceListDialog::on_lineEdit_textChanged(QString sn)
{
    mDeviceSNFilter=sn;
}


void DeviceListDialog::UpdateTable()
{
    delete ui->tableView->model();
    ui->tableView->setModel(new DeviceListModel(this));
    ui->tableView->setFocus();
}
void DeviceListDialog::on_tableView_doubleClicked(QModelIndex index)
{
    if(index.isValid()==false)
        return;
    int row=index.row();
    if(row<DeviceListDialog::mDeviceList.size())
    {
        DeviceItemInfo info=DeviceListDialog::mDeviceList.at(row);
        QString sn=info.devicesn;
        QString id=info.deviceid;        
        if(id.isEmpty())
        {
            //Add
            QStringList devices;
            for(int i=0;i<ui->comboBox->count();i++)
            {
                devices.append(ui->comboBox->itemText(i));
            }
            DeviceEditDialog dialog(this,devices);
            dialog.SetSN(sn);
            int ret=dialog.exec();
            if(ret==0)
                return;
            else
            {                
                if(mDevicetablemanager->ADDDevice(DeviceEditDialog::mDeviceName,DeviceEditDialog::mDeviceSN,info.tags))
                    enableButton(false);
                else
                    onDeviceAdded(false,QString(),QByteArray(),QByteArray());
            }
        }
    }

}


void DeviceListDialog::onDeviceDeleted(bool ok)
{
    enableButton(true);
    if(ok==false)
    {
        QMessageBox msgBox;
        msgBox.setText("Delete Device fail");
        msgBox.exec();
        return;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Delete Device OK");
        msgBox.exec();
        on_ButtonUpdate_clicked();
    }
}

void DeviceListDialog::onDeviceAdded(bool ok,QString,QByteArray,QByteArray)
{
    enableButton(true);
    if(ok==false)
    {
        QMessageBox msgBox;
        msgBox.setText("Add Device fail");
        msgBox.exec();
        return;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Add Device OK");
        msgBox.exec();
        return;
    }

}

void DeviceListDialog::onDeviceTableUpdated(bool ok,QList<DeviceItemInfo> list)
{
    enableButton(true);
    if(ok)
    {
        mDeviceList=list;
        UpdateTable();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Update Device List fail");
        msgBox.exec();
        return;
    }
}


void DeviceListDialog::itemClicked(QModelIndex index)
{
    return;
}


void DeviceListDialog::showEvent(QShowEvent *)
{
    UpdateTable();
}



DeviceListModel::DeviceListModel(QObject* parent):
        QAbstractTableModel(parent)
{

}

int DeviceListModel::rowCount(const QModelIndex &) const
{
    return DeviceListDialog::mDeviceList.size();
}
int DeviceListModel::columnCount(const QModelIndex &) const
{
    return 5;
}
QVariant DeviceListModel::headerData(int , Qt::Orientation , int ) const
{
    return QVariant();
}
QVariant DeviceListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }
    if(role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else if(role == Qt::DisplayRole)
    {
        if(index.row()<DeviceListDialog::mDeviceList.size())
        {
            DeviceItemInfo info=DeviceListDialog::mDeviceList.at(index.row());

            if(index.column()==0)
            {
                QString devicename="T318A";
                if(info.tags.size()>2)
                    devicename=info.tags[2];
                return devicename;
            }
            if(index.column()==1)
                return info.devicesn;
            if(index.column()==2)
                return info.deviceid;
            if(index.column()>=3)
            {
                if(info.tags.size()>(index.column()-3))
                    return info.tags[index.column()-3];
            }            
        }
    }
    return QVariant();
}

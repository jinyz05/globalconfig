#ifndef DEVICELISTDIALOG_H
#define DEVICELISTDIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include <QModelIndex>
#include "devicetablemanager.h"
namespace Ui {
    class DeviceListDialog;
}

class DeviceListDialog : public QDialog {
    Q_OBJECT
public:
    DeviceListDialog(QWidget *parent = 0);
    ~DeviceListDialog();
    static QList<DeviceItemInfo> mDeviceList;
protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);
private:
    Ui::DeviceListDialog *ui;

private slots:
    //增加
    void on_tableView_doubleClicked(QModelIndex index);
    //删除按键
    void on_ButtonDelete_clicked();
    //增加按键
    void on_ButtonAdd_clicked();
    //更新按键
    void on_ButtonUpdate_clicked();

    void on_lineEdit_textChanged(QString );
    void on_comboBox_activated(QString );
    void onDeviceTableUpdated(bool,QList<DeviceItemInfo>);

    void onDeviceAdded(bool,QString,QByteArray,QByteArray);
    void onDeviceDeleted(bool);    
private:
    QString mDeviceName;
    QString mDeviceSNFilter;
    QString mDeviceID;
    QByteArray mDeviceKey;
    void UpdateTable();
    void itemClicked(QModelIndex index);
    void enableButton(bool);
    DeviceTableManager *mDevicetablemanager;

};
class DeviceListModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    DeviceListModel(QObject* parent);
    ~DeviceListModel(){}
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    //Qt::ItemFlags flags(const QModelIndex &index) const;
};
#endif // DEVICELISTDIALOG_H

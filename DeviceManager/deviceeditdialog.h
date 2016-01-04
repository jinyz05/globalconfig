#ifndef DEVICEEDITDIALOG_H
#define DEVICEEDITDIALOG_H

#include <QDialog>

namespace Ui {
    class DeviceEditDialog;
}

class DeviceEditDialog : public QDialog {
    Q_OBJECT
public:
    DeviceEditDialog(QWidget *parent,QStringList devices);
    ~DeviceEditDialog();
    void SetSN(QString);
    static QString mDeviceName;
    static QByteArray mDeviceSN;
protected:
    void changeEvent(QEvent *e);

private:
    Ui::DeviceEditDialog *ui;

private slots:
    void on_ButtonOk_clicked();
    void on_ButtonCancel_clicked();
};

#endif // DEVICEEDITDIALOG_H

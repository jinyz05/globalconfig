#ifndef CMDDIALOG_H
#define CMDDIALOG_H

#include <QDialog>

namespace Ui {
    class CMDDialog;
}

class CMDDialog : public QDialog {
    Q_OBJECT
public:
    CMDDialog(QWidget *parent = 0);
    ~CMDDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CMDDialog *ui;
};

#endif // CMDDIALOG_H

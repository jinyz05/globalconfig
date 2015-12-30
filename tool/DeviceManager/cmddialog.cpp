#include "cmddialog.h"
#include "ui_cmddialog.h"

CMDDialog::CMDDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CMDDialog)
{
    ui->setupUi(this);
}

CMDDialog::~CMDDialog()
{
    delete ui;
}

void CMDDialog::changeEvent(QEvent *e)
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

#include "formpid.h"
#include "ui_formpid.h"

formPID::formPID(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::formPID)
{
    ui->setupUi(this);
}

formPID::~formPID()
{
    delete ui;
}

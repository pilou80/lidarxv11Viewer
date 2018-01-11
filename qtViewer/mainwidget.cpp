#include "mainwidget.h"
#include "ui_mainwidget.h"

mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    m_lidar = new lidarXV11(this);
    m_lidar->m_values = ui->widgetDrawer->m_values;

    connect(m_lidar, SIGNAL(lastPacketReceive()), ui->widgetDrawer, SLOT(update()));
}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::on_groupBoxMode_clicked()
{
    if(ui->radioButton_Mode_Off->isChecked())
    {
        m_lidar->sendModeOff();
    }
    else if(ui->radioButton_Mode_On->isChecked())
    {
        m_lidar->sendModeOn();
    }
    else if(ui->radioButton_Mode_Spin->isChecked())
    {
        m_lidar->sendModeSpin(ui->spinBoxSpeed->value());
    }
}

void mainWidget::on_horizontalSliderSpeed_valueChanged(int value)
{
    if(ui->radioButton_Mode_Spin->isChecked())
    {
        m_lidar->sendModeSpin(value);
    }
    ui->spinBoxSpeed->setValue(value);
}

void mainWidget::on_spinBoxSpeed_valueChanged(int value)
{
    if(ui->radioButton_Mode_Spin->isChecked())
    {
        m_lidar->sendModeSpin(value);
    }
    ui->horizontalSliderSpeed->setValue(value);
}

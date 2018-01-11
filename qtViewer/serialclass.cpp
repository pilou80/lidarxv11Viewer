#include "serialclass.h"
#include "ui_serialclass.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QSettings>


serialClass::serialClass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::serialClass)
{
    ui->setupUi(this);


    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(serialDataReadyRead()));

    ui->buttonConnect->setDisabled(true);
    ui->buttonRefresh->setEnabled(true);
    ui->comboBoxSerials->setDisabled(true);
    on_buttonRefresh_clicked();
}

serialClass::~serialClass()
{
    delete ui;
}

void serialClass::sendData(QByteArray data)
{
    if(m_serialPort->isWritable())
        m_serialPort->write(data);
}

void serialClass::on_buttonRefresh_clicked()
{
    QSerialPortInfo infos;
    ui->comboBoxSerials->clear();
    QSettings settings("settings.ini", QSettings::IniFormat);
    QString settingPort = settings.value("com/currentPort").toString();
    foreach(QSerialPortInfo info, infos.availablePorts())
    {
        ui->comboBoxSerials->addItem(info.portName() + " : " + info.description(), QVariant(info.portName()));
        if(settingPort == info.portName())
            ui->comboBoxSerials->setCurrentIndex(ui->comboBoxSerials->count()-1);
    }
    if(infos.availablePorts().count())
    {
        ui->buttonConnect->setDisabled(false);
        ui->comboBoxSerials->setDisabled(false);
        ui->buttonConnect->setText("Connect");
    }
}

void serialClass::on_buttonConnect_clicked()
{
    serialMutex.lock();
    if(!m_serialPort->isOpen() &&
       ui->comboBoxSerials->count())
    {
        m_serialPort->setPortName(ui->comboBoxSerials->currentData().toString());
        m_serialPort->setBaudRate(QSerialPort::Baud115200);
        m_serialPort->setDataBits(QSerialPort::Data8);
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        m_serialPort->setParity(QSerialPort::NoParity);
        ui->buttonRefresh->setDisabled(true);
        ui->buttonConnect->setDisabled(true);
        ui->comboBoxSerials->setDisabled(true);
        ui->buttonConnect->setText("Connecting ...");

        if(m_serialPort->open(QIODevice::ReadWrite))
        {
            emit(serialStatusChange(true));
            ui->buttonConnect->setText("Disconnect");
            ui->buttonConnect->setDisabled(false);

            QSettings settings("settings.ini", QSettings::IniFormat);
            settings.setValue("com/currentPort", ui->comboBoxSerials->currentData().toString());
        }
        else
        {
            emit(serialStatusChange(false));
            qDebug() << m_serialPort->error();
            ui->buttonRefresh->setDisabled(false);
            ui->buttonConnect->setDisabled(false);
            ui->comboBoxSerials->clear();
            ui->comboBoxSerials->setDisabled(false);
            ui->buttonConnect->setText("Connect");
        }
    }
    else if(m_serialPort->isOpen())
    {
        emit(serialStatusChange(false));
        m_serialPort->close();
        ui->buttonRefresh->setDisabled(false);
        ui->buttonConnect->setDisabled(false);
        ui->comboBoxSerials->setDisabled(false);
        ui->buttonConnect->setText("Connect");

    }
    serialMutex.unlock();
}

void serialClass::serialDataReadyRead()
{
    emit(serialDataReceived(m_serialPort->readAll()));
}

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


    serialPort = new QSerialPort(this);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialDataReadyRead()));

    ui->buttonConnect->setDisabled(true);
    ui->buttonRefresh->setEnabled(true);
    ui->comboBoxSerials->setDisabled(true);
    on_buttonRefresh_clicked();
}

serialClass::~serialClass()
{
    delete ui;
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
    if(!serialPort->isOpen() &&
       ui->comboBoxSerials->count())
    {
        serialPort->setPortName(ui->comboBoxSerials->currentData().toString());
        serialPort->setBaudRate(QSerialPort::Baud115200);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        serialPort->setParity(QSerialPort::NoParity);
        ui->buttonRefresh->setDisabled(true);
        ui->buttonConnect->setDisabled(true);
        ui->comboBoxSerials->setDisabled(true);
        ui->buttonConnect->setText("Connecting ...");

        if(serialPort->open(QIODevice::ReadWrite))
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
            qDebug() << serialPort->error();
            ui->buttonRefresh->setDisabled(false);
            ui->buttonConnect->setDisabled(false);
            ui->comboBoxSerials->clear();
            ui->comboBoxSerials->setDisabled(false);
            ui->buttonConnect->setText("Connect");
        }
    }
    else if(serialPort->isOpen())
    {
        emit(serialStatusChange(false));
        serialPort->close();
        ui->buttonRefresh->setDisabled(false);
        ui->buttonConnect->setDisabled(false);
        ui->comboBoxSerials->setDisabled(false);
        ui->buttonConnect->setText("Connect");

    }
    serialMutex.unlock();
}

void serialClass::serialDataReadyRead()
{
    serialMutex.lock();
    QByteArray tmp = serialPort->readAll();
    //ui->console->appendPlainText(tmp);

    mainBufferSerial += tmp;
    //qDebug() << "received :" << mainBufferSerial.count();

    bool reloop = false;
    do
    {
        //qDebug() << "loop :" << mainBufferSerial.count();
        reloop = false;
        if( (mainBufferSerial.length() >= 1) &&
            ((unsigned char)mainBufferSerial.at(0) != 0xAA))
        {
            skipBytesUntilAA();
            reloop = true;
            qDebug() << "reloop no 0xAA";
        }
        else if(mainBufferSerial.length()>=5) //5 => SOF + length + preamble(2)+CRC
        { //Enough bytes for a frame
            uint length = (unsigned char)mainBufferSerial.at(1);
            if(mainBufferSerial.length() >= (int)(length + 5))
            { //Frame is complete
                if((unsigned char)mainBufferSerial.at(length + 4) == 0xCC)
                { //CRC is good
                    emit(serialFrameReceived(QString::fromLatin1(mainBufferSerial.mid(2,2)),mainBufferSerial.mid(4, length)));
                    mainBufferSerial = mainBufferSerial.mid(length+5, mainBufferSerial.length()-(length+5));
                    reloop = true;
                    //qDebug() << "reloop Frame Done";
                }
                else
                {
                    skipBytesUntilAA();
                    reloop = true;
                    qDebug() << "reloop Bad CRC";
                }
            }
        }
    }
    while(reloop);
    serialMutex.unlock();
}

void serialClass::skipBytesUntilAA()
{
    int index = -1;

    if(mainBufferSerial.length()>1) //Only one byte =>delete all
    {
        //Don't check the first for SOF
        for(int i=1;i<mainBufferSerial.length();i++)
        {
           if((unsigned char)mainBufferSerial.at(i) == 0xAA)
           {
               index = i;
               break;
           }
        }
    }
    else
        index = -1;

    emit(serialError((index==-1 || index == 0)?mainBufferSerial:mainBufferSerial.left(index-1)));

    if(index == -1)
        mainBufferSerial.clear();
    else
        mainBufferSerial = mainBufferSerial.right(index-1);
}

void serialClass::sendCommand(QString preamble, QByteArray data)
{
    if(preamble.length() < 2)
        return;

    QByteArray commandData;
    commandData.append((char)0xAA);
    commandData.append((char)(data.length()));
    commandData.append((char)preamble.toLocal8Bit().at(0));
    commandData.append((char)preamble.toLocal8Bit().at(1));
    commandData.append(data);
    commandData.append((char)0xCC); //TODO calculate CRC

    serialMutex.lock();
    if(serialPort->isOpen())
    {
        serialPort->write(commandData);
    }
    serialMutex.unlock();
}

void serialClass::sendCommandWithCRCError(QString preamble, QByteArray data)
{
    if(preamble.length() < 2)
        return;

    QByteArray commandData;
    commandData.append((char)0xAA);
    commandData.append((char)(data.length()));
    commandData.append((char)preamble.toLocal8Bit().at(0));
    commandData.append((char)preamble.toLocal8Bit().at(1));
    commandData.append(data);
    commandData.append((char)0xCB); //TODO calculate CRC and remove 1

    serialMutex.lock();
    if(serialPort->isOpen())
    {
        serialPort->write(commandData);
    }
    serialMutex.unlock();
}

void serialClass::sendCommandWithLengthError(QString preamble, QByteArray data)
{
    if(preamble.length() < 2)
        return;

    QByteArray commandData;
    commandData.append((char)0xAA);
    commandData.append((char)(100));
    commandData.append((char)preamble.toLocal8Bit().at(0));
    commandData.append((char)preamble.toLocal8Bit().at(1));
    commandData.append(data);
    commandData.append((char)0xCC); //TODO calculate CRC

    serialMutex.lock();
    if(serialPort->isOpen())
    {
        serialPort->write(commandData);
    }
    serialMutex.unlock();
}

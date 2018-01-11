#ifndef SERIALCLASS_H
#define SERIALCLASS_H

#include <QWidget>
#include <QSerialPort>
#include <QMutex>

namespace Ui {
class serialClass;
}

class serialClass : public QWidget
{
    Q_OBJECT
public:
    explicit serialClass(QWidget *parent = 0);
    ~serialClass();

public slots:
    void sendCommand(QString preamble, QByteArray data);
    void sendCommandWithCRCError(QString preamble, QByteArray data);
    void sendCommandWithLengthError(QString preamble, QByteArray data);

private slots:
    void on_buttonRefresh_clicked();

    void on_buttonConnect_clicked();
    void serialDataReadyRead();

signals:
    void serialFrameReceived(QString preamble, QByteArray data);

    void serialError(QByteArray errorData);

    void serialStatusChange(bool state);

private:
    Ui::serialClass *ui;
    QSerialPort *serialPort;
    QByteArray mainBufferSerial;

    void skipBytesUntilAA();
    QMutex serialMutex;
};

#endif // SERIALCLASS_H

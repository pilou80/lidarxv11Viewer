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
    void sendData(QByteArray data);

private slots:
    void on_buttonRefresh_clicked();

    void on_buttonConnect_clicked();
    void serialDataReadyRead();

signals:
    void serialDataReceived(QByteArray data);
    void serialStatusChange(bool state);


private:
    Ui::serialClass *ui;
    QSerialPort *m_serialPort;
    QMutex serialMutex;
};

#endif // SERIALCLASS_H

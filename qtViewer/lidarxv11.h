#ifndef LIDARXV11_H
#define LIDARXV11_H

#include <QObject>

typedef struct {
    int dist; //en mm 0-6000mm
    int angle; //0-359
    float strenght;// 0.0-1.0
    bool valid;
    bool strenghtWarn;
}value360_t;

#define REPORT_EACH 0x01 //Report each record receive
#define REPORT_PACKET 0x02 //Report each packet (4 records)
#define REPORT_LASTPACKET 0x04 //Report when last packet receive (90 pacets 360 records)

class lidarXV11 : public QObject
{
    Q_OBJECT

public:
    explicit lidarXV11(QObject *parent = 0);
    QByteArray m_serialReceive;
    void setReportMode(int reportMode);
    QList<value360_t*> *m_values;

signals:
    void valueReceived(value360_t value);
    void speedReceived(float speed);
    void packetReceive(int packet);
    void lastPacketReceive();


public slots:
    void serialReceive(QByteArray data);
    void serialReceive();

    void sendModeOff();
    void sendModeOn();
    void sendModeSpin(uint speed);

private:
    float m_speed;
    void convertReadingToAngle(int angle, QByteArray packet);
    bool m_reportSpeed;
    bool m_reportEach;
    bool m_reportPacket;
    bool m_reportLastPacket;
    void sendLidarCommand(quint8 command, quint16 speed);
};

#endif // LIDARXV11_H

#include "lidarxv11.h"
//http://xv11hacking.wikispaces.com/LIDAR+Sensor
lidarXV11::lidarXV11(QObject *parent) : QObject(parent)
{
    m_values = NULL;
}

void lidarXV11::serialReceive(QByteArray data)
{
    m_serialReceive.append(data);

    m_reportSpeed = false;
    m_reportEach = false;
    m_reportPacket = false;
    m_reportLastPacket = false;
}

void lidarXV11::serialReceive()
{
    while(m_serialReceive.count() >= 22)
    {
        if((quint8)m_serialReceive.at(0) == 0xFA)
        {
            /*quint16 checksum = ((quint8)m_serialReceive.at(21))<<8;
            checksum += (quint8)m_serialReceive.at(20);*/
            //TODO checksum
            {
                int currentIndex = ((quint8)m_serialReceive.at(1)-0xA0)*90;

                //Speed in RPM
                m_speed = ((float)((((quint16)m_serialReceive.at(2))<<8) + (quint16)m_serialReceive.at(1)))/64;
                if(m_reportSpeed)
                    emit speedReceived(m_speed);
                convertReadingToAngle(currentIndex++, m_serialReceive.mid(4,4));
                convertReadingToAngle(currentIndex++, m_serialReceive.mid(8,4));
                convertReadingToAngle(currentIndex++, m_serialReceive.mid(12,4));
                convertReadingToAngle(currentIndex++, m_serialReceive.mid(16,4));
                if(m_reportPacket)
                    emit(packetReceive(currentIndex-4));
                if((currentIndex == 360) &&
                   m_reportLastPacket)
                    emit lastPacketReceive();
            }
            m_serialReceive.remove(0,22);
        }
        else
        {
            m_serialReceive.remove(0,1);
        }
    }
}

void lidarXV11::sendLidarCommand(quint8 command, quint16 speed)
{
    QByteArray commandSerial;
    commandSerial.append(0xAA);
    commandSerial.append((((quint16)command&0x03)<<6) + (speed&0x3FFF)<<8);
    commandSerial.append(speed&0xFF);
    commandSerial.append(0x55);

    //emit send commandSerial
}

void lidarXV11::sendModeOff()
{
    sendLidarCommand(0x00, 0);
}

void lidarXV11::sendModeOn()
{
    sendLidarCommand(0x01, 0);
}

void lidarXV11::sendModeSpin(uint speed)
{
    sendLidarCommand(0x02, speed);
}

void lidarXV11::convertReadingToAngle(int angle, QByteArray packet)
{
    value360_t *value = NULL;
    bool freeValue;
    if(m_values != NULL)
    {
        if(m_values->count() > (angle+1))
        {
            value = m_values->at(angle);
            freeValue = false;
        }
    }
    if(value == NULL)
    {
        value = new value360_t;
        freeValue = true;
    }

    value->angle = angle;
    value->dist = (quint16)packet.at(0) + (((quint16)packet.at(1) & 0x3F)<<8);
    value->valid = (((quint8)packet.at(1) & 0x40 ) == 0x40);
    value->strenghtWarn = (((quint8)packet.at(1) & 0x80 ) == 0x80);
    value->strenght = (quint16)packet.at(2) + ((quint16)packet.at(3)<<8);

    if(m_reportEach)
        emit(valueReceived(*value));
    if(freeValue)
        delete value;
}

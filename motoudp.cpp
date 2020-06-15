#include "motoudp.h"
#include <QUdpSocket>


MotoUDP::MotoUDP(QHostAddress h,quint16 p)
{
    _HostAddress = h;
    _port = p;
    rx_buffer = new QByteArray;
}
MotoUDP::~MotoUDP()
{
    delete rx_buffer;
}
bool MotoUDP::MotoUDP::ConnectMotoman()
{
    client = new QUdpSocket;
    client->bind();
    return 1;
}
bool MotoUDP::MotoUDP::CloseMotoman()
{
    client->close();
    delete  client;
    return 1;
}
QString MotoUDP::MotoUDP::SendCommand(QByteArray buffer)
{
    isDataReceive = false;
    client->writeDatagram(buffer,buffer.length(),_HostAddress,_port);
    return ByteArray2Hex(buffer);
}

QString MotoUDP::MotoUDP::ByteArray2Hex(QByteArray buffer)
{
    QString s;
    for (int i = 0; i < buffer.size(); i++)
    {
        QString c = QString::number(buffer.at(i),16);
        if(c.size()<2)
        {
            s.push_back('0');
            s.push_back(c);
        }
        else{
            s.push_back(c.at(c.size()-2));
            s.push_back(c.at(c.size()-1));
        }
        s.push_back(' ');
    }
    s = s.toUpper();
    return s;
}

QByteArray MotoUDP::MotoUDP::Hex2ByteArray (QString s)
{
    QByteArray buffer;
    for (int i = 0; i < s.size(); i = i+3)
    {
        QString temp;
        temp.push_back(s.at(i));
        temp.push_back(s.at(i+1));
        bool ok;
        char x = temp.toInt(&ok,16);
        if(ok == 1)
        {
            buffer.push_back(x);
        }
        else
            return 0;
    }
    return buffer;
}

QByteArray MotoUDP::MotoUDP::Int32ToByteArray (int32_t value)
{
    QByteArray buffer;
    for (uint i = 0;i < sizeof (int32_t); i++) {
        char u = (value>>8*i) & 0xFF;
        buffer.push_back(u);
    }
    return buffer;
}
int32_t MotoUDP::MotoUDP::ByteArray2Int32 (QByteArray* buffer,int start, int number)
{
    int32_t value = 0;
    for (int i = start;i < start+number; i++) {
        int32_t temp = (unsigned char) buffer->at(i) ;
        value += temp << 8*(i-start);
    }
    return value;

}
bool  MotoUDP::MotoUDP::TurnOnServo()
{

    SendCommand(Hex2ByteArray(ON_SERVO_CMD));
    tx_data = ON_SERVO_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::TurnOffServo()
{
    SendCommand(Hex2ByteArray(OFF_SERVO_CMD));
    tx_data = OFF_SERVO_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::GetPosition()
{
    SendCommand(Hex2ByteArray(GET_POS_CMD));
    tx_data = GET_POS_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::GetPulsePosition()
{
    SendCommand(Hex2ByteArray(GET_PULSE_CMD));
    tx_data = GET_PULSE_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::WritePosition(int32_t speed,int32_t X,int32_t Y,int32_t Z,int32_t RX,int32_t RY,int32_t RZ)
{
    QByteArray* position_cmd = new QByteArray;
    position_cmd->push_back(Hex2ByteArray(WRITE_POS_HEADER));
    position_cmd->push_back(Int32ToByteArray(speed));
    position_cmd->push_back(Hex2ByteArray("10 00 00 00"));
    position_cmd->push_back(Int32ToByteArray(X));
    position_cmd->push_back(Int32ToByteArray(Y));
    position_cmd->push_back(Int32ToByteArray(Z));
    position_cmd->push_back(Int32ToByteArray(RX));
    position_cmd->push_back(Int32ToByteArray(RY));
    position_cmd->push_back(Int32ToByteArray(RZ));
    position_cmd->push_back(Hex2ByteArray(WRITE_POS_END));
    SendCommand(*position_cmd);
    tx_data = ByteArray2Hex(*position_cmd);
    delete  position_cmd;
    return 1;
}
bool MotoUDP::MotoUDP::WritePulse(int32_t speed,int32_t R1,int32_t R2,int32_t R3,int32_t R4,int32_t R5,int32_t R6)
{
    QByteArray* position_cmd = new QByteArray;
    position_cmd->push_back(Hex2ByteArray(WRITE_PULSE_HEADER));
    position_cmd->push_back(Int32ToByteArray(speed));
    position_cmd->push_back(Int32ToByteArray(R1));
    position_cmd->push_back(Int32ToByteArray(R2));
    position_cmd->push_back(Int32ToByteArray(R3));
    position_cmd->push_back(Int32ToByteArray(R4));
    position_cmd->push_back(Int32ToByteArray(R5));
    position_cmd->push_back(Int32ToByteArray(R6));
    position_cmd->push_back(Hex2ByteArray(WRITE_PULSE_END));
    SendCommand(*position_cmd);
    tx_data = ByteArray2Hex(*position_cmd);
    delete  position_cmd;
    return 1;
}
void MotoUDP::MotoUDP::ReceiveData()
{
    rx_buffer->resize(client->pendingDatagramSize());
    client->readDatagram(rx_buffer->data(),rx_buffer->size());
    rx_data = ByteArray2Hex(*rx_buffer);
    isDataReceive = true;
}
QByteArray* MotoUDP::MotoUDP::Get_rx_buffer()
{
    return rx_buffer;
}

const QString MotoUDP::MotoUDP::ON_SERVO_CMD =  "59 45 52 43 20 00 04 00 03 01 00 00 00 00 00 00 39 39 39 39 39 39 39 39 83 00 02 00 01 10 00 00 01 00 00 00";
const QString MotoUDP::MotoUDP::OFF_SERVO_CMD = "59 45 52 43 20 00 04 00 03 01 00 01 00 00 00 00 39 39 39 39 39 39 39 39 83 00 02 00 01 10 00 00 02 00 00 00";
const QString MotoUDP::MotoUDP::GET_POS_CMD =   "59 45 52 43 20 00 00 00 03 01 00 02 00 00 00 00 39 39 39 39 39 39 39 39 75 00 65 00 00 01 00 00";
const QString MotoUDP::MotoUDP::GET_PULSE_CMD = "59 45 52 43 20 00 00 00 03 01 00 03 00 00 00 00 39 39 39 39 39 39 39 39 75 00 01 00 00 01 00 00";
const QString MotoUDP::MotoUDP::WRITE_POS_HEADER = "59 45 52 43 20 00 68 00 03 01 00 04 00 00 00 00 39 39 39 39 39 39 39 39 8A 00 01 00 01 02 00 00 01 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_POS_END = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_PULSE_HEADER = "59 45 52 43 20 00 58 00 03 01 00 05 00 00 00 00 39 39 39 39 39 39 39 39 8B 00 01 00 01 02 00 00 01 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_PULSE_END ="00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";

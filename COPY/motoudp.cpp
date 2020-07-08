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
bool MotoUDP::MotoUDP::GetRobotStatus()
{
    SendCommand(Hex2ByteArray(GET_STATUS));
    tx_data = GET_STATUS;
    return true;
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
MotoUDP::MotoUDP::RECEIVE_TYPE MotoUDP::MotoUDP::GetReceiveType (QByteArray buffer)
{
    return RECEIVE_TYPE(buffer.at(11));
}
bool MotoUDP::MotoUDP::CheckReceivedData(QByteArray buffer)
{
    QByteArray abuffer;
    bool check_number_data, check_header_1, check_header_2, check_header_3 ;
    abuffer = SplitArray(buffer,6,2);
    check_number_data = ByteArray2Int32(&abuffer,0,2)==Get_rx_buffer()->size()-32;
    check_header_1 = ByteArray2Hex(SplitArray(buffer,0,6)) == "59 45 52 43 20 00 ";
    check_header_2 = ByteArray2Hex(SplitArray(buffer,8,3)) == "03 01 01 ";
    check_header_3 = ByteArray2Hex(SplitArray(buffer,12,12)) == "00 00 00 80 39 39 39 39 39 39 39 39 ";
    return  check_number_data&&check_header_1&&check_header_2&&check_header_3;
}
QByteArray MotoUDP::MotoUDP::SplitArray(QByteArray array,int start, int count)
{
    QByteArray temp_array;
    for (int i = start;i<start+count;i++) {
        temp_array.push_back(array.at(i));
    }
    return temp_array;
}
int32_t MotoUDP::MotoUDP::Joint2Pulse(double joint)
{
    return PULSE_PER_DEGREE*joint;
}
double MotoUDP::MotoUDP::Pulse2Joint(int32_t pulse)
{
    return pulse/PULSE_PER_DEGREE;
}
bool MotoUDP::MotoUDP::GetVarPosition()
{
  SendCommand(Hex2ByteArray(READ_VARIABLE_POS));
  return true;
}
bool MotoUDP::MotoUDP::WriteVarPosition(int32_t X,int32_t Y,int32_t Z,int32_t RX,int32_t RY,int32_t RZ)
{
  QByteArray* position_var = new QByteArray;
  position_var->push_back(Hex2ByteArray(WRITE_VARIABLE_POS));
  position_var->push_back(Int32ToByteArray(X));
  position_var->push_back(Int32ToByteArray(Y));
  position_var->push_back(Int32ToByteArray(Z));
  position_var->push_back(Int32ToByteArray(RX));
  position_var->push_back(Int32ToByteArray(RY));
  position_var->push_back(Int32ToByteArray(RZ));
  position_var->push_back(Int32ToByteArray(0));
  position_var->push_back(Int32ToByteArray(0));
  SendCommand(*position_var);
  tx_data = ByteArray2Hex(*position_var);
  delete  position_var;
  return 1;
}
bool MotoUDP::MotoUDP::SelectJob(QString jobname){
  jobname.resize(32);
  QByteArray buffer;
  buffer.push_back(Hex2ByteArray(SELECT_JOB));
  for (int i = 0;i<32;i++) {
     buffer.push_back(jobname.at(i).toAscii());
  }
  buffer.push_back(Int32ToByteArray(0));
  SendCommand(buffer);
  return 1;
}
bool MotoUDP::MotoUDP::StartJob(){
  SendCommand(Hex2ByteArray(START_JOB));
  return 1;
}
bool MotoUDP::MotoUDP::OutputON()
{
   SendCommand(Hex2ByteArray(OUTPUT));
    return 1;
}
bool MotoUDP::MotoUDP::OutputOFF()
{
   SendCommand(Hex2ByteArray(OUTPUT));
    return 1;
}
const QString MotoUDP::MotoUDP::ON_SERVO_CMD =  "59 45 52 43 20 00 04 00 03 01 00 00 00 00 00 00 39 39 39 39 39 39 39 39 83 00 02 00 01 10 00 00 01 00 00 00";
const QString MotoUDP::MotoUDP::OFF_SERVO_CMD = "59 45 52 43 20 00 04 00 03 01 00 01 00 00 00 00 39 39 39 39 39 39 39 39 83 00 02 00 01 10 00 00 02 00 00 00";
const QString MotoUDP::MotoUDP::GET_POS_CMD =   "59 45 52 43 20 00 00 00 03 01 00 02 00 00 00 00 39 39 39 39 39 39 39 39 75 00 65 00 00 01 00 00";
const QString MotoUDP::MotoUDP::GET_PULSE_CMD = "59 45 52 43 20 00 00 00 03 01 00 03 00 00 00 00 39 39 39 39 39 39 39 39 75 00 01 00 00 01 00 00";
const QString MotoUDP::MotoUDP::WRITE_POS_HEADER = "59 45 52 43 20 00 68 00 03 01 00 04 00 00 00 00 39 39 39 39 39 39 39 39 8A 00 01 00 01 02 00 00 01 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_POS_END = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_PULSE_HEADER = "59 45 52 43 20 00 58 00 03 01 00 05 00 00 00 00 39 39 39 39 39 39 39 39 8B 00 01 00 01 02 00 00 01 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_PULSE_END ="00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::GET_STATUS = "59 45 52 43 20 00 00 00 03 01 00 10 00 00 00 00 39 39 39 39 39 39 39 39 72 00 01 00 00 0E 00 00";
const double MotoUDP::MotoUDP::PULSE_PER_DEGREE = 34816/30;
const QString MotoUDP::MotoUDP::WRITE_VARIABLE_POS = "59 45 52 43 20 00 34 00 03 01 00 06 00 00 00 00 39 39 39 39 39 39 39 39 7F 00 20 00 00 10 00 00 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::READ_VARIABLE_POS = "59 45 52 43 20 00 00 00 03 01 00 07 00 00 00 00 39 39 39 39 39 39 39 39 7F 00 20 00 00 0E 00 00";
const QString MotoUDP::MotoUDP::SELECT_JOB = "59 45 52 43 20 00 24 00 03 01 00 08 00 00 00 00 39 39 39 39 39 39 39 39 87 00 00 00 01 02 00 00";
const QString MotoUDP::MotoUDP::START_JOB = "59 45 52 43 20 00 04 00 03 01 00 09 00 00 00 00 39 39 39 39 39 39 39 39 86 00 01 00 01 10 00 00 01 00 00 00";
const QString MotoUDP::MotoUDP::OUTPUT = "59 45 52 43 20 00 01 00 03 01 00 10 00 00 00 00 39 39 39 39 39 39 39 39 78 00 E9 03 01 10 00 00";
     //   "59 45 52 43 20 00 01 00 03 01 00 10 00 00 00 00 39 39 39 39 39 39 39 39 78 00 E9 03 01 10 00 00";
//const int32_t MotoUDP::MotoUDP::S_MAX = 197291;
//const int32_t MotoUDP::MotoUDP::L_MAX = 102400;
//const int32_t MotoUDP::MotoUDP::L_MIN = -96712;
//const int32_t MotoUDP::MotoUDP::U_MAX = 51200;
//const int32_t MotoUDP::MotoUDP::U_MIN = -28445;
//const int32_t MotoUDP::MotoUDP::R_MAX = 47787;
//const int32_t MotoUDP::MotoUDP::B_MAX = 71680;
//const int32_t MotoUDP::MotoUDP::B_MIN = -10240;
//const int32_t MotoUDP::MotoUDP::T_MAX = 122880;
//const double MotoUDP::MotoUDP::JS_MAX = 170;
//const double MotoUDP::MotoUDP::JL_MAX = 90;
//const double MotoUDP::MotoUDP::JL_MIN = -85;
//const double MotoUDP::MotoUDP::JU_MAX = 90;
//const double MotoUDP::MotoUDP::JU_MIN = -50;
//const double MotoUDP::MotoUDP::JR_MAX = 140;
//const double MotoUDP::MotoUDP::JB_MAX = 210;
//const double MotoUDP::MotoUDP::JB_MIN = -30;
//const double MotoUDP::MotoUDP::JT_MAX = 360;

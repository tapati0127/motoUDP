#include "motoudp.h"
#include <QUdpSocket>
#include <QString>
#include <QByteArray>

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
struct MotoUDP::MotoUDP::TxData {
    const char identifier[4] = {'Y','E','R','C'};
    const u_int16_t header_size = 32;
    u_int16_t data_size;
    const u_int8_t reserve1 = 3;
    const u_int8_t processing_division = 1;
    const u_int8_t ack = 0;
    u_int8_t id;
    const u_int32_t block_no = 0;
    const char reserve2[8] = {'9','9','9','9','9','9','9','9'};
    u_int16_t command_no;
    u_int16_t instance;
    u_int8_t attribute;
    u_int8_t service;
    const u_int16_t padding = 0;
   // QByteArray data;
};
struct MotoUDP::MotoUDP::TxDataWritePosition {
  const u_int32_t control_group_robot = 1;
  const u_int32_t control_group_station = 0;
  const u_int32_t classification_in_speed = 0;
  u_int32_t speed;
  const u_int32_t coordinate = 0x10;
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t rx;
  int32_t ry;
  int32_t rz;
  const u_int32_t reservation1 = 0;
  const u_int32_t reservation2 = 0;
  const u_int32_t type = 0;
  const u_int32_t expanded_type = 0;
  const u_int32_t tool_no = 0;
  const u_int32_t user_coordinate_no = 0;
  const u_int32_t base_1_position = 0;
  const u_int32_t base_2_position = 0;
  const u_int32_t base_3_position = 0;
  const u_int32_t station_1_position = 0;
  const u_int32_t station_2_position = 0;
  const u_int32_t station_3_position = 0;
  const u_int32_t station_4_position = 0;
  const u_int32_t station_5_position = 0;
  const u_int32_t station_6_position = 0;
};
struct MotoUDP::MotoUDP::TxDataWritePulse {
  const u_int32_t control_group_robot = 1;
  const u_int32_t control_group_station = 0;
  const u_int32_t classification_in_speed = 0;
  u_int32_t speed;
  int32_t r1;
  int32_t r2;
  int32_t r3;
  int32_t r4;
  int32_t r5;
  int32_t r6;
  int32_t r7;
  int32_t r8;
  const u_int32_t tool_no = 0;
  const u_int32_t base_1_position = 0;
  const u_int32_t base_2_position = 0;
  const u_int32_t base_3_position = 0;
  const u_int32_t station_1_position = 0;
  const u_int32_t station_2_position = 0;
  const u_int32_t station_3_position = 0;
  const u_int32_t station_4_position = 0;
  const u_int32_t station_5_position = 0;
  const u_int32_t station_6_position = 0;
};
struct MotoUDP::MotoUDP::TxDataWriteVariablePosition
{
  u_int32_t data_type;
  const u_int32_t figure = 0;
  const u_int32_t tool_no = 0;
  const u_int32_t user_coodirnate_no = 0;
  const u_int32_t extended_figure = 0;
  int32_t first_axis_position;
  int32_t second_axis_position;
  int32_t third_axis_position;
  int32_t fourth_axis_position;
  int32_t fifth_axis_position;
  int32_t sixth_axis_position;
  const int32_t seventh_axis_position = 0;
  const int32_t eighth_axis_position = 0;
};

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
bool MotoUDP::MotoUDP::SendData (char* buffer, int lenght)
{
  isDataReceive = false;
  client->writeDatagram(buffer,lenght,_HostAddress,_port);
  return 1;
}
bool  MotoUDP::MotoUDP::TurnOnServo()
{
    TxData sent_data;
    u_int32_t data = 1;
    sent_data.id = RECEIVE_TYPE::ON_SERVO;
    sent_data.command_no = 0x83;
    sent_data.instance = 2;
    sent_data.attribute = 1;
    sent_data.service = 0x10;
    u_int16_t data_length = sizeof (data);
    u_int16_t total_length = 32 + data_length;
    sent_data.data_size = data_length;
    char* buffer = new char[total_length];
    memcpy(buffer,&sent_data,32);
    memcpy(buffer+32,&data,data_length);
    SendData(buffer,total_length);
    delete [] buffer;
//    SendCommand(Hex2ByteArray(ON_SERVO_CMD));
//    tx_data = ON_SERVO_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::TurnOffServo()
{
  TxData sent_data;
  u_int32_t data = 2;
  sent_data.id = RECEIVE_TYPE::OFF_SERVO;
  sent_data.command_no = 0x83;
  sent_data.instance = 2;
  sent_data.attribute = 1;
  sent_data.service = 0x10;
  u_int16_t data_length = sizeof (data);
  u_int16_t total_length = 32 + data_length;
  sent_data.data_size = data_length;
  char* buffer = new char[total_length];
  memcpy(buffer,&sent_data,32);
  memcpy(buffer+32,&data,data_length);
  SendData(buffer,total_length);
  delete [] buffer;
//    SendCommand(Hex2ByteArray(OFF_SERVO_CMD));
//    tx_data = OFF_SERVO_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::GetPosition()
{
  TxData sent_data;
  char* buffer = new char[sizeof (sent_data)];
  sent_data.id = RECEIVE_TYPE::GET_POSITION;
  sent_data.command_no = 0x75;
  sent_data.instance = 0x65;
  sent_data.attribute = 0;
  sent_data.service = 0x01;
  sent_data.data_size = 0;
//  sent_data.data.resize(0);
  memcpy(buffer,&sent_data,sizeof (sent_data));
  SendData(buffer,sizeof (sent_data));
  delete [] buffer;
//    SendCommand(Hex2ByteArray(GET_POS_CMD));
//    tx_data = GET_POS_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::GetPulsePosition()
{
  TxData sent_data;
  char* buffer = new char[sizeof (sent_data)];
  sent_data.id = RECEIVE_TYPE::GET_PULSE;
  sent_data.command_no = 0x75;
  sent_data.instance = 0x01;
  sent_data.attribute = 0;
  sent_data.service = 0x01;
  sent_data.data_size = 0;

  memcpy(buffer,&sent_data,sizeof (sent_data));
  SendData(buffer,sizeof (sent_data));
  delete [] buffer;
//    SendCommand(Hex2ByteArray(GET_PULSE_CMD));
//    tx_data = GET_PULSE_CMD;
    return 1;
}
bool MotoUDP::MotoUDP::WritePosition(int32_t speed,int32_t X,int32_t Y,int32_t Z,int32_t RX,int32_t RY,int32_t RZ)
{
  TxData sent_data;
  sent_data.id = RECEIVE_TYPE::WRITE_POSITION;
  sent_data.command_no = 0x8A;
  sent_data.instance = 0x01;
  sent_data.attribute = 01;
  sent_data.service = 0x02;

  TxDataWritePosition position;
  position.x = X;
  position.y = Y;
  position.z = Z;
  position.rx = RX;
  position.ry = RY;
  position.rz = RZ;
  position.speed = speed;

  sent_data.data_size = sizeof(position);
  char* buffer = new char[sizeof(sent_data)+sizeof (position)];
  memcpy(buffer,&sent_data,sizeof (sent_data));
  memcpy(buffer+sizeof (sent_data),&position,sizeof (position));
  SendData(buffer,sizeof(sent_data)+sizeof (position));
  delete [] buffer;
//    QByteArray* position_cmd = new QByteArray;
//    position_cmd->push_back(Hex2ByteArray(WRITE_POS_HEADER));
//    position_cmd->push_back(Int32ToByteArray(speed));
//    position_cmd->push_back(Hex2ByteArray("10 00 00 00"));
//    position_cmd->push_back(Int32ToByteArray(X));
//    position_cmd->push_back(Int32ToByteArray(Y));
//    position_cmd->push_back(Int32ToByteArray(Z));
//    position_cmd->push_back(Int32ToByteArray(RX));
//    position_cmd->push_back(Int32ToByteArray(RY));
//    position_cmd->push_back(Int32ToByteArray(RZ));
//    position_cmd->push_back(Hex2ByteArray(WRITE_POS_END));
//    SendCommand(*position_cmd);
//    tx_data = ByteArray2Hex(*position_cmd);
//    delete  position_cmd;
    return 1;
}
bool MotoUDP::MotoUDP::WritePulse(int32_t speed,int32_t R1,int32_t R2,int32_t R3,int32_t R4,int32_t R5,int32_t R6)
{
  TxData sent_data;
  sent_data.id = RECEIVE_TYPE::WRITE_PUSLE;
  sent_data.command_no = 0x8B;
  sent_data.instance = 0x01;
  sent_data.attribute = 01;
  sent_data.service = 0x02;

  TxDataWritePulse position;
  position.r1 = R1;
  position.r2 = R2;
  position.r3 = R3;
  position.r4 = R4;
  position.r5 = R5;
  position.r6 = R6;
  position.speed = speed;

  sent_data.data_size = sizeof(position);
  char* buffer = new char[sizeof(sent_data)+sizeof (position)];
  memcpy(buffer,&sent_data,sizeof (sent_data));
  memcpy(buffer+sizeof (sent_data),&position,sizeof (position));
  SendData(buffer,sizeof(sent_data)+sizeof (position));
  delete [] buffer;
//    QByteArray* position_cmd = new QByteArray;
//    position_cmd->push_back(Hex2ByteArray(WRITE_PULSE_HEADER));
//    position_cmd->push_back(Int32ToByteArray(speed));
//    position_cmd->push_back(Int32ToByteArray(R1));
//    position_cmd->push_back(Int32ToByteArray(R2));
//    position_cmd->push_back(Int32ToByteArray(R3));
//    position_cmd->push_back(Int32ToByteArray(R4));
//    position_cmd->push_back(Int32ToByteArray(R5));
//    position_cmd->push_back(Int32ToByteArray(R6));
//    position_cmd->push_back(Hex2ByteArray(WRITE_PULSE_END));
//    SendCommand(*position_cmd);
//    tx_data = ByteArray2Hex(*position_cmd);
//    delete  position_cmd;
    return 1;
}
bool MotoUDP::MotoUDP::GetVarPosition(u_int16_t index)
{
  TxData sent_data;
  char* buffer = new char[sizeof (sent_data)];
  sent_data.id = 06;
  sent_data.command_no = 0x7f;
  sent_data.instance = index;
  sent_data.attribute = 0;
  sent_data.service = 0x0e;
  sent_data.data_size = 0;

  memcpy(buffer,&sent_data,sizeof (sent_data));
  SendData(buffer,sizeof (sent_data));
  delete [] buffer;
//  SendCommand(Hex2ByteArray(READ_VARIABLE_POS));
  return true;
}
bool MotoUDP::MotoUDP::WriteVarPosition(u_int16_t index, int32_t X,int32_t Y,int32_t Z,int32_t RX,int32_t RY,int32_t RZ)
{
  TxData sent_data;

  sent_data.id = 07;
  sent_data.command_no = 0x7f;
  sent_data.instance = index;
  sent_data.attribute = 0;
  sent_data.service = 0x10;
  TxDataWriteVariablePosition position;
  sent_data.data_size = sizeof (position);
  position.data_type = 17;
  position.first_axis_position = X;
  position.second_axis_position = Y;
  position.third_axis_position = Z;
  position.fourth_axis_position = RX;
  position.fifth_axis_position = RY;
  position.sixth_axis_position = RZ;
  char* buffer = new char[sizeof(sent_data)+ sizeof (position)];
  memcpy(buffer,&sent_data,sizeof (sent_data));
  memcpy(buffer+sizeof(sent_data),&position,sizeof(position));
  SendData(buffer,sizeof(sent_data)+ sizeof (position));
  delete [] buffer;
//  QByteArray* position_var = new QByteArray;
//  position_var->push_back(Hex2ByteArray(WRITE_VARIABLE_POS));
//  position_var->push_back(Int32ToByteArray(X));
//  position_var->push_back(Int32ToByteArray(Y));
//  position_var->push_back(Int32ToByteArray(Z));
//  position_var->push_back(Int32ToByteArray(RX));
//  position_var->push_back(Int32ToByteArray(RY));
//  position_var->push_back(Int32ToByteArray(RZ));
//  position_var->push_back(Int32ToByteArray(0));
//  position_var->push_back(Int32ToByteArray(0));
//  SendCommand(*position_var);
//  tx_data = ByteArray2Hex(*position_var);
//  delete  position_var;
  return 1;
}
bool MotoUDP::MotoUDP::WriteOutput(u_int8_t state)
{

    TxData sent_data;
    sent_data.id = 8;
    sent_data.command_no = 0x78;
    sent_data.instance = 2701;
    sent_data.attribute = 1;
    sent_data.service = 0x0E;
    u_int16_t data_length = sizeof (state);
    u_int16_t total_length = 32 + data_length;
    sent_data.data_size = data_length;
    char* buffer = new char[total_length];
    memcpy(buffer,&sent_data,32);
    memcpy(buffer+32,&state,data_length);
    SendData(buffer,total_length);
    delete [] buffer;
    return 1;
}

bool MotoUDP::MotoUDP::SelectJob(char* jobname){
    u_int32_t line_number=0;
    TxData sent_data;
    sent_data.id = 9;
    sent_data.command_no = 0x87;
    sent_data.instance = 1;
    sent_data.attribute = 1;
    sent_data.service = 0x02;
    u_int16_t data_length = 36;
    u_int16_t total_length = 32 + data_length;
    sent_data.data_size = data_length;
    char* buffer = new char[total_length];

    memcpy(buffer,&sent_data,32);
    memcpy(buffer+32,jobname,32);
    memcpy(buffer+32+32,&line_number,4);
    SendData(buffer,total_length);
    delete [] buffer;
    return 1;
//  jobname.resize(32);
//  QByteArray buffer;
//  buffer.push_back(Hex2ByteArray(SELECT_JOB));
//  for (int i = 0;i<32;i++) {
//     buffer.push_back(jobname.at(i).toAscii());
//  }
//  buffer.push_back(Int32ToByteArray(0));
//  SendCommand(buffer);
  return 1;
}
bool MotoUDP::MotoUDP::StartJob(){
  SendCommand(Hex2ByteArray(START_JOB));
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
int32_t MotoUDP::MotoUDP::Joint2Pulse(double joint, int i)
{

    if(i==0)
    {
      return int32_t(PULSE_PER_DEGREE_S*joint);
    }
    else if (i==1) {
      return int32_t(PULSE_PER_DEGREE_L*joint);
    }
    else if (i==2) {
      return int32_t(PULSE_PER_DEGREE_U*joint);
    }
    else {
      return int32_t(PULSE_PER_DEGREE_RBT*joint);
    }
}
double MotoUDP::MotoUDP::Pulse2Joint(int32_t pulse, int i)
{
  if(i==0)
  {
    return double(pulse)/PULSE_PER_DEGREE_S;
  }
  else if (i==1) {
    return double(pulse)/PULSE_PER_DEGREE_L;
  }
  else if (i==2) {
    return double(pulse)/PULSE_PER_DEGREE_U;
  }
  else {
    return double(pulse)/PULSE_PER_DEGREE_RBT;
  }
}
//std::vector<double> MotoUDP::MotoUDP::ByteArray2Position(QByteArray* pos_buffer)
//{
//    double pos[6];
//    for (int i = 0; i < 6;i = i+4) {
//      pos[i] = ByteArray2Int32(rx_buffer,4*i+52,4);
//    }
//    return pos;
//}
std::vector<double> MotoUDP::MotoUDP::ByteArray2Joint(QByteArray *pulse_buffer)
{
  std::vector<double> pulse;
  for (int i = 0; i < 6;i++) {
    pulse.push_back(Pulse2Joint(ByteArray2Int32(pulse_buffer,4*i+52,4),i)*3.1415926536/180);
  }
  return pulse;
}
const QString MotoUDP::MotoUDP::ON_SERVO_CMD =  "59 45 52 43 20 00 04 00 03 01 00 00 00 00 00 00 39 39 39 39 39 39 39 39 83 00 02 00 01 10 00 00 01 00 00 00";
const QString MotoUDP::MotoUDP::OFF_SERVO_CMD = "59 45 52 43 20 00 04 00 03 01 00 01 00 00 00 00 39 39 39 39 39 39 39 39 83 00 02 00 01 10 00 00 02 00 00 00";
const QString MotoUDP::MotoUDP::GET_POS_CMD =   "59 45 52 43 20 00 00 00 03 01 00 02 00 00 00 00 39 39 39 39 39 39 39 39 75 00 65 00 00 01 00 00";
const QString MotoUDP::MotoUDP::GET_PULSE_CMD = "59 45 52 43 20 00 00 00 03 01 00 03 00 00 00 00 39 39 39 39 39 39 39 39 75 00 01 00 00 01 00 00";
const QString MotoUDP::MotoUDP::WRITE_POS_HEADER = "59 45 52 43 20 00 68 00 03 01 00 04 00 00 00 00 39 39 39 39 39 39 39 39 8A 00 01 00 01 02 00 00 01 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_POS_END = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_PULSE_HEADER = "59 45 52 43 20 00 58 00 03 01 00 05 00 00 00 00 39 39 39 39 39 39 39 39 8B 00 01 00 01 02 00 00 01 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_PULSE_END ="00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::WRITE_VARIABLE_POS = "59 45 52 43 20 00 34 00 03 01 00 06 00 00 00 00 39 39 39 39 39 39 39 39 7F 00 20 00 11 02 00 00 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
const QString MotoUDP::MotoUDP::READ_VARIABLE_POS = "59 45 52 43 20 00 00 00 03 01 00 07 00 00 00 00 39 39 39 39 39 39 39 39 7F 00 20 00 11 01 00 00";
const QString MotoUDP::MotoUDP::SELECT_JOB = "59 45 52 43 20 00 24 00 03 01 00 08 00 00 00 00 39 39 39 39 39 39 39 39 87 00 01 00 01 02 00 00";
const QString MotoUDP::MotoUDP::START_JOB = "59 45 52 43 20 00 04 00 03 01 00 09 00 00 00 00 39 39 39 39 39 39 39 39 86 00 01 00 01 10 00 00 01 00 00 00";
const double MotoUDP::MotoUDP::PULSE_PER_DEGREE_S = 34816/30;
const double MotoUDP::MotoUDP::PULSE_PER_DEGREE_L = 102400/90;
const double MotoUDP::MotoUDP::PULSE_PER_DEGREE_U = 51200/90;
const double MotoUDP::MotoUDP::PULSE_PER_DEGREE_RBT = 10204/30;


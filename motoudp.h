#ifndef MOTOUDP_H
#define MOTOUDP_H

#include <QUdpSocket>

class MotoUDP : public QObject
{

public:
    MotoUDP(QHostAddress h,quint16 p);
    enum RECEIVE_TYPE  {ON_SERVO = 0x00, OFF_SERVO = 0x01, GET_POSITION = 0x02, GET_PULSE = 0x03, WRITE_POSITION = 0x04, WRITE_PUSLE = 0x05};
    ~MotoUDP();
    bool SendData (char* buffer, int lenght);
    bool ConnectMotoman();
    bool CloseMotoman();
    bool TurnOnServo();
    bool TurnOffServo();
    bool GetPosition();
    //std::vector<double> ByteArray2Position(QByteArray* pos_buffer);
    bool GetPulsePosition();
    bool GetVarPosition(u_int16_t index);
    bool SelectJob(char* jobname);
    bool StartJob();

    std::vector<double> ByteArray2Joint(QByteArray *pulse_buffer);
    bool WritePosition(int32_t speed,int32_t X,int32_t Y,int32_t Z,int32_t RX,int32_t RY,int32_t RZ);
    bool WritePulse(int32_t speed,int32_t R1,int32_t R2,int32_t R3,int32_t R4,int32_t R5,int32_t R6);
    bool WriteVarPosition(u_int16_t index, int32_t X,int32_t Y,int32_t Z,int32_t RX,int32_t RY,int32_t RZ);
    bool WriteOutput(u_int8_t state);

    RECEIVE_TYPE GetReceiveType (QByteArray buffer);
    bool CheckReceivedData(QByteArray buffer);
    QByteArray SplitArray(QByteArray array,int start, int count);
    int32_t Joint2Pulse(double joint, int i);
    double Pulse2Joint(int32_t pulse, int i);

    QByteArray* Get_rx_buffer();
    QString SendCommand(QByteArray buffer);
    QString ByteArray2Hex(QByteArray buffer);
    QByteArray Hex2ByteArray (QString s);
    QByteArray Int32ToByteArray (int32_t value);
    int32_t ByteArray2Int32 (QByteArray* buffer,int start, int number);
    QString rx_data;
    QString tx_data;
    QUdpSocket* client;
    bool isDataReceive;
    void ReceiveData();

private:
    QHostAddress _HostAddress;
    quint16 _port;
    QByteArray* rx_buffer;
    static const QString ON_SERVO_CMD;
    static const QString OFF_SERVO_CMD;
    static const QString GET_POS_CMD;
    static const QString GET_PULSE_CMD;
    static const QString WRITE_POS_HEADER;
    static const QString WRITE_POS_END;
    static const QString WRITE_PULSE_HEADER;
    static const QString WRITE_PULSE_END;
    static const QString WRITE_VARIABLE_POS;
    static const QString READ_VARIABLE_POS;
    static const QString SELECT_JOB;
    static const QString START_JOB;

    static const double PULSE_PER_DEGREE_S;
    static const double PULSE_PER_DEGREE_L;
    static const double PULSE_PER_DEGREE_U;
    static const double PULSE_PER_DEGREE_RBT;

    struct TxData;
    struct TxDataWritePosition;
    struct TxDataWritePulse;
    struct TxDataWriteVariablePosition;
    struct RxData;

};

#endif // MOTOUDP_H

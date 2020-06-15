#ifndef MOTOUDP_H
#define MOTOUDP_H

#include <QUdpSocket>
class MotoUDP : public QObject
{

public:
    static const QString ON_SERVO_CMD;
    static const QString OFF_SERVO_CMD;
    static const QString GET_POS_CMD;
    static const QString GET_PULSE_CMD;
    static const QString WRITE_POS_HEADER;
    static const QString WRITE_POS_END;
    static const QString WRITE_PULSE_HEADER;
    static const QString WRITE_PULSE_END;
    MotoUDP(QHostAddress h,quint16 p);
    enum RECEIVE_TYPE  {ON_SERVO = 0x00, OFF_SERVO = 0x01, GET_POSITION = 0x02, GET_PULSE = 0x03, WRITE_POSITION = 0x04, WRITE_PUSLE = 0x05};
    ~MotoUDP();
    bool ConnectMotoman();
    bool CloseMotoman();
    bool TurnOnServo();
    bool TurnOffServo();
    bool GetPosition();
    bool GetPulsePosition();
    bool WritePosition(int32_t speed,int32_t X,int32_t Y,int32_t Z,int32_t RX,int32_t RY,int32_t RZ);
    bool WritePulse(int32_t speed,int32_t R1,int32_t R2,int32_t R3,int32_t R4,int32_t R5,int32_t R6);
    RECEIVE_TYPE GetReceiveType (QByteArray buffer);
    bool CheckReceivedData(QByteArray buffer);
    QByteArray SplitArray(QByteArray array,int start, int count);

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

public slots:
    void ReceiveData();

private:
    QHostAddress _HostAddress;
    quint16 _port;
    QByteArray* rx_buffer;

};

#endif // MOTOUDP_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "motoudp.h"
#include <time.h>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonConnect_clicked()
{
    QHostAddress h;
    if(ui->pushButtonConnect->text()=="CONNECT")
    {
       ui->pushButtonConnect->setText("DISCONNECT");
       ui->groupBox_2->setEnabled(true);
       ui->groupBoxGoalPos->setEnabled(true);
       ui->groupGoalPulse->setEnabled(true);
       ui->groupBoxVel->setEnabled(true);
       if(ui->comboBoxIpAddress->currentText()=="LocalHost")
       {
           h = QHostAddress::LocalHost;
       }
       else {
           h.setAddress(ui->comboBoxIpAddress->currentText());
       }
    quint16 p = ui->textEditport->toPlainText().toUInt();
    socket = new MotoUDP(h,p);
    socket->ConnectMotoman();
    connect(socket->client,SIGNAL(readyRead()),this,SLOT(Read()));
    }
    else {
         ui->pushButtonConnect->setText("CONNECT");
         ui->groupBox_2->setEnabled(false);
         ui->groupBoxGoalPos->setEnabled(false);
         ui->groupGoalPulse->setEnabled(false);
         ui->groupBoxVel->setEnabled(false);
         socket->CloseMotoman();
    }
}
void MainWindow::Read()
{
    socket->ReceiveData();
    stop = clock();
    ui->textBrowserReceiveData->setText(socket->rx_data);
    ui->textBrowserTime->setText(QString::number(double(stop-start)/CLOCKS_PER_SEC*1000));
    switch (socket->Get_rx_buffer()->at(11)) {
    case 02:
        DisplayPosition();
        break;
    case 03:
        DisplayPulse();
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButtonServo_clicked()
{
    if(ui->pushButtonServo->text()=="SERVO ON")
    {
        ui->pushButtonServo->setText("SERVO OFF");
        start = clock();
        socket->TurnOnServo();
        ui->textBrowserSentData->setText(socket->tx_data);
    }
    else {
        ui->pushButtonServo->setText("SERVO ON");
        start = clock();
        socket->TurnOffServo();
        ui->textBrowserSentData->setText(socket->tx_data);
    }
}

void MainWindow::on_pushButtonGetPosition_clicked()
{
    start = clock();
    socket->GetPosition();
    ui->textBrowserSentData->setText(socket->tx_data);
}
void MainWindow::WaitandPrint()
{
    while(!socket->client->hasPendingDatagrams()){;}
    stop = clock();
    ui->textBrowserReceiveData->setText(socket->rx_data);
    ui->textBrowserTime->setText(QString::number(double((stop-start)/CLOCKS_PER_SEC*1000)));
}

void MainWindow::on_pushButtonGetPulse_clicked()
{
    start = clock();
    socket->GetPulsePosition();
    ui->textBrowserSentData->setText(socket->tx_data);
}
void MainWindow::DisplayPosition()
{
        ui->textBrowserX->setText(QString::number((double)socket->ByteArray2Int32(socket->Get_rx_buffer(),52,4)/1000));
        ui->textBrowserY->setText(QString::number((double)socket->ByteArray2Int32(socket->Get_rx_buffer(),56,4/1000)));
        ui->textBrowserZ->setText(QString::number((double)socket->ByteArray2Int32(socket->Get_rx_buffer(),60,4)/1000));
        ui->textBrowserRX->setText(QString::number((double)socket->ByteArray2Int32(socket->Get_rx_buffer(),64,4)/10000));
        ui->textBrowserRY->setText(QString::number((double)socket->ByteArray2Int32(socket->Get_rx_buffer(),68,4)/10000));
        ui->textBrowserRZ->setText(QString::number((double)socket->ByteArray2Int32(socket->Get_rx_buffer(),72,4)/10000));
}
void MainWindow::DisplayPulse()
{
        ui->textBrowserS->setText(QString::number(socket->ByteArray2Int32(socket->Get_rx_buffer(),52,4)));
        ui->textBrowserR->setText(QString::number(socket->ByteArray2Int32(socket->Get_rx_buffer(),56,4)));
        ui->textBrowserL->setText(QString::number(socket->ByteArray2Int32(socket->Get_rx_buffer(),60,4)));
        ui->textBrowserU->setText(QString::number(socket->ByteArray2Int32(socket->Get_rx_buffer(),64,4)));
        ui->textBrowserB->setText(QString::number(socket->ByteArray2Int32(socket->Get_rx_buffer(),68,4)));
        ui->textBrowserT->setText(QString::number(socket->ByteArray2Int32(socket->Get_rx_buffer(),72,4)));
}

void MainWindow::on_pushButtonMOVJ_clicked()
{
    bool x,y,z,rx,ry,rz,v;
    x = ui->textEditX->toPlainText().size()==0;
    y = ui->textEditY->toPlainText().size()==0;
    z = ui->textEditZ->toPlainText().size()==0;
    rx = ui->textEditRX->toPlainText().size()==0;
    ry = ui->textEditRY->toPlainText().size()==0;
    rz = ui->textEditRZ->toPlainText().size()==0;
    v = ui->textEditV->toPlainText().size()==0;
    if(x||y||z||rx||ry||rz)
    {
        QMessageBox::critical(this,"Error","Please insert the position!");
    }
    else if (v) {
        QMessageBox::critical(this,"Error","Please insert the velocity!");
    }
    else {
        int32_t speed = ui->textEditV->toPlainText().toDouble(&v)*100;
        int32_t X = ui->textEditX->toPlainText().toDouble(&x)*1000;
        int32_t Y = ui->textEditY->toPlainText().toDouble(&y)*1000;
        int32_t Z = ui->textEditZ->toPlainText().toDouble(&z)*1000;
        int32_t RX = ui->textEditRX->toPlainText().toDouble(&rx)*10000;
        int32_t RY = ui->textEditRY->toPlainText().toDouble(&ry)*10000;
        int32_t RZ = ui->textEditRZ->toPlainText().toDouble(&rz)*10000;
        if(x&&y&&z&&rx&&ry&&rz&&v)
        {
            start = clock();
            socket->WritePosition(speed,X,Y,Z,RX,RY,RZ);
            ui->textBrowserSentData->setText(socket->tx_data);
        }
        else {
            QMessageBox::critical(this,"Error","Please check the number format!");
        }

    }
}

void MainWindow::on_pushButtonMOVL_clicked()
{
    bool r1,r2,r3,r4,r5,r6,v;
    r1 = ui->textEditR1->toPlainText().size()==0;
    r2 = ui->textEditR2->toPlainText().size()==0;
    r3 = ui->textEditR3->toPlainText().size()==0;
    r4 = ui->textEditR4->toPlainText().size()==0;
    r5 = ui->textEditR5->toPlainText().size()==0;
    r6 = ui->textEditR6->toPlainText().size()==0;
    v = ui->textEditV->toPlainText().size()==0;
    if(r1||r2||r3||r4||r5||r6)
    {
        QMessageBox::critical(this,"Error","Please insert the pulse!");
    }
    else if (v) {
        QMessageBox::critical(this,"Error","Please insert the velocity!");
    }
    else {
        int32_t speed = ui->textEditV->toPlainText().toDouble(&v)*100;
        int32_t R1 = ui->textEditR1->toPlainText().toInt(&r1);
        int32_t R2 = ui->textEditR2->toPlainText().toInt(&r2);
        int32_t R3 = ui->textEditR3->toPlainText().toInt(&r3);
        int32_t R4 = ui->textEditR4->toPlainText().toInt(&r4);
        int32_t R5 = ui->textEditR5->toPlainText().toInt(&r5);
        int32_t R6 = ui->textEditR6->toPlainText().toInt(&r6);
        if(v&&r1&&r2&&r3&&r4&&r5&&r6)
        {
            start = clock();
            socket->WritePulse(speed,R1,R2,R3,R4,R5,R6);
            ui->textBrowserSentData->setText(socket->tx_data);
        }
        else {
            QMessageBox::critical(this,"Error","Please check the number format!");
        }

    }
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "motoudp.h"
#include <time.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_clicked();
    void Read();
    void WaitandPrint();

    void on_pushButtonServo_clicked();

    void on_pushButtonGetPosition_clicked();

    void on_pushButtonGetPulse_clicked();

    void on_pushButtonMOVJ_clicked();

    void on_pushButtonMOVL_clicked();

    void on_pushButtonMOVC_clicked();

    void on_pushButtonGetStatus_clicked();

    void on_pushButtonGetVarPOs_clicked();

    void on_pushButtonWriteVarPos_clicked();

    void on_pushButtonSelectJob_clicked();

    void on_pushButtonStartJob_clicked();

    void on_pushButtonOUT_clicked();

    void on_pushButtonOUT_2_clicked();

private:
    Ui::MainWindow *ui;
    MotoUDP* socket;
    QUdpSocket *client;
    time_t start;
    time_t stop;
    void DisplayPosition();
    void DisplayPulse();
    void DisplayStatus();
};

#endif // MAINWINDOW_H

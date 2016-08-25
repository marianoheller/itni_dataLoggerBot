#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTimer>
#include "httprequestworker.h"


enum eRunModes {
    eDeviceMode = 0,
    eFixedPayload
};

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer internalClock;
    HttpRequestWorker *worker;

private slots:
    void on_pushButton_send_clicked();
    void handle_result(HttpRequestWorker *worker);


    void internalClockTick();
    QByteArray generatePayload();
    void sendAuthrization();

    void on_pushButton_loadData_clicked();
    void on_actionLimpiar_consola_triggered();
    void on_toolButton_setLocalClock_clicked();
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>


QString DEFAULT_URL = "http://localhost/web/logdata";
QString DEFAULT_METHOD = "POST";
QString DEFAULT_CONTENT_TYPE = "application/json";
int DEFAULT_INTERVAL = 60;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&internalClock,SIGNAL(timeout()),this,SLOT(internalClockTick()));

    QStringList methodList;
    methodList.append("GET");
    methodList.append("POST");
    methodList.append("PUT");
    methodList.append("HEAD");
    methodList.append("DELETE");
    ui->comboBox_method->addItems(methodList);

    QStringList contentTypeList;
    contentTypeList.append("text/plain");
    contentTypeList.append("application/json");
    ui->comboBo_contentType->addItems(contentTypeList);

    ui->plainTextEdit_console->setReadOnly(true);

    ui->dateTimeEdit_clock->setDisplayFormat("dd/MM/yyyy hh:mm:ss");

    ui->lineEdit_url->setText(DEFAULT_URL);
    ui->comboBox_method->setCurrentText(DEFAULT_METHOD);
    ui->comboBo_contentType->setCurrentText(DEFAULT_CONTENT_TYPE);
    ui->spinBox_intervalo->setValue(DEFAULT_INTERVAL);
    ui->toolButton_setLocalClock->click();
}

MainWindow::~MainWindow() {
    delete ui;
}



void MainWindow::on_pushButton_send_clicked() {

    if ( ui->tabWidget_runMode->currentIndex() == eDeviceMode) {

        if ( internalClock.isActive() ) { //Stopping
            internalClock.stop();
            ui->pushButton_send->setText("Enviar");
            ui->groupBox_runMode->setEnabled(true);
        }
        else {  //Start ensayo
            internalClock.start(1000);
            ui->pushButton_send->setText("Detener");
            ui->groupBox_runMode->setDisabled(true);
        }
    }
    else if ( ui->tabWidget_runMode->currentIndex() == eFixedPayload ){

        HttpRequestInput input(ui->lineEdit_url->text(), ui->comboBox_method->currentText());
        input.setContentType(ui->comboBo_contentType->currentText());

        input.setPayload(ui->plainTextEdit_payload->toPlainText().toLocal8Bit());
        qDebug(ui->plainTextEdit_payload->toPlainText().toLocal8Bit());

        HttpRequestWorker *worker = new HttpRequestWorker(this);
        connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result(HttpRequestWorker*)));
        worker->execute(&input);

    }
    else {
        ui->plainTextEdit_console->clear();
        ui->plainTextEdit_console->appendPlainText("Error de run mode");
    }
}

void MainWindow::internalClockTick() {
    static int counterTick = ui->spinBox_intervalo->value();
    ui->dateTimeEdit_clock->setDateTime(ui->dateTimeEdit_clock->dateTime().addSecs(1));
    counterTick--;
    if (counterTick <= 0) {
        on_actionLimpiar_consola_triggered();
        counterTick = ui->spinBox_intervalo->value();
        QByteArray payload = generatePayload();
        qDebug(payload);

        HttpRequestInput input(ui->lineEdit_url->text(), ui->comboBox_method->currentText());
        input.setContentType(ui->comboBo_contentType->currentText());

        input.setPayload(payload);

        HttpRequestWorker *worker = new HttpRequestWorker(this);
        connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result(HttpRequestWorker*)));
        worker->execute(&input);
    }
}

QByteArray MainWindow::generatePayload() {
    QByteArray payload="";
    QString new_line = "\r\n";

    payload.append("{ \"LOGIN\": [");
    payload.append(new_line);
    payload.append("  { \"password\": \"INTI1957\"}");
    payload.append(new_line);
    payload.append(" ],");
    payload.append(new_line);
    payload.append("  \"CANALES\": [");
    for (int i=1 ; i<=32 ; i++) {
        payload.append(new_line);
        QString stringAux = "";
        stringAux.append("    { ");
        stringAux.append("\"canal\": \"");
        stringAux.append(QString::number(i));
        stringAux.append("\", \"temperatura\": \"");
        float randomValue = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/ui->doubleSpinBox_maxVal->value()));
        stringAux.append(QString::number(randomValue));
        stringAux.append("\", \"fecha\": \"");
        stringAux.append(ui->dateTimeEdit_clock->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
        stringAux.append("\" }");
        if ( i!=32 ) {
            stringAux.append(",");
        }

        payload.append(stringAux);
    }
    payload.append(new_line);
    payload.append(" ]");
    payload.append(new_line);
    payload.append("}");

    return payload;
}

void MainWindow::handle_result(HttpRequestWorker *worker) {
    ui->plainTextEdit_console->appendPlainText("User-Agent: "+worker->requestHeader.userAgent.toString());
    ui->plainTextEdit_console->appendPlainText("Content type: "+worker->requestHeader.contentType.toString());
    ui->plainTextEdit_console->appendPlainText(worker->requestHeader.data);
    QString msg;
    if (worker->error_type == QNetworkReply::NoError) {
        // communication was successful
        msg = "Success - Response: " + worker->response;
    }
    else {
        // an error occurred
        msg = "Error: " + worker->error_str;
    }

    ui->plainTextEdit_console->appendPlainText("===========================");
    ui->plainTextEdit_console->appendPlainText(worker->response);
    //QMessageBox::information(this, "", msg);
}

void MainWindow::on_pushButton_loadData_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
             tr("Open payload"), "Todos", tr("Todos (*.*)"));

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       ui->plainTextEdit_payload->clear();
       while (!in.atEnd())
       {
          QString line = in.readLine();
          ui->plainTextEdit_payload->appendPlainText(line);
       }
       inputFile.close();
    }
}

void MainWindow::on_actionLimpiar_consola_triggered()
{
    ui->plainTextEdit_console->clear();
}

void MainWindow::on_toolButton_setLocalClock_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    ui->dateTimeEdit_clock->setDateTime(now);
}

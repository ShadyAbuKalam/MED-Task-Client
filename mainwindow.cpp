#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork>
#include <QAbstractSocket>
#include <QMessageBox>
#define MAXPORTNUMBER 65535
#define MINPORTNUMBER 0
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);

    // QIntValidator can't prevent typing zeroes, so we may have an input = 0, even if we raised the bottom of the validator
    this->ui->portLineEdit->setValidator(new QIntValidator(0,MAXPORTNUMBER));
    this->socket = new QTcpSocket(this);

    connect(this->socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onSocketerror(QAbstractSocket::SocketError)));
    connect(this->socket,SIGNAL(readyRead()),this,SLOT(onSocketReadyRead()));
    connect(this->socket,SIGNAL(connected()),this,SLOT(onSocketConnected()));
    connect(this->socket,SIGNAL(disconnected()),this,SLOT(onSocketDisconnected()));

    // Rows indexing start at 1 (Sorry,But it's not the famous zero-based indexing :( )
    this->rowIndex = 1;
    // set locale to english, so we get english month names:
    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    // seconds of current time, we'll use it as starting point in time for data:
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);

    // configure bottom axis text labels:
    ui->customPlot->xAxis->setTicker(textTicker);
    // set a more compact font size for bottom and left axis tick labels:
    ui-> customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui-> customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    // set axis labels:
    ui-> customPlot->xAxis->setLabel("Time");
    ui-> customPlot->yAxis->setLabel("Random values");
    // set axis ranges to show all data:
    ui->customPlot->xAxis->setRange(this->rowIndex, this->rowIndex+10);
    ui->customPlot->yAxis->setRange(0, 1000);
}


MainWindow::~MainWindow()
{
    delete this->ui;
    delete this->socket;
}

void MainWindow::on_connectButton_clicked()
{
    //Disable interface
    this->ui->connectButton->setEnabled(false);
    this->ui->portLineEdit->setEnabled(false);

    //Update StatusBar
    this->ui->statusBar->showMessage("Connecting..");

    // Connecting
    int port = ui->portLineEdit->text().toInt();
    this->socket->connectToHost(QHostAddress::LocalHost,port);
}

void MainWindow::on_portLineEdit_textChanged(const QString &arg1)
{

    // Due to the QintValidator associated withe the portlineEdit, the string will always have a number inside it
    if(arg1.toInt() > MINPORTNUMBER)
        this->ui->connectButton->setEnabled(true);
    else
        this->ui->connectButton->setEnabled(false);


}

void MainWindow::onSocketConnected()
{
    this->ui->statusBar->showMessage(QString("Connected. "));

    //Send the first row to send
    this->socket->write(QString::number(this->rowIndex).toAscii());

}

void MainWindow::onSocketDisconnected()
{
    this->ui->statusBar->showMessage(QString("Disconnected. "));

}

void MainWindow::onSocketerror(QAbstractSocket::SocketError socketError)
{
    //Enable interface
    this->ui->connectButton->setEnabled(true);
    this->ui->portLineEdit->setEnabled(true);

    QString errorMessage;
    switch(socketError)
        {
        case(QAbstractSocket::SocketError::ConnectionRefusedError):
            errorMessage = "Connection was refused by the server, please check for the port number";
            break;
        default:
            errorMessage = this->socket->errorString();

        }

    // Popup a message box with error
    QMessageBox (QMessageBox::Icon::Critical,"Connection Failure",errorMessage,QMessageBox::Button::Ok).exec();
    //Update StatusBar
    this->ui->statusBar->showMessage(QString("Error: %1").arg(errorMessage));
}

void MainWindow::onSocketReadyRead()
{
    char buff[100];
    this->socket->readLine(buff,sizeof(buff));

    QString result(buff);
    QStringList parts = result.split(":");

    long long stamp = parts[0].toLongLong();
    float value = parts[1].toFloat();
    //todo : push the data into a some sort of container
    ui->customPlot->graph()->addData(this->rowIndex,value);

    QDateTime timestamp;
    timestamp.setTime_t(stamp);
    //todo : extend text ticker to allow better memory allocation, as the used method is keeping allocating and only using 10
    textTicker->addTick(this->rowIndex,timestamp.toString(Qt::SystemLocaleShortDate));

    if (ui->customPlot->graph()->dataCount() >=11)
        {

            ui->customPlot->xAxis->setRange(ui->customPlot->xAxis->range().lower+1, ui->customPlot->xAxis->range().upper+1);
            ui->customPlot->graph()->data()->removeBefore(ui->customPlot->xAxis->range().lower);
        }
    ui->customPlot->replot();

    qDebug() << stamp <<" : "<<value <<endl;
    this->rowIndex++;

}


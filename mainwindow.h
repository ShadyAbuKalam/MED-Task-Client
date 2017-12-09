#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();

    void on_portLineEdit_textChanged(const QString &arg1);

    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketerror(QAbstractSocket::SocketError socketError);
    void onSocketReadyRead();

private:
    Ui::MainWindow *ui;

    QTcpSocket *socket;

    // The first row that the server will start sending, indexing starts from 1
    unsigned long long rowIndex;
};

#endif // MAINWINDOW_H

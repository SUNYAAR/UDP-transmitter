#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <fstream>

#include "readwritemanager.h"
#include "transceiver.h"
#include "udpsender.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void loadSignal();
    void startButtonClicked();
    void reset();
    void initNetwork();
    void forceInitNetwork();

private:
    QStringList devList;
    QString pcIPAdress;
    QString brodCasthwIPAdress;
    void setIpList(QStringList);
    void updateParams();
    void enableConfigUI(bool enable);

    Ui::MainWindow *ui;
    QString FileAddress;
    bool stopReadingFlag;
    ReadWriteManager *ioManager;
    QThread *ioThread;
    void closeEvent(QCloseEvent *event);

    transceiver *myTx;
    QThread *txThread;
    UDPSender *txSender = nullptr;

    int portNumber;
    int Gaptime;  // int Gaptime = 1;
    int frameLen; //1024
    int readSize; //must be a multiple of frameLen (handled in this class)
    const int nChunks{1024};
};

#endif  // MAINWINDOW_H

#include "mainwindow.h"

#include <QNetworkInterface>
#include <QUdpSocket>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    Gaptime = ui->GapTimeVal_LE->text().toInt();
    frameLen = ui->FrameLenVal_LE->text().toInt();
    readSize = ui->ReadSizeVal_LE->text().toInt();
    portNumber = ui->PortNum_LE->text().toInt();

    forceInitNetwork();

    ioManager = new ReadWriteManager(readSize, nChunks, Gaptime,frameLen);
    ioManager->setFileAddress(&FileAddress);
    ioManager->setStopReadingFlag(&stopReadingFlag);
    ioThread = new QThread;
    ioManager->moveToThread(ioThread);
    ioThread->start();

    connect(ui->LoadFileButton, SIGNAL(clicked(bool)), this, SLOT(loadSignal()));
    connect(ui->StartButton, SIGNAL(clicked(bool)), this,
            SLOT(startButtonClicked()));

    connect(ioManager, SIGNAL(finished()), this, SLOT(reset()));

    //---------------------------------------------- //
    myTx = new transceiver(readSize, nChunks);
    myTx->setStopSendingFlag(&stopReadingFlag);
    myTx->setFlagData(ioManager->getFlagData());
    myTx->setInputBuffer(ioManager->getBuffer());
    txThread = new QThread;
    myTx->moveToThread(txThread);
    txThread->start(QThread::TimeCriticalPriority);
    // ------------------------------- //
    QStringList devList = myTx->getDeviceList();
    ui->CB_devList->addItems(devList);

    connect(ui->SetNetwork_PB, SIGNAL(clicked(bool)), this, SLOT(initNetwork()));
    connect(ui->CB_devList, SIGNAL(currentIndexChanged(int)), this, SLOT(forceInitNetwork()));
}

void MainWindow::closeEvent(QCloseEvent *event) { event->accept(); }

void MainWindow::loadSignal() {
    QString newAddress =
        QFileDialog::getOpenFileName(0, tr("Open File"), "../../home:");
    if (!newAddress.isEmpty()) FileAddress = newAddress;
    qDebug() << "AddressFile = " << FileAddress;
}

void MainWindow::forceInitNetwork(){
    ui->StartButton->setEnabled(false);
    ui->LoadFileButton->setEnabled(false);
}

void MainWindow::enableConfigUI(bool enable){
    ui->CB_devList->setEnabled(enable);
    ui->SetNetwork_PB->setEnabled(enable);
    ui->LoadFileButton->setEnabled(enable);
    ui->GapTimeVal_LE->setEnabled(enable);
    ui->FrameLenVal_LE->setEnabled(enable);
    ui->ReadSizeVal_LE->setEnabled(enable);
    ui->PortNum_LE->setEnabled(enable);
}

void MainWindow::startButtonClicked() {
    if (ui->StartButton->text().toLower().contains("start")) {
        if (FileAddress.isEmpty()) {
            ui->LoadFileButton->setStyleSheet("border: 1px solid red;");
            return;
        } else {
            ui->LoadFileButton->setStyleSheet("");
        }

        stopReadingFlag = false;
        ui->StartButton->setText("Stop");
        enableConfigUI(false);

        ioManager->resetFlagData();
        QTimer::singleShot(200, ioManager, SLOT(getDataInput()));
        QTimer::singleShot(200, myTx, SLOT(sendOnePacket()));
    } else {
        ui->StartButton->setEnabled(false);
        stopReadingFlag = true;
    }
}

void MainWindow::reset() {
    ui->StartButton->setText("Start");
    ui->StartButton->setEnabled(true);
    enableConfigUI(true);
}

void MainWindow::setIpList(QStringList ipList) {
    pcIPAdress = ipList[0];
    brodCasthwIPAdress = ipList[1];
}

void MainWindow::updateParams(){
    Gaptime = ui->GapTimeVal_LE->text().toInt();
    frameLen = ui->FrameLenVal_LE->text().toInt();
    readSize = frameLen * (ui->ReadSizeVal_LE->text().toInt() / frameLen);
    ui->ReadSizeVal_LE->setText(QString::number(readSize));
    portNumber = ui->PortNum_LE->text().toInt();
    int num_repeat{ui->NumRep_LE->text().toInt()};
    ioManager->updateParams(readSize, Gaptime,num_repeat);
    myTx->updateParams(readSize);
    myTx->setInputBuffer(ioManager->getBuffer());

    double bitRate = 8.0 * (double(readSize)) * 1000.0 / double(Gaptime);
    ui->BitRateVal_Lbl->setText(QString::number(bitRate));
}

void MainWindow::initNetwork() {
    updateParams();

    qDebug() << ui->CB_devList->currentIndex() << ui->CB_devList->currentText();
    int networkIdx{ui->CB_devList->currentIndex()};
    setIpList(myTx->selectDevice(networkIdx));

    delete txSender;
    txSender = new UDPSender(pcIPAdress, brodCasthwIPAdress, portNumber);
    myTx->setTxCore(txSender);
    myTx->startTx(frameLen, 0);
    qDebug() << "initNetwork done!";

    ui->LoadFileButton->setEnabled(true);
    ui->StartButton->setEnabled(true);
}

MainWindow::~MainWindow() {
    ioThread->exit();
    delete ui;
}

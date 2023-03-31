#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QDebug>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <QtCore>
//#include "packetfill.h"
#include "stdlib.h"
//#include "byteutils.h"

class UDPSender : public QObject {
    Q_OBJECT
public:
    explicit UDPSender(QString hostAddr, QString destAddr, int port_number,
                       QObject *parent = 0);
    QByteArray RcvData;
    QByteArray RcvData111;
    void startSendingConstPacket();
signals:
    void packetReceive();
public slots:
    void setNewPacket(QByteArray &newPacket);
    void readyRead();
private slots:

private:
    int tempValue;
    bool activeRx;
    QString hostIPAdress;
    QString destIPAddress;
    int portNumber;
    QUdpSocket *mSocket;
    template <typename T>
    QVector<char> byteArray(T);
    QByteArray packet;

    QFile *file;

    void startSending();
    QMutex mMut;
};

#endif  // UDPSENDER_H

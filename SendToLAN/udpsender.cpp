#include "udpsender.h"

UDPSender::UDPSender(QString hostAddr, QString destAddr,
                     int port_number, QObject *parent)
    : QObject(parent), activeRx(true) {
    hostIPAdress = hostAddr;
    qDebug()<<hostIPAdress;
    destIPAddress = destAddr;
    portNumber = port_number;
    mSocket = new QUdpSocket(this);
    mSocket->bind(QHostAddress(hostIPAdress), 1234);

    //use with writaDatagram(..)
    //mSocket->bind(QHostAddress(hostIPAdress), portNumber);

    //use with write(..)
    mSocket->setPauseMode(QUdpSocket::PauseMode::PauseNever);
    mSocket->connectToHost(QHostAddress(destIPAddress), portNumber);
    long long buffSize = 1460*100000*10; //??
    mSocket->setSocketOption(QAbstractSocket::SocketOption::SendBufferSizeSocketOption,buffSize);
}

void UDPSender::setNewPacket(QByteArray &newPacket) {
//    packet = newPacket;
    while (mSocket->write(newPacket.constData(), newPacket.size()) < newPacket.size())
    {
    }
//        qDebug() << mSocket->error() << ", " << mSocket->errorString();
//    startSending();
}

void UDPSender::startSending() {
//        qint64 bytesWritten = mSocket->write(packet);
    //    if (bytesWritten < packet.size())
    //        qDebug() << mSocket->error() << ", " << bytesWritten;

    while ( tempValue< packet.size())
//    if (mSocket->write(packet.constData(), packet.size()) < packet.size())
    {
        tempValue=mSocket->write(packet);
//        qDebug() << mSocket->error() << ", " << mSocket->errorString()<<tempValue<<packet.size();

    }

//    if(mSocket->writeDatagram(packet, QHostAddress(destIPAddress), portNumber) < packet.size())
//        qDebug() << mSocket->error();
}

void UDPSender::startSendingConstPacket() {
    qint64 byteWritten;
    byteWritten =
        mSocket->writeDatagram(packet, QHostAddress(destIPAddress), portNumber);
}

void UDPSender::readyRead() {
    if (activeRx) {
        while (mSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(mSocket->pendingDatagramSize());
            mSocket->readDatagram(datagram.data(), datagram.size());
            if (datagram.mid(0, 4).toHex() == "feeffccf") {
                // qDebug() << "Message receive: " << datagram.toHex();
                //                RcvData = datagram.mid(11,1);

                // qDebug() << "RcvData: " << RcvData.toHex();
                emit packetReceive();
            }
        }
    }
}

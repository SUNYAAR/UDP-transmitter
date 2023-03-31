#include "transceiver.h"

transceiver::transceiver(const int &read_szie, const int &n_chunks,
                         QObject *parent)
    : QObject(parent), readSize(read_szie), nChunks(n_chunks) {}

void transceiver::updateParams(const int &read_size){
    readSize = read_size;
}

void transceiver::setTxCore(UDPSender *x) {
    txCore = x;
}

void transceiver::startTx(int payload_len, bool chkSum) {
    frame_i = 0;
    flagCheckSum = chkSum;
    payloadLen = payload_len;
    setupTx();
}

void transceiver::setupTx() {
    frameLen = payloadLen + 4;
    Packet_FrameCounter = 0;

    PayloadPacket.resize(frameLen);
    PayloadPacket[0] = PayloadPacket[1] = 0;
    qDebug() << frameLen << " -> Frame Len";

}

void transceiver::sendPacket(int isDummy) {
    Packet_FrameCounter = Packet_FrameCounter % payloadLen;
    PayloadPacket[2] = (Packet_FrameCounter >> 1 * 8) % 256;
    PayloadPacket[3] = Packet_FrameCounter % 256;

    Packet_FrameCounter++;
    txCore->setNewPacket(PayloadPacket);
}

QStringList transceiver::getDeviceList() {
    QStringList devList;
    foreach (const QNetworkInterface &NetworkInterface,
             QNetworkInterface::allInterfaces()) {
        unsigned int flags = NetworkInterface.flags();
        bool isUp = (bool)(flags & QNetworkInterface::IsUp);
        bool isRunning = (bool)(flags & QNetworkInterface::IsRunning);
        //        qDebug() <<  isUp << isRunning << "  " <<
        //        NetworkInterface.humanReadableName() ;
        devList << NetworkInterface.humanReadableName();
        // deviceListActivation << (isUp && isRunning);
        // qDebug() << deviceList[i] << deviceListActivation[i];
    }
    return devList;
}

QStringList transceiver::selectDevice(int deviceIdx) {
    QStringList IpList;

    QList<QNetworkInterface> NetworkInterface =
        QNetworkInterface::allInterfaces();
    QString macAddress = NetworkInterface[deviceIdx].hardwareAddress();
    // QStringList macList = macAddress.split(':');
    qDebug() << "MACAddress " << macAddress;
    QString stringHostAddress;
    QStringList hostList;
    QString pcIPAdress;
    QString brodCasthwIPAdress;

    QNetworkInterface eth1Ip =
        QNetworkInterface::interfaceFromName(NetworkInterface[deviceIdx].name());
    QList<QNetworkAddressEntry> entries = eth1Ip.addressEntries();
    QNetworkAddressEntry entry = entries.last();
    if (entry.netmask().toString() == "255.255.255.0") {
        stringHostAddress = entry.ip().toString();
    } else {
        entry = entries.first();
        stringHostAddress = entry.ip().toString();
    }
    qDebug() << "Netmask: " << entry.netmask().toString();

    hostList = stringHostAddress.split('.');
    pcIPAdress =
        hostList[0] + "." + hostList[1] + "." + hostList[2] + "." + hostList[3];
    brodCasthwIPAdress =
        hostList[0] + "." + hostList[1] + "." + hostList[2] + "." + "255";

    qDebug() << pcIPAdress;

    IpList << pcIPAdress << brodCasthwIPAdress;
    //qDebug() << "Inside selectDevice: ";
    //qDebug() << pcIPAdress << ", " << brodCasthwIPAdress;
    return IpList;
}

// we're assuming that this is ahead of file reading
// void transceiver::sendOnePacket() {
//  if (flagData[frame_i]) {
//    for (int i{}; i < readSize / payloadLen; ++i) {
//      // qDebug() << "transceiver: chunk " << frame_i + 1;
//      // memcpy(PayloadPacket.data()+4, input_buffer + frame_i*payloadLen,
//      // payloadLen*sizeof(uint8_t));
//      memcpy(PayloadPacket.data() + 4,
//             input_buffer + frame_i * readSize + i * payloadLen,
//             payloadLen * sizeof(uint8_t));
//      sendPacket(0);
//    }
//    flagData[frame_i] = 0;
//    frame_i++;
//    if (frame_i == nChunks) frame_i = 0;
//  }
//  if (!stopSendingFlag[0])
//    QTimer::singleShot(TimeIntruption, this, SLOT(sendOnePacket()));
//  else {
//    frame_i = 0;
//    // Packet_FrameCounter = 0;
//    qDebug() << "Sending Packets stopped";
//  }
//}

void transceiver::sendOnePacket() {
    while (!stopSendingFlag[0]) {
        if (flagData[frame_i]) {
            for (int i{}; i < readSize / payloadLen; ++i) {
                //        auto start{std::chrono::high_resolution_clock::now()};

                memcpy(PayloadPacket.data() + 4,
                       input_buffer + frame_i * readSize + i * payloadLen,
                       payloadLen * sizeof(uint8_t));

                sendPacket(0);

                //        txCore->startSendingConstPacket();

                //        auto stop{std::chrono::high_resolution_clock::now()};
                //        auto time_taken{
                //            std::chrono::duration_cast<std::chrono::microseconds>(stop
                //            - start)
                //                .count()};
                //        qDebug() << "Time taken to send " << payloadLen
                //                 << " bytes : " << time_taken;
            }
            flagData[frame_i] = 0;
            frame_i++;
            if (frame_i == nChunks) frame_i = 0;
        }
    }

    frame_i = 0;
    Packet_FrameCounter = 0;
    qDebug() << "Sending Packets stopped";
}

void transceiver::setStopSendingFlag(bool *flag) { stopSendingFlag = flag; }

void transceiver::setFlagData(bool *flag) { flagData = flag; }

void transceiver::setInputBuffer(uint8_t *buffer) { input_buffer = buffer; }

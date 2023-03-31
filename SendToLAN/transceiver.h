#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <QNetworkInterface>
#include <QObject>
#include <QTimer>
#include <cstdlib>

#include "udpsender.h"

class transceiver : public QObject {
    Q_OBJECT
public:
    explicit transceiver(const int &read_szie, const int &n_chunks,
                         QObject *parent = nullptr);
    void setTxCore(UDPSender *x);
    void startTx(int payload_len, bool chkSum);
    void updateParams(const int&);

    void setStopSendingFlag(bool *);
    void setFlagData(bool *);
    void setInputBuffer(uint8_t *);

    QStringList getDeviceList();
    QStringList selectDevice(int deviceIdx);

private:
    bool flagCheckSum{false};
    UDPSender *txCore;
    QByteArray PayloadPacket;
    void setupTx();
    void sendPacket(int isDummy);

    int payloadLen{0}, frameLen{};
    uint Packet_FrameCounter{0};
    int frame_i{0};
    int readSize;
    const int nChunks;

    bool *stopSendingFlag;
    bool *flagData;
    uint8_t *input_buffer;

signals:

private slots:
    void sendOnePacket();
};

#endif  // TRANSCEIVER_H

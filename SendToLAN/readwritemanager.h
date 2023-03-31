#ifndef READWRITEMANAGER_H
#define READWRITEMANAGER_H

#include <QObject>
#include<QTimer>
#include<QDebug>

#include<fstream>
#include<chrono>

class ReadWriteManager : public QObject
{
    Q_OBJECT
public:
    explicit ReadWriteManager(const int& read_size,
                              const int& n_chunks,
                              const int& gap_time,
                              const int frame_len,
                              QObject *parent = nullptr);

    void setFileAddress(QString *);
    void setStopReadingFlag(bool *);
    uint8_t *getBuffer();
    bool* getFlagData();
    void resetFlagData();

public slots:
    void getDataInput();
    void requestDataFromFile();
    void updateParams(const int&, const int&, const int num_repeat);

private:
    int numRepeat;
    int i_repeat{0};
    int frameLen;
    int readSize;
    const int nChunks;
    int requestTimeMilsec;

    QString *FileAddress;
    bool *stopReadingFlag;
    std::ifstream inputFile;
    uint64_t num_elements;
    uint8_t *buffer;
    bool *flagData;
signals:
    void finished();

};

#endif // READWRITEMANAGER_H

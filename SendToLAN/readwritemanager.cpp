#include "readwritemanager.h"

ReadWriteManager::ReadWriteManager(const int& read_size,
                                   const int& n_chunks, const int& gap_time,const int frame_len,
                                   QObject* parent)
    : QObject(parent),
    readSize(read_size),
    nChunks(n_chunks),
    frameLen(frame_len),
    requestTimeMilsec(gap_time) {
    buffer = new uint8_t[readSize * nChunks];
    flagData = new bool[nChunks]{};
}

void ReadWriteManager::updateParams(const int& read_size, const int& gap_time,const int num_repeat){
    numRepeat=num_repeat;
    readSize = read_size;

    requestTimeMilsec = gap_time;
    delete[] buffer;
    buffer = new uint8_t[readSize * nChunks];
}

void ReadWriteManager::setFileAddress(QString* addr) {
    FileAddress = addr;
}

void ReadWriteManager::setStopReadingFlag(bool* flag) {
    stopReadingFlag = flag;
}

uint8_t* ReadWriteManager::getBuffer() {
    return buffer;
}

void ReadWriteManager::requestDataFromFile() {
    static uint64_t pos{};
    static int flagRead{};
    inputFile.clear();
    inputFile.seekg(pos);

    if (!flagData[flagRead]) {
        inputFile.read(reinterpret_cast<char*>(buffer + flagRead * readSize),
                       readSize * sizeof(uint8_t));


        int elementsRead = inputFile.gcount() / sizeof(uint8_t);  // 1 but still
        if (elementsRead < readSize) {
            if(i_repeat==numRepeat)
            {
                inputFile.close();
                pos = flagRead = 0;
                stopReadingFlag[0]=1;
                qDebug() << "Reading file stopped";
                emit finished();
                return ;
            }
            else
               i_repeat++;
            inputFile.clear();
            inputFile.seekg(0);
            inputFile.read(
                reinterpret_cast<char*>(buffer + flagRead * readSize+elementsRead),
                (readSize-elementsRead) * sizeof(uint8_t));
            pos = readSize-elementsRead;
            qDebug() << "One round finished";
        } else {
            pos += readSize;
            if (pos == num_elements) pos = 0;
        }

        flagData[flagRead] = 1;
        flagRead++;
        if (flagRead == nChunks) flagRead = 0;
    } else {
        qDebug() << "------------ ! Frame Loss ! -------------";
    }

    if (!stopReadingFlag[0])
        QTimer::singleShot(requestTimeMilsec, this, SLOT(requestDataFromFile()));
    else {
        inputFile.close();
        pos = flagRead = 0;
        qDebug() << "Reading file stopped";
        emit finished();
    }
}

void ReadWriteManager::getDataInput() {
    inputFile.open(FileAddress[0].toStdString(), std::ios::binary);
    if (!inputFile.is_open()) {
        qDebug() << "Error: Cannot open input file";
        exit(1);
    }
    num_elements = inputFile.tellg() / sizeof(uint8_t);
    QTimer::singleShot(requestTimeMilsec, this, SLOT(requestDataFromFile()));
}

bool* ReadWriteManager::getFlagData() { return flagData; }

void ReadWriteManager::resetFlagData(){
    for (int i{}; i < nChunks; ++i) flagData[i] = false;

    i_repeat=0;

}

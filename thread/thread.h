#ifndef THREAD_H
#define THREAD_H
#include <QObject>
#include <QRunnable>
#include <QUdpSocket>
#include <QtEndian>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QWaitCondition>
#include "device/device.h"
#include "tftp/tftp.h"
#include "tftp/tftprequest.h"
#include "protocal/protocal.h"
#include "crc/crc.h"
#include "safequeue/safequeue.h"
#include "global.h"
#include "spdlog/common.h"

namespace spdlog {
class logger;
}
#define TFTP_READ_OP_CODE 1
#define TFTP_WRITE_OP_CODE 2
class thread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    thread(const Device* device, QObject* parent = nullptr);
    virtual ~thread();
    const QHostAddress getHostAddress() const;
    TftpRequest* getTftpRequest() const;
    const Device *getDevice() const;
    void addTftpRequest(const TftpRequest& tftpRequest);
    virtual void parseStatusFile() = 0;
protected:
    QDir dir;
    const Device* device;
    spdlog::logger& logger;
    SafeQueue<TftpRequest> tftpRequests;

    QString statusMessage;
    QString errorMessage;
    quint16 statusCode;
    std::condition_variable cv;
    std::mutex m;

    void waitStatusFileRcved();
    bool waitStatusFileRcved(QString& errorMessage, unsigned long mseconds = ULONG_MAX);
signals:
    void threadFinish(bool status, QString info);
    void mainThreadExitedSignal(); 
};


#endif // THREAD_H

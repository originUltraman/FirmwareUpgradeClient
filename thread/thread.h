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
#include "singleton/singleton.h"
#include "safequeue/safequeue.h"
#include "global.h"

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
    void addTftpRequest(TftpRequest&& tftpRequest);
protected:
    const Device* device;
    std::shared_ptr<QUdpSocket> protocalFileSocket;
    std::shared_ptr<QUdpSocket> statusFileSocket;
    spdlog::logger& logger;
    SafeQueue<TftpRequest> tftpRequests;

    QString statusMessage;
    QString errorMessage;
    quint16 statusCode;

    QDir dir;
    void waitStatusFileRcved();
    bool waitStatusFileRcved(QString& errorMessage, unsigned long mseconds = ULONG_MAX);
signals:
    void threadFinish(bool status, QString info);
    void mainThreadExitedSignal();
public slots:
    virtual void parseStatusFile() = 0;
};


#endif // THREAD_H

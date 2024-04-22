#ifndef STATUSFILERCVTHREAD_H
#define STATUSFILERCVTHREAD_H
#include <QObject>
#include "thread/thread.h"
#include "thread/uploadthread.h"
#include "tftp/tftprequest.h"

class StatusFileRcvThread : public thread{
    Q_OBJECT
public:
    StatusFileRcvThread(const TftpRequest tftpRequest, const Device* device = nullptr, QObject* parent = nullptr):
    thread(device, parent)
    {

    }
    void run();

signals:
    void statusFileRcvFinishedSignal();
};

#endif // STATUSFILERCVTHREAD_H

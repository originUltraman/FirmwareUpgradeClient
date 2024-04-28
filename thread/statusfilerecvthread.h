#ifndef STATUSFILERCVTHREAD_H
#define STATUSFILERCVTHREAD_H
#include <QObject>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include "thread/uploadthread.h"
#include "tftp/tftprequest.h"
namespace spdlog {
class logger;
}
class StatusFileRcvThread : public QThread{
    Q_OBJECT
public:
    StatusFileRcvThread(QObject* parent = nullptr);
    void run();
    void addTftpRequest(TftpRequest& tftpRequest);
    void addThread(const std::string&, class thread*);
    void threadExit();

private:
    std::unordered_map<std::string, class thread*> hostToThread;
    spdlog::logger& logger;
    SafeQueue<TftpRequest> tftpRequests;
    std::mutex m;
    std::condition_variable cv;
    bool exitFlag = false;
};

#endif // STATUSFILERCVTHREAD_H

#ifndef TFTPSERVERTHREAD_H
#define TFTPSERVERTHREAD_H
#include<QObject>
#include<QRunnable>
#include<memory>
#include<QThread>
#include<unordered_map>
#include<QUdpSocket>
class thread;
namespace spdlog{
class logger;
}
class TftpServerThread: public QThread
{
    Q_OBJECT
public:
    TftpServerThread();
    void run() override;
    void addThread(const std::string&, class thread*);
    void removeThread(const std::string&);
    //void quitThread();

private:
    std::shared_ptr<QUdpSocket> uSock;
    std::unordered_map<std::string, class thread*> hostToThread;
    spdlog::logger& logger;
private slots:
    void onUSockReadyRead();
};

#endif // TFTPSERVERTHREAD_H

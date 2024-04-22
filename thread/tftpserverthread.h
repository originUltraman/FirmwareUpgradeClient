#ifndef TFTPSERVERTHREAD_H
#define TFTPSERVERTHREAD_H
#include<QObject>
#include<QRunnable>
#include<memory>
#include<unordered_map>
#include<QUdpSocket>
class thread;
class TftpServerThread: public QObject, public QRunnable
{
    Q_OBJECT
public:
    TftpServerThread();
    void run() override;
    void addThread(const std::string&, class thread*);
    void removeThread(const std::string&);

private:
    std::shared_ptr<QUdpSocket> uSock;
    std::unordered_map<std::string, class thread*> hostToThread;
private slots:
    void onUSockReadyRead();
};

#endif // TFTPSERVERTHREAD_H

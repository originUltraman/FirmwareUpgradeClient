#ifndef FINDTHREAD_H
#define FINDTHREAD_H

#include <QObject>
#include <QRunnable>
#include <memory>
namespace spdlog{
class logger;
}
class QUdpSocket;
class QTimer;
class FindThread: public QObject, public QRunnable
{
    Q_OBJECT
public:
    FindThread();
    ~FindThread();
    void run();
private:
    const QByteArray& makeFindRequest();
    std::shared_ptr<QUdpSocket> uSock;
    std::shared_ptr<QTimer> timer;
    spdlog::logger* logger;
private slots:
    void onFindResponseRecved();
};

#endif // FINDTHREAD_H

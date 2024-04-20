#ifndef FINDTHREAD_H
#define FINDTHREAD_H

#include <QObject>
#include <QRunnable>
#include <memory>
class QUdpSocket;
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
};

#endif // FINDTHREAD_H

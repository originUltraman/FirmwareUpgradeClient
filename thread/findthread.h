#ifndef FINDTHREAD_H
#define FINDTHREAD_H

#include <QObject>
#include <QRunnable>
#include <memory>
#include <QThread>
#include "device/device.h"
namespace spdlog{
class logger;
}
class QUdpSocket;
class QTimer;
class FindThread: public QThread
{
    Q_OBJECT
public:
    explicit FindThread(QObject *parent = nullptr);
    ~FindThread();
    void run();
private:
    const QByteArray& makeFindRequest();
    QTimer* timer;
    QUdpSocket* uSock;
    spdlog::logger* logger;
    std::vector<Device> devices;
private slots:
    void onFindResponseRecved();
    void onTimerTimeout();
signals:
    void findOperationFinished(std::vector<Device>);
};

#endif // FINDTHREAD_H

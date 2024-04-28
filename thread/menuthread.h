#ifndef MENUTHREAD_H
#define MENUTHREAD_H

#include <QObject>
#include <QRunnable>
#include "singleton/singleton.h"
class MenuThread: public QObject, public QRunnable
{
    Q_OBJECT
public:
    MenuThread();
    void printMenu() const;
    void run();
private:
    spdlog::logger& logger;
    std::function<void(std::string)> info;
    std::function<void(std::string)> debug;
    std::function<void(std::string)> error;
    std::function<void()>flush;
signals:
    void sendMenuOp(int, const std::vector<std::string>&);
};

#endif // MENUTHREAD_H

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
signals:
    void sendMenuOp(int, const std::vector<std::string>&);
};

#endif // MENUTHREAD_H

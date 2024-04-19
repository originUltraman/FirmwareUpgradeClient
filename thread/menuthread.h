#ifndef MENUTHREAD_H
#define MENUTHREAD_H

#include <QObject>
#include <QRunnable>
class MenuThread: public QObject, public QRunnable
{
    Q_OBJECT
public:
    MenuThread();
    void printMenu() const;
    void run();
signals:
    void sendMenuOp(int, std::initializer_list<std::string>);
};

#endif // MENUTHREAD_H

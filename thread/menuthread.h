#ifndef MENUTHREAD_H
#define MENUTHREAD_H

#include <QObject>
#include <QRunnable>
#include "singleton/singleton.h"
class File_LNL;
class MenuThread: public QObject, public QRunnable
{
    Q_OBJECT
public:
    MenuThread();
    void printMenu() const;
    void run();

private:
    spdlog::logger& logger;
    std::vector<std::pair<std::string, std::string>> files;
    std::function<void(std::string)> info;
    std::function<void(std::string)> debug;
    std::function<void(std::string)> error;
    std::function<void()>flush;
signals:
    void sendMenuOp(int, const std::vector<std::string>&);
    void downloadFiles(QStringList files);
public slots:
    void on_parseLNL_finished(File_LNL*);
};

#endif // MENUTHREAD_H

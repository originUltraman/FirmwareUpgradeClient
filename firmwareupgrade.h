#ifndef FIRMWAREUPGRADE_H
#define FIRMWAREUPGRADE_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <memory>
#include <QThreadPool>
class MenuThread;
class FindThread;
class TftpServerThread;
class FirmwareUpgrade : public QObject
{
    Q_OBJECT
public:
    FirmwareUpgrade();
    ~FirmwareUpgrade();
    /*
     * 根据输入参数的不同执行相关的逻辑
     *
     * 目前支持的操作
     * 1、find 输入参数个数2，输入格式：find 192.168.0.1
     * 2、upload 输入参数个数3+，输入格式：upload 192.168.0.4 filename1 filename2 ...
     * 3、download 输入参数个数3+，输入格式：download 192.168.0.4 filename1 filename2 ...
     *
     */

    /*
     * 根据传入的参数执行find操作
     *
     */
    void execFind(const QStringList &argList){
        qDebug() << "execFind(): " << argList;
    }

    /*
     * 根据传入的参数执行upload操作
     *
     */
    void execUpload(const QStringList &argList){

        qDebug() << "execUpload(): " << argList;
    }

    /*
     * 根据传入的参数执行download操作
     *
     */
    void execDownload(const QStringList &argsList){
        qDebug() << "execDownload(): " << argsList;
    }
private:
    std::shared_ptr<QTimer> timer = nullptr;
    std::shared_ptr<TftpServerThread> tftpServerThread = nullptr;
    std::shared_ptr<MenuThread> menuThread = nullptr;
    //std::shared_ptr<FindThread> findThread = nullptr;
    QThreadPool pool;

private slots:
    void onRecvdMenuOp(int op, const std::vector<std::string>& args);
};

#endif // FIRMWAREUPGRADE_H

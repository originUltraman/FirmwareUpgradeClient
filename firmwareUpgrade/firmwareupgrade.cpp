#include "firmwareupgrade.h"
#include <iostream>
#include "inputListener.h"
#include "thread/menuthread.h"
#include "thread/findthread.h"
#include "global.h"

FirmwareUpgrade::FirmwareUpgrade()
{
    pool.setMaxThreadCount(16);

//    // 创建输入监听器对象
//    InputListener inputListener;

//    // 连接输入监听器的信号与槽
//    QObject::connect(&inputListener, &InputListener::findRequested, [this](const QString &ipAddress){
//        std::cout << "Find requested for IP:" << ipAddress.toStdString() << std::endl;
//        // 执行 find 操作
//        FindThread* findThread = new FindThread();
//        findThread->setAutoDelete(true);//执行完在新线程里面自动调用析构函数
//        pool.start(findThread);
//        std::cout << "find"<<std::endl;
//    });

//    QObject::connect(&inputListener, &InputListener::uploadRequested, [this](const QString &ipAddress, const QStringList &filenames){
//        std::cout << "Upload requested for IP:" << ipAddress.toStdString() << "with filenames:";
//        for (const QString &filename : filenames) {
//            std::cout << " " << filename.toStdString();
//        }
//        std::cout << std::endl;
//        // 执行 upload 操作
//        std::cout << "upload"<<std::endl;
//    });

//    QObject::connect(&inputListener, &InputListener::downloadRequested, [this](const QString &ipAddress, const QStringList &filenames){
//        std::cout << "Download requested for IP:" << ipAddress.toStdString() << "with filenames:";
//        for (const QString &filename : filenames) {
//            std::cout << " " << filename.toStdString();
//        }
//        std::cout << std::endl;
//        // 执行 download 操作
//        std::cout << "download"<<std::endl;
//    });
    menuThread = std::make_shared<MenuThread>();
    //注册数据类型
    qRegisterMetaType<std::initializer_list<std::string>>("std::initializer_list<std::string>");
    connect(menuThread.get(), &MenuThread::sendMenuOp, this, &FirmwareUpgrade::onRecvdMenuOp);
    pool.start(menuThread.get());
}


void FirmwareUpgrade::onRecvdMenuOp(int op, std::initializer_list<std::string> list)
{
    switch (op) {
    case FIND:{
        //findThread = std::make_shared<FindThread>();
        FindThread* findThread = new FindThread();
        findThread->setAutoDelete(true);//执行完在新线程里面自动调用析构函数
        pool.start(findThread);
        break;
    }
    case UPLOAD:
        break;
    case DOWNLOAD:
        break;
    default:
        break;
    }
}

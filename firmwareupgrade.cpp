#include "firmwareupgrade.h"
#include <iostream>
#include <QCoreApplication>
#include "thread/menuthread.h"
#include "thread/findthread.h"
#include "thread/uploadthread.h"
#include "thread/tftpserverthread.h"
#include "global.h"
FirmwareUpgrade::FirmwareUpgrade()
{
    pool.setMaxThreadCount(16);
    menuThread = std::make_shared<MenuThread>();
    tftpServerThread = std::make_shared<TftpServerThread>();
    //注册数据类型
    qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
    connect(menuThread.get(), &MenuThread::sendMenuOp, this, &FirmwareUpgrade::onRecvdMenuOp);
    pool.start(menuThread.get());
    pool.start(tftpServerThread.get());
}

FirmwareUpgrade::~FirmwareUpgrade()
{
    std::cout << "quit application" << std::endl;
}

void FirmwareUpgrade::onRecvdMenuOp(int op, const std::vector<std::string>& args)
{
    switch (op) {
    case FIND:{
        //findThread = std::make_shared<FindThread>();
        FindThread* findThread = new FindThread();
        findThread->setAutoDelete(true);//执行完在新线程里面自动调用析构函数
        pool.start(findThread);
        break;
    }
    case UPLOAD:{
        QStringList list;
        for(auto file = args.begin() + 1; file != args.end(); ++file){
            list.append(QString::fromStdString(*file));
        }
        Device* device = new Device(QString::fromStdString(args[0]), "02DA");
        UploadThread* uploadThread = new UploadThread(list, device, nullptr);
        tftpServerThread->addThread(args[0], uploadThread);
        uploadThread->setAutoDelete(true);
        pool.start(uploadThread);
        break;
    }
    case DOWNLOAD:
        break;
    case EXIT:
        QCoreApplication::instance()->quit();
        break;
    default:
        break;
    }
}



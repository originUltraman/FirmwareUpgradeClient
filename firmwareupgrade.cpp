#include "firmwareupgrade.h"
#include <iostream>
#include <vector>
#include <QCoreApplication>
#include "thread/menuthread.h"
#include "thread/findthread.h"
#include "thread/uploadthread.h"
#include "thread/tftpserverthread.h"
#include "thread/statusfilerecvthread.h"
#include "singleton/singleton.h"
#include "global.h"
extern std::mutex menuThreadMutex;
extern std::condition_variable menuThreadCv;
FirmwareUpgrade::FirmwareUpgrade()
{
    logger = &Singleton<spdlog::logger>::Instance();
    pool.setMaxThreadCount(16);


    //状态文件监听线程
    statusFileRcvThread = &Singleton<StatusFileRcvThread>::Instance();
    //pool.start(statusFileRcvThread);
    statusFileRcvThread->start();

    //TFTP服务线程
    tftpServerThread = &Singleton<TftpServerThread>::Instance();
    tftpServerThread->start();


    //注册数据类型
    qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");

    //菜单线程
    menuThread = &Singleton<MenuThread>::Instance();
    connect(menuThread, &MenuThread::sendMenuOp, this, &FirmwareUpgrade::onRecvdMenuOp);
    pool.start(menuThread);



}

FirmwareUpgrade::~FirmwareUpgrade()
{
    logger->info("quit FirewareUpgrade");
    statusFileRcvThread->threadExit();
    tftpServerThread->quit();
}

void FirmwareUpgrade::onRecvdMenuOp(int op, const std::vector<std::string>& args)
{
    switch (op) {
    case FIND:{
        findThread = new FindThread();
        connect(findThread, &FindThread::findOperationFinished, this, [&](std::vector<Device> devices){
            logger->info("-------------------------------------");
            logger->info("{0} device found", devices.size());
            for(decltype (devices.size()) i = 0; i < devices.size(); ++i){
                logger->info("Device{0}:{1}", i + 1, devices[i].toString());
            }
            logger->info("-------------------------------------");
            this->devices = std::move(devices);
            findThread->quit();
            findThread->wait();
            delete findThread;
            findThread = nullptr;
            std::unique_lock<std::mutex> ulock(menuThreadMutex);
            menuThreadCv.notify_one();
        });
        findThread->start();
        break;
    }
    case UPLOAD:{
        QStringList list;
        auto deviceId = atoi(args.at(0).data());
        for(auto file = args.begin() + 1; file != args.end(); ++file){
            list.append(QString::fromStdString(*file));
        }
        if(deviceId >= 1 && (uint)deviceId <= devices.size()){
            UploadThread* uploadThread = new UploadThread(list, &devices[deviceId - 1], nullptr);
            tftpServerThread->addThread(devices[deviceId - 1].getHostAddress(), uploadThread);
            statusFileRcvThread->addThread(devices[deviceId - 1].getHostAddress(), uploadThread);
            uploadThread->setAutoDelete(true);
            pool.start(uploadThread);
        }
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



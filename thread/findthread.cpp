#include "findthread.h"
#include "global.h"
#include <QUdpSocket>
#include <iostream>
#include <QEventLoop>
#include <QTimer>
#include <condition_variable>
//#include <format>
#include <mutex>
#include "singleton/singleton.h"
extern std::mutex menuThreadMutex;
extern std::condition_variable menuThreadCv;

FindThread::FindThread():
    logger(&Singleton<spdlog::logger>::Instance())
{

}

FindThread::~FindThread()
{
    //std::cout << "findthread deconstructor" << std::endl;
}

void FindThread::run()
{
    uSock = std::make_shared<QUdpSocket>();
    uSock->bind(QHostAddress::AnyIPv4, FIND_PORT, QUdpSocket::ShareAddress);
    uSock->writeDatagram(makeFindRequest().data(), makeFindRequest().size(), QHostAddress("10.68.141.129"), FIND_PORT);
    connect(uSock.get(), &QUdpSocket::readyRead, this, &FindThread::onFindResponseRecved);

    timer = std::make_shared<QTimer>();
    timer->setInterval(3000);
    QEventLoop eventLoop;
    connect(timer.get(), &QTimer::timeout, this, [&eventLoop](){
        std::unique_lock<std::mutex> ulock(menuThreadMutex);
        menuThreadCv.notify_one();
        eventLoop.quit();
    });
    timer->start();
    eventLoop.exec();
}

const QByteArray& FindThread::makeFindRequest()
{
    static QByteArray findRequest("\x00\x01\x00\x10", 4);
    return findRequest;
}

void FindThread::onFindResponseRecved()
{
    QByteArray datagram;
    QHostAddress remoteAddress;
    //让datagrama的大小为等待处理的数据包的大小，这样才能接收到完整的数据
    datagram.resize(uSock->pendingDatagramSize());
    //接收数据报，将其存放到datagram中
    uSock->readDatagram(datagram.data(), datagram.size(), &remoteAddress);
    datagram = datagram.trimmed();
    //去掉FIND相应报文的头部，获取内容
    if(datagram.size() < 2 || strcmp(datagram.left(2).data(), "\x00\x02")){
        return;
    }
    QString info = QString::fromStdString(datagram.mid(2).toStdString());
    QStringList deviceInfo;
    //首先加上设备端的IP地址
    deviceInfo << remoteAddress.toString();
    //然后获取以0作为间隔的FIND相应内容
    deviceInfo << info.split('\0');
    //去掉所有的空字符串项和非字符项
    int i = 0;
    for(; i < deviceInfo.size(); i++){
        if(deviceInfo[i] == "") {
            deviceInfo.removeAt(i);
            i = -1;
        }
        else{
            for(int j = 0; j < deviceInfo[i].size(); j++){
                if(deviceInfo[i][j] < 32 || deviceInfo[i][j] > 126){
                    deviceInfo.removeAt(i);
                    break;
                    i = -1;
                }
            }
        }
    }
    std::string devInfoString = "[";
    for(auto& deviceInfo: deviceInfo.toStdList()){
        devInfoString += deviceInfo.toStdString() + " ";
    }
    devInfoString += "]";
    logger->info("[{0} {1} {2} {3} {4} {5}]", deviceInfo[0].toStdString(),
            deviceInfo[1].toStdString(), deviceInfo[2].toStdString(),
            deviceInfo[3].toStdString(), deviceInfo[4].toStdString(),
            deviceInfo[5].toStdString());
    //logger->info(devInfoString);
    //logger->log(deviceInfo.toStdList());
}

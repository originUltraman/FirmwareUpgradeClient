#include "findthread.h"
#include "global.h"
#include <QUdpSocket>
#include <iostream>
#include <QEventLoop>
#include <QTimer>
#include <condition_variable>
//#include <format>
#include <mutex>
#include <QThread>
#include "singleton/singleton.h"


FindThread::FindThread(QObject *parent):
    QThread(parent),
    logger(&Singleton<spdlog::logger>::Instance())
{
    moveToThread(this);
}

FindThread::~FindThread()
{
    uSock->deleteLater();
    timer->deleteLater();
}

void FindThread::run()
{
    qRegisterMetaType<std::vector<Device>>("std::vector<Device>");
    uSock = new QUdpSocket();
    if(uSock->bind(QHostAddress::AnyIPv4, FIND_PORT, QUdpSocket::ShareAddress) == false){
        logger->info("bind error");
        return;
    }
    uSock->writeDatagram(makeFindRequest().data(), makeFindRequest().size(), QHostAddress("10.68.141.129"), FIND_PORT);
    connect(uSock, &QUdpSocket::readyRead, this, &FindThread::onFindResponseRecved);
    timer = new QTimer();
    timer->setInterval(3000);
    connect(timer, &QTimer::timeout, this, &FindThread::onTimerTimeout);
    timer->start();
    exec();
}

const QByteArray& FindThread::makeFindRequest()
{
    static QByteArray findRequest("\x00\x01\x00\x10", 4);
    return findRequest;
}

void FindThread::onFindResponseRecved()
{
    //logger->info(QThread::currentThreadId());
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
    devices.emplace_back(deviceInfo[0].toStdString(),
            deviceInfo[1].toStdString(), deviceInfo[2].toStdString(),
            deviceInfo[3].toStdString(), deviceInfo[4].toStdString(),
            deviceInfo[5].toStdString());
}

void FindThread::onTimerTimeout()
{
    timer->stop();
    uSock->close();
    emit findOperationFinished(std::move(devices));
}

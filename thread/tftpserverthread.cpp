#include "tftpserverthread.h"
#include "thread/thread.h"
#include "thread/statusfilerecvthread.h"
#include "tftp/tftprequest.h"
#include "singleton/singleton.h"
#include <QEventLoop>

TftpServerThread::TftpServerThread():logger(Singleton<spdlog::logger>::Instance()){
    moveToThread(this);
}

void TftpServerThread::run()
{
    uSock = std::make_shared<QUdpSocket>();
    if(uSock->bind(QHostAddress::AnyIPv4, TFTP_SERVER_PORT, QUdpSocket::ShareAddress) == false){
        logger.error("PORT {} ALREADY IN USE", TFTP_SERVER_PORT);
        return;
    }
    logger.info("tftp server init success");
    connect(uSock.get(), &QUdpSocket::readyRead, this, &TftpServerThread::onUSockReadyRead);
    exec();
}

void TftpServerThread::addThread(const std::string &host, class thread* thread_)
{
    hostToThread.insert(std::make_pair(host, thread_));
}

void TftpServerThread::removeThread(const std::string &host)
{
    hostToThread.erase(host);
}



void TftpServerThread::onUSockReadyRead()
{
    while(uSock->hasPendingDatagrams()){
        QHostAddress host;
        quint16 port;
        QByteArray datagram;
        datagram.resize(uSock->pendingDatagramSize());
        uSock->readDatagram(datagram.data(), datagram.size(), &host, &port);
        TftpRequest tftpRequest(datagram.data(), datagram.size(), host.toString().toStdString(), port);
        logger.info(fmt::format("request file name is {}", tftpRequest.getFileName()));

        if(tftpRequest.getFileName().find(".LUS") != std::string::npos || tftpRequest.getFileName().find(".LUS") != std::string::npos){
            logger.info("LUS");
            Singleton<StatusFileRcvThread>::Instance().addTftpRequest(tftpRequest);
            logger.info("filename{} host{} port{} blksize{} timeout{} retry{}", tftpRequest.getFileName(), tftpRequest.getHost(),
                        tftpRequest.getPort(), tftpRequest.getBlksize(), tftpRequest.getTimeout(), tftpRequest.getRetry());
        }
        else{
            logger.info("no LUS");
            if(hostToThread.count(host.toString().toStdString())){
                hostToThread[host.toString().toStdString()]->addTftpRequest(tftpRequest);
            }
        }
    }
}

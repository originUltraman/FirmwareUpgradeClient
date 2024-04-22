#include "tftpserverthread.h"
#include "thread/thread.h"
#include "thread/statusfilerecvthread.h"
#include <QEventLoop>

TftpServerThread::TftpServerThread(){

}

void TftpServerThread::run()
{
    QEventLoop eventLoop;
    uSock = std::make_shared<QUdpSocket>();
    connect(uSock.get(), &QUdpSocket::readyRead, this, &TftpServerThread::onUSockReadyRead);
    eventLoop.exec();
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
        QHostAddress remote;
        quint16 port;
        QByteArray datagram;
        QString fileName;
        datagram.resize(uSock->pendingDatagramSize());
        fileName = datagram.mid(2).split('\0').at(0);
        uSock->readDatagram(datagram.data(), datagram.size(), &remote, &port);
        TftpRequest tftpRequest(datagram, port);
        if(hostToThread.count(remote.toString().toStdString())){
            hostToThread[remote.toString().toStdString()]->addTftpRequest(std::move(tftpRequest));
        }
        else if(fileName.endsWith(".LUS") || fileName.endsWith(".LNS") || fileName.endsWith(".LCS")){
            //class thread* statusFileRcvThread = new StatusFileRcvThread();
        }
    }
}

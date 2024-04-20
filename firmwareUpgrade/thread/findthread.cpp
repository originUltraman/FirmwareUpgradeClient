#include "findthread.h"
#include "global.h"
#include <QUdpSocket>
#include <iostream>
#include <QEventLoop>

FindThread::FindThread()
{

}

FindThread::~FindThread()
{
    std::cout << "findthread deconstructor" << std::endl;
}

void FindThread::run()
{
    uSock = std::make_shared<QUdpSocket>();
    uSock->bind(QHostAddress::AnyIPv4, FIND_PORT, QUdpSocket::ShareAddress);
    uSock->writeDatagram(makeFindRequest().data(), makeFindRequest().size(), QHostAddress("10.68.141.129"), FIND_PORT);
    QEventLoop eventLoop;
    eventLoop.exec();
}

const QByteArray& FindThread::makeFindRequest()
{
    static QByteArray findRequest("\x00\x01\x00\x10", 4);
    return findRequest;
}

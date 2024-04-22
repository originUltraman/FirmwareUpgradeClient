#include "thread/statusfilerecvthread.h"


void StatusFileRcvThread::run()
{
//    QString errorMessage;
//    statusFileSocket = std::make_shared<QUdpSocket>();
//    if(statusFileSocket->bind(STATUS_FILE_PORT) == false){
//        logger.error("PORT %1 HAS BEEN OCCUPIED", STATUS_FILE_PORT);
//        return;
//    }
//    QByteArray request = tftpRequest->getRequest();
//    quint16 port = tftpRequest->getPort();
//    QString fileName = request.mid(2).split('\0').at(0);
//    QString statusMessage;
//    const unsigned short DLP_retry = 2;
//    unsigned short tries = 0;
//    while(!Tftp::handlePut(statusFileSocket.get(), dir.dirName(), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, request)
//          && ++tries < DLP_retry + 1){
//    }
//    if(tries >= DLP_retry + 1){
//        logger.error("EXCEED MAX RETRANSMIT OF DLP, GET STATUS FILE {0} FAILED", fileName);
//    }
//    else{
//        emit statusFileRcvFinishedSignal();
//    }
//    statusFileSocket->close();
}


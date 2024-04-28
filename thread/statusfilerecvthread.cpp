#include "thread/statusfilerecvthread.h"
#include "spdlog/logger.h"
#include "singleton/singleton.h"
#include "tftp/tftprequest.h"
extern unsigned short dlpRetry_default;
class thread;
StatusFileRcvThread::StatusFileRcvThread(QObject *parent):
    QThread(parent),
    logger(Singleton<spdlog::logger>::Instance())
{
    moveToThread(this);
}

void StatusFileRcvThread::run()
{
    logger.info("statusFileThread init success");
    QUdpSocket* uSock = new QUdpSocket();
    TftpRequest request;
    std::string errorMessage;
    unsigned short dlpTry = 0;
    if(uSock->bind(QHostAddress::AnyIPv4, STATUS_FILE_PORT, QUdpSocket::ShareAddress) == false){
        logger.info("PORT %1 HAS BEEN OCCUPIED", STATUS_FILE_PORT);
        return;
    }
    logger.info("statusFileThread init success");
    while(!exitFlag){
        while(!exitFlag && (tftpRequests.empty() || !tftpRequests.dequeue(request))){
            std::unique_lock<std::mutex> ulock(m);
            cv.wait(ulock);
        }
        logger.info("statusFileThread listened");
        //const unsigned short DLP_retry = DLP
        while(!Tftp::handlePut(uSock, request, errorMessage) && ++dlpTry < dlpRetry_default){

        }
        if(dlpTry >= dlpRetry_default + 1){
            logger.error("EXCEED MAX RETRANSMIT OF DLP, GET STATUS FILE {0} FAILED", request.getFileName());
        }
        else{
            if(hostToThread.count(request.getHost())){
                logger.info("found in hostToThread");
                hostToThread[request.getHost()]->parseStatusFile();
            }
        }     
    }
    uSock->close();
    logger.info("statusFileThread finished");
}

void StatusFileRcvThread::addTftpRequest(TftpRequest &tftpRequest)
{
    std::unique_lock<std::mutex> ulock(m);
    tftpRequests.enqueue(std::move(tftpRequest));
    cv.notify_one();
}

void StatusFileRcvThread::addThread(const std::string &host, class thread* thread_)
{
    hostToThread.insert(std::make_pair(host, thread_));
}

void StatusFileRcvThread::threadExit()
{
    std::unique_lock<std::mutex> ulock(m);
    exitFlag = true;
    cv.notify_one();
}


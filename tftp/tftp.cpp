#include "tftp.h"
#include "tftp/tftprequest.h"
#include "tftp/tftpoack.h"
#include "spdlog/common.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <QUdpSocket>
#include"spdlog/fmt/bundled/core.h"
extern unsigned short blksize_default;
extern unsigned short timeout_default;
extern unsigned short retry_default;




unsigned short Tftp::getBlockNo(char *buf, unsigned short)
{
    unsigned short high = static_cast<unsigned char>(buf[2]);
    unsigned short low = static_cast<unsigned char>(buf[3]);
    unsigned short blockNo = (high << 8) + low;
    return blockNo;
}

bool Tftp::put(QUdpSocket *uSock, const TftpRequest &request, std::string &errorMessage, std::string path)
{
    char sendBuf[512];
    char recvBuf[512];
    unsigned short newPort;
    auto sendLen = makeTftpRWRequest(request, sendBuf);
    int recvLen = 0;
    unsigned short try_ = 0;
    if(sendLen < 0){
        errorMessage = "MAKE TFTP REQUEST ERROR";
        return false;
    }
    do{
        uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(request.getHost())), request.getPort());
    }while(try_++ < request.getRetry() &&
           (!uSock->waitForReadyRead(request.getTimeout() * 1000) ||
             uSock->pendingDatagramSize() <= 0 ||
             (recvLen = uSock->readDatagram(recvBuf, 512, nullptr, &newPort)) <= 0));
    if(try_ >= request.getRetry()){
        errorMessage = "TIME OUT FOR OACK";
        return false;
    }
    TftpOAck oAck(recvBuf, recvLen);
    oAck.setBlksize(512);
    oAck.setRetry(5);
    oAck.setTimeout(2);
    qDebug() << "port = " <<  newPort;
    return upload(uSock, path + "/" + request.getFileName(), request.getHost(), newPort, errorMessage, oAck);

}

bool Tftp::get(QUdpSocket *uSock, const TftpRequest &request, std::string &errorMessage, std::string path)
{
    char sendBuf[512];
    char recvBuf[512];
    unsigned short newPort;
    auto sendLen = makeTftpRWRequest(request, sendBuf);
    int recvLen = 0;
    unsigned short try_ = 0;
    if(sendLen < 0){
        errorMessage = "MAKE TFTP REQUEST ERROR";
        return false;
    }
    do{
        uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(request.getHost())), request.getPort());
    }while(try_++ < request.getRetry() &&
           (!uSock->waitForReadyRead(request.getTimeout() * 1000) ||
           uSock->pendingDatagramSize() <= 0 ||
           (recvLen = uSock->readDatagram(recvBuf, 512, nullptr, &newPort)) <= 0));
    if(try_++ >= request.getRetry()){
        errorMessage = "TIME OUT FOR OACK";
        return false;
    }

    TftpOAck oAck(recvBuf, recvLen);
    oAck.setBlksize(512);
    oAck.setRetry(5);
    oAck.setTimeout(2);
    sendLen = makeTftpAck(0, sendBuf);
    uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(request.getHost())), newPort);
    return download(uSock, path + "/" + request.getFileName(), request.getHost(), newPort, errorMessage, oAck, sendBuf, sendLen);
}

bool Tftp::handlePut(QUdpSocket *uSock, const TftpRequest &request, std::string &errorMessage, const std::string path)
{
    TftpOAck oAck;
    oAck.setBlksize(request.getBlksize() < blksize_default ? request.getBlksize() : blksize_default);
    oAck.setRetry(request.getRetry() < retry_default ? request.getRetry() : retry_default);
    oAck.setTimeout(request.getTimeout() < timeout_default ? request.getTimeout() : timeout_default);
    char sendBuf[512];
    int sendLen = 0;
    sendLen = makeTftpOAck(oAck, sendBuf);
    uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(request.getHost())), request.getPort());
    return download(uSock, path + "/" + request.getFileName(), request.getHost(), request.getPort(), errorMessage, oAck, sendBuf, sendLen);
}

bool Tftp::handleGet(QUdpSocket *uSock, const TftpRequest &request, std::string &errorMessage, const std::string path)
{
    TftpOAck oAck;
    oAck.setBlksize(request.getBlksize() < blksize_default ? request.getBlksize() : blksize_default);
    oAck.setRetry(request.getRetry() < retry_default ? request.getRetry() : retry_default);
    oAck.setTimeout(request.getTimeout() < timeout_default ? request.getTimeout() : timeout_default);
    char sendBuf[512];
    char recvBuf[512];
    int sendLen = 0, recvLen = 0;
    unsigned short port;
    sendLen = makeTftpOAck(oAck, sendBuf);
    uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(request.getHost())), request.getPort());
    unsigned short try_ = 0;
    while(true){
        while(!uSock->waitForReadyRead(oAck.getTimeout() * 1000) && ++try_ < oAck.getRetry() + 1){}
        if(try_ >= oAck.getRetry() + 1 || uSock->pendingDatagramSize() > 0) break;
    }
    if(try_ >= oAck.getRetry() + 1){
        errorMessage = "TIME OUT FOR ACK 0";
        return false;
    }
    recvLen = uSock->readDatagram(recvBuf, 512, nullptr, &port);

    return upload(uSock, path + "/" + request.getFileName(), request.getHost(), port, errorMessage, oAck);
}

bool Tftp::download(QUdpSocket* uSock, const std::string& absolutePath, const std::string& host, const unsigned short port, std::string& errorMessage, const TftpOAck& oAck, char* lastPacket, unsigned short lastPacketLen)
{
    std::ofstream out;
    out.open(absolutePath);
    if(!out){
        errorMessage = fmt::format("FILE {} OPEN ERROR", absolutePath);
        return false;
    }
    char* sendBuf = lastPacket;
    //memcpy(sendBuf, lastPacket, lastPacketLen);
    char* recvBuf = new char[oAck.getBlksize() + 4];
    int sendLen = lastPacketLen, recvLen = 0;
    unsigned short try_ = 1, expectedDataNo = 1;
    do{
        while(try_ < oAck.getRetry() + 1 &&
              (!uSock->waitForReadyRead(oAck.getTimeout() * 1000) ||
               (recvLen = uSock->readDatagram(recvBuf, oAck.getBlksize() + 4, nullptr, nullptr)) <= 0 ||
              getBlockNo(recvBuf, recvLen) != expectedDataNo)
              ){
            uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(host)), port);
            ++try_;
        }
        if(try_ >= oAck.getRetry() + 1){
            errorMessage = "TIME OUT FOR DATA PACKET";
            delete[] recvBuf;
            return false;
        }
        out.write(recvBuf + 4, recvLen - 4);
        sendLen = makeTftpAck(expectedDataNo++, sendBuf);
        uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(host)), port);
        try_ = 1;
    }while(recvLen - 4 == oAck.getBlksize());
    out.close();
    delete[] recvBuf;
    return true;
}

bool Tftp::upload(QUdpSocket* uSock, const std::string& absolutePath, const std::string& host, const unsigned short port, std::string& errorMessage, const TftpOAck& oAck)
{
    std::ifstream in;
    in.open(absolutePath, std::ios::binary);
    if(!in){
        errorMessage = fmt::format("FILE {} OPEN ERROR", absolutePath);
        return false;
    }
    char* sendBuf = new char[oAck.getBlksize() + 4];
    char recvBuf[128];
    int sendLen = 0, recvLen = 0;
    unsigned short blockNo = 1, try_ = 0;
    do{
        //qDebug() << "oAck.blksize " << oAck.getBlksize();

        in.read(sendBuf + 4, oAck.getBlksize());

        //qDebug() << "in.gcount = " << in.gcount();

        sendLen = makeTftpData(sendBuf, in.gcount(), blockNo);

        //qDebug() << "sendLen = " << sendLen;

        uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(host)), port);
        try_ = 1;
        while(true){
            while(!uSock->waitForReadyRead(oAck.getTimeout() * 1000) && try_ < oAck.getRetry() + 1){
                uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(host)), port);
                ++try_;
            }
            if(try_ >= oAck.getRetry() + 1){
                errorMessage = fmt::format("TIME OUT FOR ACK {}", blockNo);
                delete[] sendBuf;
                return false;
            }
            while(uSock->pendingDatagramSize() > 0){
                recvLen = uSock->readDatagram(recvBuf, 128, nullptr, nullptr);
            }
            if(getBlockNo(recvBuf, recvLen) == blockNo) break;
            uSock->writeDatagram(sendBuf, sendLen, QHostAddress(QString::fromStdString(host)), port);
            ++try_;
        }
        ++blockNo;
    }while(sendLen == oAck.getBlksize() + 4);
    in.close();
    delete[] sendBuf;
    return true;
}





int Tftp::makeTftpRWRequest(const TftpRequest &request, char *buf)
{
    char* curp = buf;
    char digits[128];
    buf[0] = 0;
    buf[1] = request.opcode;
    curp += 2;
    memcpy(curp, request.getFileName().data(), strlen(request.getFileName().data()) + 1);
    curp += strlen(request.getFileName().data()) + 1;
    memcpy(curp, request.getMode().data(), strlen(request.getMode().data()) + 1);
    curp += strlen(request.getMode().data()) + 1;
    memcpy(curp, "blksize", sizeof("blksize"));
    curp += sizeof("blksize");
    memcpy(curp, digits, sprintf(digits, "%hu", request.getBlksize()) + 1);
    curp += strlen(digits) + 1;
    memcpy(curp, "timeout", sizeof("timeout"));
    curp += sizeof("timeout");
    memcpy(curp, digits, sprintf(digits, "%hu", request.getTimeout()) + 1);
    curp += strlen(digits) + 1;
    memcpy(curp, "retry", sizeof("retry"));
    curp += sizeof("retry");
    memcpy(curp, digits, sprintf(digits, "%hu", request.getRetry()) + 1);
    curp += strlen(digits) + 1;
    return curp -  buf;
}

int Tftp::makeTftpData(char buf[], int len, unsigned short block)
{
    buf[0] = 0;
    buf[1] = 3;
    buf[2] = block >> 8;
    buf[3] = block;
    return 4 + len;
}

int Tftp::makeTftpAck(unsigned short ack, char* buf)
{
    buf[0] = 0;
    buf[1] = 4;
    buf[2] = ack >> 8;
    buf[3] = ack;
    return 4;
}

int Tftp::makeTftpError(unsigned short errorCode, std::string errorMessage)
{

}

int Tftp::makeTftpOAck(const TftpOAck& oAck, char* buf)
{
    char* curp = buf;
    char digits[128];
    buf[0] = 0;
    buf[1] = 6;
    curp += 2;
    memcpy(curp, "blksize", sizeof("blksize"));
    curp += sizeof("blksize");
    //memcpy(curp, itoa())
    memcpy(curp, digits, sprintf(digits, "%hu", oAck.getBlksize()) + 1);
    curp += strlen(digits) + 1;
    memcpy(curp, "timeout", sizeof("timeout"));
    curp += sizeof("timeout");
    memcpy(curp, digits, sprintf(digits, "%hu", oAck.getTimeout()) + 1);
    curp += strlen(digits) + 1;
    memcpy(curp, "max-retransmit", sizeof("max-retransmit"));
    curp += sizeof("max-retransmit");
    memcpy(curp, digits, sprintf(digits, "%hu", oAck.getRetry()) + 1);
    curp += strlen(digits) + 1;
    return curp - buf;
}

int Tftp::makeTftpAbort(unsigned short statusCode)
{

}

template<typename... Args>
std::pair<std::unique_ptr<char>, int> Tftp::makeTftpPacket(const TftpPacketType type, Args&&... args)
{
    std::unique_ptr<char> data = nullptr;
    int len = 0;
    switch(type){
    case RRQ:
        len = makeTftpRWRequest(std::forward<Args>(args)..., data.get());
        break;
    case WRQ:
        len = makeTftpRWRequest(std::forward<Args>(args)..., data.get());
        break;
    case DATA:
        len = makeTftpData(std::forward<Args>(args)..., data.get());
        break;
    case ACK:
        len = makeTftpAck(std::forward<Args>(args)..., data.get());
        break;
    default:
        break;
    }
    //std::pair<std::unique_ptr<char>, int> p{data, len};
    return std::make_pair(std::move(data), len);
}

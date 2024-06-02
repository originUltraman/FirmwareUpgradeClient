#ifndef TFTP_H
#define TFTP_H
//#include"tftp/tftprequest.h"
#include<string>
#include<memory>
class QUdpSocket;
class TftpRequest;
class TftpOAck;
extern unsigned short dlpRetry_default;
class Tftp
{
public:
    Tftp();
    enum TftpPacketType{RRQ = 1, WRQ, DATA, ACK, ERROR, OACK, UNDEFINED};
    static void parseOACK(char* buf, unsigned short len);

    static unsigned short getBlockNo(char* buf, unsigned short len);

    static bool put(QUdpSocket* uSock, const TftpRequest& request, std::string& errorMessage, std::string path = ".");
    static bool get(QUdpSocket* uSock, const TftpRequest& request, std::string& errorMessage, std::string path = ".");
    static bool handlePut(QUdpSocket* uSock, const TftpRequest& request, std::string& errorMessage, const std::string path = ".");
    static bool handleGet(QUdpSocket* uSock, const TftpRequest& request, std::string& errorMessage, const std::string path = ".");

    static bool download(QUdpSocket* uSock, const std::string& absolutePath, const std::string& host, const unsigned short port, std::string& errorMessage, const TftpOAck& oAck, char* lastPacket, unsigned short lastPacketLen);
    static bool upload(QUdpSocket* uSock, const std::string& absolutePath, const std::string& host, const unsigned short port, std::string& errorMessage, const TftpOAck& oAck);

    template<typename... Args>
    std::pair<std::unique_ptr<char>, int> makeTftpPacket(const TftpPacketType, Args&&...);
    static int makeTftpRWRequest(const TftpRequest& tftpRequest, char* buf);
    static int makeTftpData(char buf[], int len, unsigned short block);
    static int makeTftpAck(unsigned short ack, char* buf);
    static int makeTftpError(unsigned short errorCode, std::string errorMessage);
    static int makeTftpOAck(const TftpOAck& oAck, char* buf);


    static int makeTftpAbort(unsigned short statusCode);

    //static TftpPacketType getTftpPacketType(const QByteArray& tftpPacket);
    //static bool checkBlockNo(const QByteArray& tftpPacket, unsigned short No);
    //static bool checkAckNo(const QByteArray& tftpPacket, unsigned short No);
    //static unsigned short getNo(const QByteArray& tftpPacket);
};
#endif // TFTP_H


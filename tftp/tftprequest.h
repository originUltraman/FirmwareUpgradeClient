#ifndef TFTPREQUEST_H
#define TFTPREQUEST_H
#include<string>
#include<cstring>
#include<iostream>
#include<algorithm>
extern unsigned short blksize_default;
extern unsigned short timeout_default;
extern unsigned short retry_default;
class TftpRequest
{
public:

    enum OpCode{RRQ = 1, WRQ} opcode;
    TftpRequest(){

    }
    TftpRequest(const char* data, const unsigned int size, const std::string& host, const unsigned short port){
        this->host = std::move(host);
        this->port = port;
        opcode = (OpCode)data[1];
        //strcpy(fileName.c_str(), &data[2]);
        fileName = std::string(&data[2]);


        char target[64] = "blksize";
        const char* pos;
        if((pos = std::search(data, data + size, target, target + strlen(target))) != data + size){
            pos += sizeof("blksize");
            this->blksize = atoi(pos);
        }
        strcpy(target, "timeout");
        if((pos = std::search(data, data + size, target, target + strlen(target))) != data + size){
            pos += sizeof("timeout");
            this->timeout = atoi(pos);
        }
        strcpy(target, "retry");
        if((pos = std::search(data, data + size, target, target + strlen(target))) != data + size){
            pos += sizeof("retry");
            this->retry = atoi(pos);
        }
        strcpy(target, "mode");
        if((pos = std::search(data, data + size, target, target + strlen(target))) != data + size){
            pos += sizeof("mode");
            mode = std::string(pos);
        }
    }
    TftpRequest(enum OpCode opcode, const std::string& fileName, const std::string& host, unsigned short port,
                unsigned short blksize = blksize_default, unsigned short timeout = timeout_default,
                unsigned short retry = retry_default, std::string mode = "octet"):
        opcode(opcode), fileName(fileName), host(host), port(port), mode(mode), blksize(blksize),
        timeout(timeout), retry(retry)
    {

    }
    ~TftpRequest(){

    }
    std::string getFileName()const{
        return fileName;
    }
    unsigned short getPort()const{
        return port;
    }
    unsigned short getBlksize() const{
        return blksize;
    }
    unsigned short getTimeout() const{
        return timeout;
    }
    unsigned short getRetry() const{
        return retry;
    }
    std::string getHost() const{
        return host;
    }
    std::string getMode()const{
        return mode;
    }

    void setPort(unsigned short newPort){
        this->port = newPort;
    }

private:
    std::string host;
    std::string fileName;
    unsigned short port;
    std::string mode = "octet";
    unsigned short blksize = 512;
    unsigned short timeout = 2;
    unsigned short retry = 2;
};

#endif // TFTPREQUEST_H


#ifndef TFTPOACK_H
#define TFTPOACK_H
#include<string>
#include<cstring>
#include<algorithm>
class TftpOAck
{
public:
    TftpOAck(){}
    TftpOAck(const char* data, const unsigned int size){
        char target[64] = "blksize";
        const char* pos;
        if((pos = std::search(data, data + size, target, target + strlen(target))) != data + size){
            this->blksize = atoi(pos);
        }
        strcpy(target, "timeout");
        if((pos = std::search(data, data + size, target, target + strlen(target))) != data + size){
            this->timeout = atoi(pos);
        }
        strcpy(target, "retry");
        if((pos = std::search(data, data + size, target, target + strlen(target))) != data + size){
            this->retry = atoi(pos);
        }
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

    void setTimeout(unsigned short newTimeout){
        timeout = newTimeout;
    }

    void setRetry(unsigned short newRetry){
        retry = newRetry;
    }

    void setBlksize(unsigned short newBlksize){
        blksize = newBlksize;
    }

private:
    unsigned short timeout;
    unsigned short retry;
    unsigned short blksize;
};

#endif // TFTPOACK_H

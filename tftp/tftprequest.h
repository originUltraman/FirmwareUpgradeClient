#ifndef TFTPREQUEST_H
#define TFTPREQUEST_H

#include <QObject>
class TftpRequest
{
public:
    TftpRequest(){}
    TftpRequest(QByteArray request, quint16 port):
        request(request), port(port)
    {

    }
    TftpRequest(TftpRequest&& tftpRequest)noexcept{
        request = std::move(tftpRequest.request);
        port = std::move(tftpRequest.port);
    }
    ~TftpRequest(){

    }
    QByteArray getRequest(){
        return request;
    }
    quint16 getPort(){
        return port;
    }
private:
    QByteArray request;
    quint16 port;
};

#endif // TFTPREQUEST_H


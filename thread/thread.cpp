#include "thread.h"

thread::thread(const Device *device, QObject *parent):
    QObject(nullptr),
    device(device),
    logger(Singleton<spdlog::logger>::Instance()){
    //创建一个虚拟连接，只接受目的设备端发来的信息
    QDir tmpDir(QDir::currentPath() + "/" + device->getName() + "_" + device->getHostAddress());
    if(!tmpDir.exists()){
        qDebug() << QDir::currentPath();
        if(!tmpDir.mkpath(QDir::currentPath() + "/" + device->getName() + "_" + device->getHostAddress())) qDebug() << "文件夹创建失败";
        else qDebug() << "文件创建成功";
    }
    dir = tmpDir;
}

thread::~thread(){
    //if(tftpRequest) delete tftpRequest;
    //qDebug() << "MyThread的析构函数\n";
}

const QHostAddress thread::getHostAddress() const{
    return QHostAddress(this->device->getHostAddress());
}

const Device*thread::getDevice() const{
    return device;
}

void thread::addTftpRequest(TftpRequest &&tftpRequest)
{
    tftpRequests.enqueue(std::forward<TftpRequest>(tftpRequest));
}


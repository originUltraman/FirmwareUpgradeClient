#include "thread.h"
#include "singleton/singleton.h"
thread::thread(const Device *device, QObject *parent):
    QObject(nullptr),
    device(device),
    logger(Singleton<spdlog::logger>::Instance()){
    //创建一个虚拟连接，只接受目的设备端发来的信息
    QDir tmpDir(QDir::currentPath() + "/" + QString::fromStdString(device->getName()) + "_" + QString::fromStdString(device->getHostAddress()));
    if(!tmpDir.exists()){
        qDebug() << QDir::currentPath();
        if(!tmpDir.mkpath(QDir::currentPath() + "/" +
                          QString::fromStdString(device->getName()) + "_" +
                          QString::fromStdString(device->getHostAddress()))) {
            qDebug() << "文件夹创建失败";
        }

        else qDebug() << "文件创建成功";
    }
    dir = tmpDir;
//    info = std::bind(&spdlog::logger::info<>, &this->logger, std::placeholders::_1);
//    debug = std::bind(&spdlog::logger::debug<>, &this->logger, std::placeholders::_1);
//    error = std::bind(&spdlog::logger::error<>, &this->logger, std::placeholders::_1);
}

thread::~thread(){
    //if(tftpRequest) delete tftpRequest;
    //qDebug() << "MyThread的析构函数\n";
}

const QHostAddress thread::getHostAddress() const{
    return QHostAddress(QString::fromStdString(this->device->getHostAddress()));
}

const Device*thread::getDevice() const{
    return device;
}

void thread::addTftpRequest(const TftpRequest& tftpRequest)
{
    std::unique_lock<std::mutex> ul(m);
    tftpRequests.enqueue(tftpRequest);
    cv.notify_one();
    //std::cout << device->getHostAddress() << std::endl;
}


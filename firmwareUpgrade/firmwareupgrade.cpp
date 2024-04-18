#include "firmwareupgrade.h"
#include <iostream>
#include "inputListener.h"

FirmwareUpgrade::FirmwareUpgrade()
{
    pool.setMaxThreadCount(16);
    timer = std::make_shared<QTimer>(this);
    std::cout << "Welcome to FirmwareUpgrade System.\n"
                 "1. Find\n"
                 "2. Upload\n"
                 "3. Donwload" << std::endl;

//    timer->setInterval(1000); // 设置定时器间隔为100毫秒
//    QTextStream stdinStream(stdin);
//    QObject::connect(timer.get(), &QTimer::timeout, [&]() {
////        if (std::cin.gcount() > 0) {
////            QString input = stdinStream.readLine(); // 读取输入的一行
////            qDebug() << "User input:" << input;
////        }
//        std::cout << "hello world" << std::endl;
//    });
//    timer->start(); // 启动定时器
//    std::cout << "------" << std::endl;

    // 创建输入监听器对象
    InputListener inputListener;

    // 连接输入监听器的信号与槽
    QObject::connect(&inputListener, &InputListener::findRequested, [](const QString &ipAddress){
        std::cout << "Find requested for IP:" << ipAddress.toStdString() << std::endl;
        // 执行 find 操作
    });

    QObject::connect(&inputListener, &InputListener::uploadRequested, [](const QString &ipAddress, const QStringList &filenames){
        std::cout << "Upload requested for IP:" << ipAddress.toStdString() << "with filenames:";
        for (const QString &filename : filenames) {
            std::cout << " " << filename.toStdString();
        }
        std::cout << std::endl;
        // 执行 upload 操作
    });

    QObject::connect(&inputListener, &InputListener::downloadRequested, [](const QString &ipAddress, const QStringList &filenames){
        std::cout << "Download requested for IP:" << ipAddress.toStdString() << "with filenames:";
        for (const QString &filename : filenames) {
            std::cout << " " << filename.toStdString();
        }
        std::cout << std::endl;
        // 执行 download 操作
    });
}



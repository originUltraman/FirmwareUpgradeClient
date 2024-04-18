#include "firmwareupgrade.h"
#include <iostream>

FirmwareUpgrade::FirmwareUpgrade()
{
    pool.setMaxThreadCount(16);
    timer = std::make_shared<QTimer>(this);
    std::cout << "Welcome to FirmwareUpgrade System.\n"
                 "1. Find\n"
                 "2. Upload\n"
                 "3. Donwload" << std::endl;

    timer->setInterval(1000); // 设置定时器间隔为100毫秒
    QTextStream stdinStream(stdin);
    QObject::connect(timer.get(), &QTimer::timeout, [&]() {
//        if (std::cin.gcount() > 0) {
//            QString input = stdinStream.readLine(); // 读取输入的一行
//            qDebug() << "User input:" << input;
//        }
        std::cout << "hello world" << std::endl;
    });
    timer->start(); // 启动定时器
    std::cout << "------" << std::endl;
}



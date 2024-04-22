#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QThreadPool>
#include <iostream>
#include "firmwareupgrade.h"
#include "singleton/singleton.h"
#include "spdlog/spdlog.h"
unsigned short blksize_default = 512;
unsigned short timeout_default = 2;
unsigned short maxRetransmit_default = 5;


int main(int argc, char *argv[])
{
    //std::cout << "hello world" << std::endl;
    QCoreApplication a(argc, argv);
    FirmwareUpgrade fupgrade;
    //std::cout << "hello world" << std::endl;
    return a.exec();
}

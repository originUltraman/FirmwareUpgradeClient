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
unsigned short retry_default = 5;
unsigned short dlpRetry_default = 2;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FirmwareUpgrade fupgrade;
    return a.exec();
}

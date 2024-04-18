#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QThreadPool>
#include "firmwareupgrade.h"
unsigned short blksize_default;
unsigned short timeout_default;
unsigned short maxRetransmit_default;






int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//test
    FirmwareUpgrade fupgrade;
    return a.exec();
}

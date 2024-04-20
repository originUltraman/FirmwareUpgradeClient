#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QThreadPool>
#include "firmwareupgrade.h"
unsigned short blksize_default;
unsigned short timeout_default;
unsigned short maxRetransmit_default;


char dir[256];
size_t max_retrans_times = 5;
size_t wait_time_ms = 2000;
size_t max_find_response_time_ms = 1000;
size_t state_file_send_interval = 10000;




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//test
    FirmwareUpgrade fupgrade;
    return a.exec();
}

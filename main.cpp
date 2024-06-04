#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QThreadPool>
#include <iostream>
#include "firmwareupgrade.h"
#include "singleton/singleton.h"
#include "spdlog/spdlog.h"
#include "httplib/httplib.h"
unsigned short blksize_default = 512;
unsigned short timeout_default = 2;
unsigned short retry_default = 5;
unsigned short dlpRetry_default = 2;

int main(int argc, char *argv[])
{
    std::thread svrThread([](){
        httplib::Server svr;
        svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
          res.set_content("Hello World!", "text/plain");
        });

        svr.listen("0.0.0.0", 8080);
    });
    QCoreApplication a(argc, argv);
    FirmwareUpgrade fupgrade;
    return a.exec();
}

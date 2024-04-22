#include "menuthread.h"
#include "global.h"
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <singleton/singleton.h>
std::mutex menuThreadMutex;
std::condition_variable menuThreadCv;
MenuThread::MenuThread():
    logger(Singleton<spdlog::logger>::Instance())
{

}

void MenuThread::printMenu() const
{
    logger.info("-------------------------------------\n"
                "Welcome to FirmwareUpgrade System.\n"
                "1. Find\n"
                "2. Upload\n"
                "3. Donwload\n"
                "4. Exit\n"
                "-------------------------------------\n"
                "Please input > ");
    logger.flush();
}

void MenuThread::run()
{
    int op;
    do{
        std::vector<std::string> args;
        printMenu();
        std::cin >> op;
        switch (op) {
        case FIND:{
            emit sendMenuOp(op, args);
            logger.info("wait for results");
            std::unique_lock<std::mutex> ulock(menuThreadMutex);
            menuThreadCv.wait(ulock);
            logger.info("FIND operation finish");
            break;
        }
        case UPLOAD:{
            logger.info("Please input your device ip");
            std::string ip;
            logger.info("Please input > ");
            std::cin >> ip;
            args.emplace_back(std::move(ip));
            logger.info("Plearse input upload files(e.g. file1 file2): ");
            while(true){
                std::string file;
                std::cin >> file;
                args.emplace_back(std::move(file));
                if(std::cin.get() == '\n') break;
            }
            emit sendMenuOp(UPLOAD, args);
            break;
        }
        case EXIT:
            emit sendMenuOp(EXIT, args);
            break;
        default:
            break;
        }

    }while(op != EXIT);
}

#include "menuthread.h"
#include "global.h"
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <protocal/protocal.h>
#include <singleton/singleton.h>
std::mutex menuThreadMutex;
std::condition_variable menuThreadCv;
MenuThread::MenuThread():
    logger(Singleton<spdlog::logger>::Instance())
{
    info = std::bind(&spdlog::logger::info<>, &logger, std::placeholders::_1);
    debug = std::bind(&spdlog::logger::debug<>, &logger, std::placeholders::_1);
    error = std::bind(&spdlog::logger::error<>, &logger, std::placeholders::_1);
    flush = std::bind(&spdlog::logger::flush, &logger);
}

void MenuThread::printMenu() const
{
    info("-------------------------------------\n"
                "Welcome to FirmwareUpgrade System.\n"
                "1. Find\n"
                "2. Upload\n"
                "3. Donwload\n"
                "4. Exit\n"
                "-------------------------------------\n"
                "Please input > ");
    flush();
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
            std::unique_lock<std::mutex> ulock(menuThreadMutex);
            menuThreadCv.wait(ulock);
            break;
        }
        case UPLOAD:{
            info("Please input your device id");
            std::string id;
            std::cin >> id;
            args.emplace_back(id);
            info("Plearse input upload files(e.g. file1 file2): ");
            while(true){
                std::string file;
                std::cin >> file;
                args.emplace_back(std::move(file));
                if(std::cin.get() == '\n') break;
            }
            emit sendMenuOp(UPLOAD, args);
            info("UPLOAD IN PROGRESS, INPUT C TO INTERRUPT");
//            std::thread thread([&](){

//            })
            std::unique_lock<std::mutex> ulock(menuThreadMutex);
            menuThreadCv.wait(ulock);
            break;
        }
        case DOWNLOAD:{
            info("Please input your device id");
            std::string id;
            std::cin >> id;
            args.emplace_back(id);
            emit sendMenuOp(DOWNLOAD, args);
            std::unique_lock<std::mutex> ulock(menuThreadMutex);
            menuThreadCv.wait(ulock);
            info("Plearse input download files(e.g. file1 file2): ");
            QStringList files;
            while(true){
                std::string file;
                std::cin >> file;
                files.push_back(QString::fromStdString(file));
                if(std::cin.get() == '\n') break;
            }
            emit downloadFiles(files);
            ulock.try_lock();
            menuThreadCv.wait(ulock);
            break;
        }

        case EXIT:{
            emit sendMenuOp(EXIT, args);
            break;
        }
        default:
            break;
        }
    }while(op != EXIT);
}

void MenuThread::on_parseLNL_finished(File_LNL *LNL)
{
    std::unique_lock<std::mutex> ulock(menuThreadMutex);
    for(decltype(LNL->file_num) i = 0; i < LNL->file_num; i++){
       logger.info("FileName:{0} : Descrip:{1}", LNL->file[i].file_name, LNL->file[i].file_stat_des);
    }
    menuThreadCv.notify_one();
}


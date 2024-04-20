#include "menuthread.h"
#include <iostream>
MenuThread::MenuThread()
{

}

void MenuThread::printMenu() const
{
    std::cout << "Welcome to FirmwareUpgrade System.\n"
                 "1. Find\n"
                 "2. Upload\n"
                 "3. Donwload\n"
                 "4. Exit" << std::endl;
}

void MenuThread::run()
{
    int op;
    do{
        printMenu();
        std::cin >> op;
        emit sendMenuOp(op, {});
    }while(op != 4);
}

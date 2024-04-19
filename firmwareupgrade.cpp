#include "firmwareupgrade.h"
#include <iostream>
#include "thread/menuthread.h"
#include "thread/findthread.h"
#include "global.h"
FirmwareUpgrade::FirmwareUpgrade()
{
    pool.setMaxThreadCount(16);
    menuThread = std::make_shared<MenuThread>();
    //注册数据类型
    qRegisterMetaType<std::initializer_list<std::string>>("std::initializer_list<std::string>");
    connect(menuThread.get(), &MenuThread::sendMenuOp, this, &FirmwareUpgrade::onRecvdMenuOp);
    pool.start(menuThread.get());
}

void FirmwareUpgrade::onRecvdMenuOp(int op, std::initializer_list<std::string> list)
{
    switch (op) {
    case FIND:{
        //findThread = std::make_shared<FindThread>();
        FindThread* findThread = new FindThread();
        findThread->setAutoDelete(true);//执行完在新线程里面自动调用析构函数
        pool.start(findThread);
        break;
    }
    case UPLOAD:
        break;
    case DOWNLOAD:
        break;
    default:
        break;
    }
}



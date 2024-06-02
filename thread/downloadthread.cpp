#include "downloadthread.h"
#include "spdlog/logger.h"
#include "tftp/tftprequest.h"

DownloadThread::DownloadThread(const Device* device, QObject *parent):
    thread(device, parent)
{

}

void DownloadThread::run()
{
    auto uSock = std::make_shared<QUdpSocket>();
    if(uSock->bind(PROTOCAL_FILE_PORT) == false){
        logger.error("BIND PORT {} ALREADY IN USE", PROTOCAL_FILE_PORT);
        return;
    }
    unsigned short dlpTry = 0;
    std::string errorMessage;
    while(status != END){
        switch(status){
        case INITIALIZATION:{
            TftpRequest request(TftpRequest::RRQ, fmt::format("{0}.LNO", device->getName()), device->getHostAddress(), TFTP_SERVER_PORT);
            while(!Tftp::get(uSock.get(), request, errorMessage) && ++dlpTry < dlpRetry_default + 1){
                status = END;
                logger.error(errorMessage);
            }
            if(status == END) break;
            status = LIST_TRANSFER;
            break;
        }
        case LIST_TRANSFER:{
            while(LNS.op_stat_code != 0x0001){
                std::unique_lock<std::mutex> lock(m);
                cv.wait(lock);
            }
            while(tftpRequests.empty()){
                std::unique_lock<std::mutex> ulock(m);
                auto cv_status = cv.wait_for(ulock, std::chrono::seconds(13));
                if(cv_status == std::cv_status::timeout){
                    errorMessage = fmt::format("TIME OUT FOR WRQ OF {}.LNL", device->getName());
                    status = ERROR_;
                    break;
                }
            }
            if(status == ERROR_) break;
            TftpRequest WRQ;
            tftpRequests.dequeue(WRQ);
            while(!Tftp::handlePut(uSock.get(), WRQ, errorMessage) && ++dlpTry < dlpRetry_default + 1){
                logger.error(errorMessage);
            }
            if(dlpTry >= dlpRetry_default + 1){
                status = ERROR_;
                errorMessage = fmt::format("HANDLE PUT {0}.LNL ERROR", device->getName());
                break;
            }
            dlpTry = 0;
            if(!parseLNL(errorMessage)){
                status = ERROR_;
                errorMessage = fmt::format("PARSE FILE {0}.LNL ERROR", device->getName());
                break;
            }

            if(!makeLNA(errorMessage)){
                status = ERROR_;
                errorMessage = fmt::format("MAKE FILE {0}.LNA ERROR", device->getName());
                break;
            }
            logger.info("after makeLNA");
            WRQ = TftpRequest(TftpRequest::WRQ, fmt::format("{0}.LNA", device->getName()), device->getHostAddress(), TFTP_SERVER_PORT);
            while(!Tftp::put(uSock.get(), WRQ, errorMessage) && ++dlpTry < dlpRetry_default + 1){
                logger.error(errorMessage);
            }
            if(dlpTry >= dlpRetry_default + 1){
                status = ERROR_;
                errorMessage = fmt::format("PUT {0}.LNA ERROR", device->getName());
                break;
            }
            status = TRANSFER;
            break;
        }
        case TRANSFER:{
            while(tftpRequests.empty()){
                std::unique_lock<std::mutex> ulock(m);
                auto cv_status = cv.wait_for(ulock, std::chrono::seconds(13));
                if(cv_status == std::cv_status::timeout){
                    errorMessage = fmt::format("TIME OUT FOR WRQ OF DATA FILE");
                    status = ERROR_;
                    break;
                }
            }
            if(status == ERROR_) break;
            TftpRequest WRQ;
            tftpRequests.dequeue(WRQ);
            while(!Tftp::handlePut(uSock.get(), WRQ, errorMessage) && ++dlpTry < dlpRetry_default + 1){
                logger.error(errorMessage);
            }
            if(dlpTry >= dlpRetry_default + 1){
                status = ERROR_;
                errorMessage = fmt::format("HANDLE PUT {0}.LNL ERROR", device->getName());
                break;
            }
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock);
            switch(LNS.op_stat_code){
            case 0x0002:
                break;
            case 0x0003:
                status = END;
                break;
            default:
                status = END;
                break;
            }
            break;
        }

        case ERROR_:
            logger.error(errorMessage);
            status = END;
            break;
        case END:
            break;
        default:
            break;
        }
    }

}

void DownloadThread::on_userOp_finished(QStringList fileList)
{
    std::unique_lock<std::mutex> m(userOp_m);
    checkedFileList = std::move(fileList);
    userOp_flag = true;
    userOp_cv.notify_one();
}

bool DownloadThread::parseStatusFile(std::string& errorMessage)
{
    //qDebug() << "进入ODownload线程的parseStatusFile";
    QFile fLNS(QString("%1.LNS").arg(QString::fromStdString(device->getName())));
    if(fLNS.open(QIODevice::ReadOnly) == false){
        errorMessage = fmt::format("OPEN FILE {}.LNS ERROR", device->getName());
        return false;
    }
    memset(&LNS, 0, sizeof(File_LNS));
    QDataStream in(&fLNS);
    in.setByteOrder(QDataStream::LittleEndian);

    in >> LNS.file_len;
    in >> LNS.Pro_ver;
    in >> LNS.op_stat_code;
    in >> LNS.stat_des_len;
    in.readRawData(LNS.stat_des, LNS.stat_des_len);
    in >> LNS.counter;
    in >> LNS.excep_timer;
    in >> LNS.estim_timer;
    in.readRawData(LNS.down_list_ratio, 3);
    in >> LNS.file_num;
    if(LNS.file_num > 0) LNS.LNS_files = (struct file_info_LNS*)malloc(sizeof(file_info_LNS) * LNS.file_num);
    for(int i= 0; i < LNS.file_num; i++){
        in >> LNS.LNS_files[i].file_name_len;
        in.readRawData(LNS.LNS_files[i].file_name, 255);
        in >> LNS.LNS_files[i].file_stat;
        in >> LNS.LNS_files[i].file_stat_des_len;
        in.readRawData(LNS.LNS_files[i].file_stat_des, 255);
    }
    fLNS.close();
    //emit(parseStatusFileFinished(LNS));

    std::unique_lock<std::mutex> locker(m);
    cv.notify_one();
    return true;
}

bool DownloadThread::parseLNL(std::string& errorMessage)
{
    QFile fLUS(QString("%1.LNL").arg(QString::fromStdString(device->getName())));
    if(fLUS.open(QIODevice::ReadOnly) == false){
        errorMessage = fmt::format("OPEN FILE {0}.LNL ERROR", device->getName());
        return false;
    }
    memset(&LNL, 0, sizeof(LNL));
    QDataStream in(&fLUS);

#ifdef BIG_ENDIAN
    in.setByteOrder(QDataStream::BigEndian);
#else
    in.setByteOrder(QDataStream::LittleEndian);
#endif

    in >> LNL.file_len;
    in.readRawData(LNL.Pro_ver, 2);
    in >> LNL.file_num;

    LNL.file = (struct file_info_LNL*)malloc(sizeof(file_info_LNL) * LNL.file_num);
    for(int i = 0; i < LNL.file_num; i++){
        in >> LNL.file[i].file_name_len;
        in.readRawData(LNL.file[i].file_name, 255);
        in >> LNL.file[i].file_stat_des_len;
        in.readRawData(LNL.file[i].file_stat_des, 255);
    }
    emit parseLNLFinished(&LNL);
    fLUS.close();
    return true;
}

bool DownloadThread::makeLNA(std::string& errorMessage)
{
    while(!userOp_flag){
        std::unique_lock<std::mutex> ulock(m);
        auto cv_status = cv.wait_for(ulock, std::chrono::seconds(13));
        if(cv_status == std::cv_status::timeout){
            errorMessage = "TIME OUT FOR USER OPERATION";
            status = ERROR_;
            break;
        }
    }
    if(status == ERROR_){
        return false;
    }

    File_LNA* struct_LNA = new File_LNA();
    struct_LNA->file_len = 8 + sizeof(file_info_LNA) * checkedFileList.size();
    memcpy(struct_LNA->Pro_ver, Protocol_ver, 2);

    struct_LNA->file_num = checkedFileList.size();
    struct_LNA->file = (struct file_info_LNA*)malloc(sizeof(file_info_LNA) * checkedFileList.count());
    for(int i = 0; i < checkedFileList.size(); i++){
        strcpy(struct_LNA->file[i].file_name, checkedFileList.at(i).toUtf8().data());
        struct_LNA -> file[i].file_name_len = checkedFileList.at(i).size();
    }
    QFile LNA(QString("%1.LNA").arg(QString::fromStdString(device->getName())));
    if(LNA.open(QIODevice::WriteOnly) == false){
        status = ERROR_;
        errorMessage = fmt::format("FILE {0}.LNA OPEN ERROR", device->getName());
        return false;
    }
    QDataStream out(&LNA);
#ifdef BIG_ENDIAN
    out.setByteOrder(QDataStream::BigEndian);
#else
    out.setByteOrder(QDataStream::LittleEndian);
#endif

    out << struct_LNA->file_len;
    out.writeRawData(struct_LNA->Pro_ver, 2);
    out << struct_LNA->file_num;
    for(int i = 0; i < struct_LNA->file_num; i++){
        out << struct_LNA->file[i].file_name_len;
        out.writeRawData(struct_LNA->file[i].file_name, 255);
    }
    LNA.close();
    free(struct_LNA);
}

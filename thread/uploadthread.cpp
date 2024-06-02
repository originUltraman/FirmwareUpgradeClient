#include "uploadthread.h"
#include <functional>
#include "spdlog/logger.h"
void UploadThread::run()
{
    QUdpSocket* uSock = new QUdpSocket();
    if(uSock->bind(PROTOCAL_FILE_PORT) == false){
        logger.error("BIND PORT {} ALREADY IN USE", PROTOCAL_FILE_PORT);
        return;
    }
    TftpRequest request;
    QFile LUR(QString::fromStdString(fmt::format("{0}/{1}.LUR", dir.dirName().toStdString(), device->getName())));
    QFile LUH(QString::fromStdString(fmt::format("{0}/{1}.LUH", dir.dirName().toStdString(), device->getName())));
    unsigned short dlpTry = 0;
    std::string errorMessage;
    while(status != END){
        switch (status) {
        case INITIALIZATION:{
            TftpRequest request(TftpRequest::RRQ, fmt::format("{0}.LUI", device->getName()), device->getHostAddress(), TFTP_SERVER_PORT);
            while(!Tftp::get(uSock, request, errorMessage) && ++dlpTry < dlpRetry_default + 1){
                logger.error(errorMessage);
            }
            if(dlpTry >= dlpRetry_default + 1){
                status = ERROR_;
                errorMessage = fmt::format("REQUEST FILE {}.LUI FAILED", device->getName());
                break;
            }
            dlpTry = 0;
            logger.info("get LUI success");
            status = LIST_TRANSFER;
            break;
        }
        case LIST_TRANSFER:{
            while(LUS.op_stat_code != 0x0001){
                std::unique_lock<std::mutex> lock(m);
                cv.wait(lock);
            }
            //当状态码为0x0001时表示设备端接受上传操作
            QFile LUR(QString::fromStdString(fmt::format("{0}/{1}.LUR", dir.dirName().toStdString(), device->getName())));
            makeLUR();
            if(!LUR.exists()){
                errorMessage = fmt::format("CREATE {}.LUR FAILED", device->getName());
                status = ERROR_;
                break;
            }
            TftpRequest request(TftpRequest::WRQ, fmt::format("{}.LUR", device->getName()), device->getHostAddress(), TFTP_SERVER_PORT);
            while(!Tftp::put(uSock, request, errorMessage, dir.dirName().toStdString()) && ++dlpTry < dlpRetry_default + 1){
                logger.error(errorMessage);
            }
            if(dlpTry >= dlpRetry_default + 1){
                status = ERROR_;
                errorMessage = fmt::format("PUT FILE {}.LUR FAILED", device->getName());
                break;
            }
            dlpTry = 0;
            logger.info(fmt::format("put {}.LUR success", device->getName()));
            status = TRANSFER;
            break;
        }
        case TRANSFER:{

            while(tftpRequests.empty()){
                std::unique_lock<std::mutex> ulock(m);
                auto cv_status = cv.wait_for(ulock, std::chrono::seconds(13));
                if(cv_status == std::cv_status::timeout){
                    errorMessage = "TIME OUT FOR DATA FILE READ REQUEST";
                    status = ERROR_;
                    break;
                }
                if(LUS.op_stat_code != 0x0002) break;
                logger.info("LUS.op_stat_code = {}", LUS.op_stat_code);
            }
            if(status == ERROR_) break;
            if(LUS.op_stat_code != 0x0002){
                switch (LUS.op_stat_code) {
                case 0x0003:
                    status = END;
                    break;
                default:
                    status = ERROR_;
                    break;
                }
                break;
            }
            TftpRequest request;
            tftpRequests.dequeue(request);
            logger.info("before handleget");
            while(!Tftp::handleGet(uSock, request, errorMessage) && ++dlpTry < dlpRetry_default + 1){
                logger.error(errorMessage);
            }
            if(dlpTry >= dlpRetry_default + 1){
                status = ERROR_;
                errorMessage = fmt::format("HANDLE GET FILE {} FAILED", request.getFileName());
                break;
            }
            logger.info(fmt::format("handle get file {} success", request.getFileName()));
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock);
            switch(LUS.op_stat_code){
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
            //TODO----执行Abort操作
            logger.info("ERROR {}", errorMessage);
            status = END;
            break;
        default:
            break;
        }
    }
    uSock->close();
    logger.info("uoload finish");
}

void UploadThread::makeLUR(){
    QFile LUR(QString("%1/%2.LUR").arg(dir.dirName(), QString::fromStdString(device->getName())));
    //LUR.open(QIODevice::WriteOnly);
    File_LUR LUR_struct;
    memcpy(LUR_struct.Pro_ver, Protocol_ver, 2);
    LUR_struct.Header_num = fileList.size();
    LUR_struct.file_len = sizeof(LUR_struct.Pro_ver) + sizeof(LUR_struct.Header_num) + sizeof(LUR_struct.file_len);
    LUR_struct.Hfile = (HFILE_INFO*) malloc(LUR_struct.Header_num * sizeof(HFILE_INFO));
    for(int i = 0; i < LUR_struct.Header_num; i++){
        QFileInfo fileInfo(fileList.at(i));
        LUR_struct.Hfile[i].fileLen = fileInfo.size();
        memset(LUR_struct.Hfile[i].name, 0 ,sizeof (LUR_struct.Hfile[i].name));
        strcpy(LUR_struct.Hfile[i].name, fileList.at(i).mid(fileList.at(i).lastIndexOf('/') + 1).toStdString().c_str());
        LUR_struct.Hfile[i].len_name = strlen(LUR_struct.Hfile[i].name);
        //LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].len_name) + LUR_struct.Hfile[i].len_name;
        memset(LUR_struct.Hfile[i].load_part_name, 0, sizeof(LUR_struct.Hfile[i].load_part_name));
        strcpy(LUR_struct.Hfile[i].load_part_name, dir.absolutePath().toStdString().c_str());
        LUR_struct.Hfile[i].load_part_len_name = strlen(LUR_struct.Hfile[i].load_part_name);
        //LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].load_part_len_name) + LUR_struct.Hfile[i].load_part_len_name;
        LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].load_part_len_name) + sizeof(LUR_struct.Hfile[i].len_name);
        LUR_struct.file_len += strlen(LUR_struct.Hfile[i].load_part_name) + 1;
        LUR_struct.file_len += strlen(LUR_struct.Hfile[i].name) + 1;
    }
    if(LUR.open(QIODevice::WriteOnly)){
        QDataStream os(&LUR);
#ifdef BIG_ENDIAN
        os.setByteOrder(QDataStream::BigEndian);
#else
        os.setByteOrder(QDataStream::LittleEndian);
#endif
        os << LUR_struct.file_len;
        os.writeRawData(LUR_struct.Pro_ver, sizeof(LUR_struct.Pro_ver));
        os << LUR_struct.Header_num;
        for(int i = 0; i < LUR_struct.Header_num; ++i){
            os << LUR_struct.Hfile[i].fileLen;
            os << LUR_struct.Hfile[i].len_name;
            os.writeRawData(LUR_struct.Hfile[i].name, LUR_struct.Hfile[i].len_name + 1);
            os << LUR_struct.Hfile[i].load_part_len_name;
            os.writeRawData(LUR_struct.Hfile[i].load_part_name, LUR_struct.Hfile[i].load_part_len_name + 1);
        }
        LUR.close();
    }
    free(LUR_struct.Hfile);
}

void UploadThread::makeLUH()
{
    QFile LUH(QString("%1/%2.LUH").arg(dir.dirName(), QString::fromStdString(device->getName())));
    LUH.open(QIODevice::ReadWrite | QIODevice::Truncate);
    File_LUH LUH_struct;
    int i = 0;
    memset(&LUH_struct, 0, sizeof(File_LUH));
    LUH_struct.DataFile_Num = fileList.size();
    LUH_struct.Ptr_DataFile_Num = fileList.size();
    LUH_struct.datafile = (DataFile*)malloc(sizeof (DataFile) * LUH_struct.DataFile_Num);
    LUH_struct.file_len = sizeof(LUH_struct.file_len) + sizeof (LUH_struct.LoadFile_Ver);
    LUH_struct.file_len += sizeof(LUH_struct.Part_Flag) + sizeof(LUH_struct.Ptr_PN_Len);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_TargetID_Num) + sizeof(LUH_struct.Ptr_DataFile_Num);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_SupFile_Num) + sizeof(LUH_struct.Ptr_UserDef_Data);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_LoadTypeDes_Len) + sizeof(LUH_struct.Ptr_TargetIDPosition_Num);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_LoadCheckVal_Len) + sizeof(LUH_struct.DataFile_Num);
    LUH_struct.file_len += sizeof(LUH_struct.LoadCheckVal_Type) + sizeof(LUH_struct.HFile_CRC);
    memset(LUH_struct.datafile, 0, LUH_struct.DataFile_Num * sizeof(DataFile));
    qDebug() << LUH_struct.file_len << "---1";
    for(i = 0; i < LUH_struct.DataFile_Num; ++i)
    {
        //结构体内容转换
        LUH_struct.file_len += 789;
        int lastSlash = fileList.at(i).lastIndexOf('/');
        QString path = fileList.at(i).left(lastSlash);
        QString name = fileList.at(i).mid(lastSlash + 1);
        LUH_struct.datafile[i].DataFile_Name_Len = name.size();
        memset(LUH_struct.datafile[i].DataFile_Name, 0, sizeof(LUH_struct.datafile[i].DataFile_Name));
        strcpy(LUH_struct.datafile[i].DataFile_Name, name.toUtf8().data());
        LUH_struct.datafile[i].DataFile_PN_Len = ((path.size() + 1 + 1)/ 2) * 2;
        memset(LUH_struct.datafile[i].DataFile_PN, 0, LUH_struct.datafile[i].DataFile_PN_Len);
        QFile file(fileList.at(i));
        char to_crc[512];
        if(file.open(QIODevice::ReadOnly)){
            LUH_struct.datafile[i].DataFile_Len = file.size();
            unsigned short crc = 0;
            size_t nBytes;
            memset(to_crc, 0, sizeof(to_crc));
//            memcpy(to_crc, file.readAll(), file.size());
//            crc = crc16_table(to_crc, file.size(), 0);
            while((nBytes = file.read(to_crc, 512)) > 0){
                crc = crc16_table(to_crc, nBytes, crc);
            }
            LUH_struct.datafile[i].DataFile_CRC = crc;
            //free(to_crc);
            qDebug()<<"16进制小写输出"<<QString("%1").arg(crc,4,16,QLatin1Char('0'));
            file.close();
        }
        LUH_struct.datafile[i].DataFile_LenInBytes = LUH_struct.datafile[i].DataFile_Len;
        LUH_struct.datafile[i].DataFile_CheckVal_Len = 0;
        memset(LUH_struct.datafile[i].DataFile_CheckVal, 0, 2);

        if(i + 1 == LUH_struct.DataFile_Num)
            LUH_struct.datafile[i].Ptr_DataFile = 0;

    }
    qDebug() << LUH_struct.file_len << "---2";
    LUH.write((char *)&LUH_struct.file_len, sizeof(LUH_struct.file_len));
    LUH.write((char *)&LUH_struct.LoadFile_Ver, sizeof(LUH_struct.LoadFile_Ver));
    LUH.write((char *)&LUH_struct.Part_Flag, sizeof(LUH_struct.Part_Flag));
    LUH.write((char *)&LUH_struct.Ptr_PN_Len, sizeof(LUH_struct.Ptr_PN_Len));
    LUH.write((char *)&LUH_struct.Ptr_TargetID_Num, sizeof(LUH_struct.Ptr_TargetID_Num));
    LUH.write((char *)&LUH_struct.Ptr_DataFile_Num, sizeof(LUH_struct.Ptr_DataFile_Num));
    LUH.write((char *)&LUH_struct.Ptr_SupFile_Num, sizeof(LUH_struct.Ptr_SupFile_Num));
    LUH.write((char *)&LUH_struct.Ptr_UserDef_Data, sizeof(LUH_struct.Ptr_UserDef_Data));
    LUH.write((char *)&LUH_struct.Ptr_LoadTypeDes_Len, sizeof(LUH_struct.Ptr_LoadTypeDes_Len));
    LUH.write((char *)&LUH_struct.Ptr_TargetIDPosition_Num, sizeof(LUH_struct.Ptr_TargetIDPosition_Num));
    LUH.write((char *)&LUH_struct.Ptr_LoadCheckVal_Len, sizeof(LUH_struct.Ptr_LoadCheckVal_Len));
    LUH.write((char *)&LUH_struct.DataFile_Num, sizeof(LUH_struct.DataFile_Num));
    for(i = 0; i < LUH_struct.DataFile_Num; ++i)
    {
        LUH.write((char*)&LUH_struct.datafile[i].Ptr_DataFile, sizeof (LUH_struct.datafile[i].Ptr_DataFile));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_Name_Len, &LUH_struct.datafile[i].DataFile_Name_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_Name_Len, sizeof (LUH_struct.datafile[i].DataFile_Name_Len));
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_Name, sizeof (LUH_struct.datafile[i].DataFile_Name));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_PN_Len, &LUH_struct.datafile[i].DataFile_PN_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_PN_Len, sizeof (LUH_struct.datafile[i].DataFile_PN_Len));
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_PN, sizeof (LUH_struct.datafile[i].DataFile_PN));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_Len, &LUH_struct.datafile[i].DataFile_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_Len, sizeof (LUH_struct.datafile[i].DataFile_Len));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_CRC, &LUH_struct.datafile[i].DataFile_CRC);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CRC, sizeof(LUH_struct.datafile[i].DataFile_CRC));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_LenInBytes, &LUH_struct.datafile[i].DataFile_LenInBytes);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_LenInBytes, sizeof(LUH_struct.datafile[i].DataFile_LenInBytes));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_CheckVal_Len, &LUH_struct.datafile[i].DataFile_CheckVal_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CheckVal_Len, sizeof(LUH_struct.datafile[i].DataFile_CheckVal_Len));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_CheckVal_Type, &LUH_struct.datafile[i].DataFile_CheckVal_Type);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CheckVal_Type, sizeof(LUH_struct.datafile[i].DataFile_CheckVal_Type));
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CheckVal, sizeof(LUH_struct.datafile[i].DataFile_CheckVal));
    }
    LUH.flush();
    char *to_crc = (char *)malloc(LUH_struct.file_len - 2);
    LUH.seek(LUH_struct.file_len - 4);
    LUH_struct.LoadCheckVal_Type = 4;
    LUH.write((char*)&LUH_struct.LoadCheckVal_Type, sizeof(LUH_struct.LoadCheckVal_Type));
    LUH.seek(0);
    memcpy(to_crc, LUH.readAll(), LUH_struct.file_len - 2);
    LUH_struct.HFile_CRC = crc16_table(to_crc, LUH_struct.file_len - 2, 0);
    //qToBigEndian(LUH_struct.HFile_CRC, &LUH_struct.HFile_CRC);
    qDebug()<<"16进制小写输出"<<QString("%1").arg(LUH_struct.HFile_CRC,4,16,QLatin1Char('0'));
    LUH.write((char*)&LUH_struct.HFile_CRC, sizeof(LUH_struct.HFile_CRC));
    LUH.close();
    free(to_crc);
    free(LUH_struct.datafile);
}


bool UploadThread::parseStatusFile(std::string& errorMessage)
{
    free(LUS.hfiles);
    QFile fLUS(QString("%1.LUS").arg(QString::fromStdString(device->getName())));
    if(fLUS.open(QIODevice::ReadOnly) == false){
        errorMessage = fmt::format("OPEN FILE {}.LUS ERROR", device->getName());
        return false;
    }
    memset(&LUS, 0, sizeof(File_LUS));
    QDataStream in(&fLUS);
#ifdef BIG_ENDIAN
        in.setByteOrder(QDataStream::BigEndian);
#else
        in.setByteOrder(QDataStream::LittleEndian);
#endif
    in >> LUS.file_len;
    in.readRawData(LUS.Pro_ver, 2);
    in >> LUS.op_stat_code;
    in >> LUS.stat_des_len;
    in.readRawData(LUS.stat_des, LUS.stat_des_len);
    in >> LUS.counter;
    in >> LUS.excep_timer;
    in >> LUS.estim_timer;
    in.readRawData(LUS.load_list_ratio, 3);
    in >> LUS.hfile_num;
    if(LUS.hfile_num > 0) LUS.hfiles = (struct Hfile_info_LUS*)malloc(sizeof(Hfile_info_LUS) * LUS.hfile_num);
    for(int i = 0; i < LUS.hfile_num; ++i){
        in >> LUS.hfiles[i].fileLen;
        in >> LUS.hfiles[i].Hfile_name_len;
        in.readRawData(LUS.hfiles[i].Hfile_name, LUS.hfiles[i].Hfile_name_len + 1);
        in >> LUS.hfiles[i].load_part_num_name_len;
        in.readRawData(LUS.hfiles[i].load_part_num_name, LUS.hfiles[i].load_part_num_name_len + 1);
        in.readRawData(LUS.hfiles[i].load_ratio, 3);
        in >> LUS.hfiles[i].load_stat;
        in >> LUS.hfiles[i].load_stat_des_len;
        in.readRawData(LUS.hfiles[i].load_stat_des, LUS.hfiles[i].load_stat_des_len + 1);

//        qDebug() << LUS.hfiles[i].Hfile_name_len << LUS.hfiles[i].Hfile_name << LUS.hfiles[i].load_part_num_name_len << LUS.hfiles[i].load_part_num_name
//                 << LUS.hfiles[i].load_ratio << LUS.hfiles[i].load_stat << LUS.hfiles[i].load_stat_des_len << LUS.hfiles[i].load_stat_des;
    }
    logger.info("in parseStatusFile() LUS.op_stat_code = {}", LUS.op_stat_code);
    fLUS.close();
    std::unique_lock<std::mutex> lock(m);
    cv.notify_one();
    return true;
}


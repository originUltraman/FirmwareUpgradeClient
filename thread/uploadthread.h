#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H
#include "thread/thread.h"
#include "safequeue/safequeue.h"

class UploadThread : public thread
{
    Q_OBJECT
public:
    UploadThread(QStringList fileList, const Device* device, QObject *parent = 0):
        thread(device, parent), fileList(fileList){
        status = INITIALIZATION;
        memset(&LUS, 0, sizeof(LUS));
    }
    void run() override;
    static File_LUS* parseLUS(QFile*);
    enum status_set{INITIALIZATION, LIST_TRANSFER, TRANSFER, END, ERROR_} status;

signals:
    void uploadResult(bool);
    void uploadStatusMessage(QString);
    void uploadRate(int, bool);
    void uploadFinish();
    void parseStatusFileFinished(File_LUS);
private:
    QStringList fileList;
    QMap<QString, bool> files_sent;
    unsigned int fileSentCnt = 0;
    unsigned int waitTimes = 0;
    File_LUS LUS;
    void makeLUR();
    void makeLUH();

    // thread interface
public:
    bool parseStatusFile(std::string& errorMessage) override;
};

#endif // UPLOADTHREAD_H

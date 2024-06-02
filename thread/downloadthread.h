#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include "thread.h"

class DownloadThread : public thread
{
    Q_OBJECT
public:
    explicit DownloadThread(const Device* device, QObject *parent = nullptr);
    enum status_set{INITIALIZATION, LIST_TRANSFER, TRANSFER, END, ERROR_};
    void run() override;
public slots:
    void on_userOp_finished(QStringList checkedFileList);
private:
    status_set status = INITIALIZATION;
    File_LNS LNS;
    File_LNL LNL;
    std::condition_variable userOp_cv;
    bool userOp_flag = false;
    std::mutex userOp_m;
    QStringList checkedFileList;
public:
    bool parseStatusFile(std::string& errorMessage) override;
    bool parseLNL(std::string& errorMessage);
    bool makeLNA(std::string& errorMessage);

signals:
    void parseLNLFinished(File_LNL* LNL);
};

#endif // DOWNLOADTHREAD_H

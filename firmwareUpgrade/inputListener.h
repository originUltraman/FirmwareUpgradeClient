#include <QCoreApplication>
#include <QObject>
#include <QSocketNotifier>
#include <QTextStream>
#include <unistd.h> // for STDIN_FILENO
#include <iostream>

class InputListener : public QObject
{
    Q_OBJECT
public:
    InputListener(QObject *parent = nullptr) : QObject(parent)
    {
        // 创建标准输入流的通知器
        m_stdinNotifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);

        // 连接通知器的activated信号到processInput槽函数
        connect(m_stdinNotifier, &QSocketNotifier::activated, this, &InputListener::processInput);
    }

public slots:
    void processInput(int socket)
    {
        if (socket == STDIN_FILENO) {
            // 从标准输入流中读取数据并解析参数
            QString input = m_inputStream.readLine().trimmed(); // 移除前后空格

            QStringList parts = input.split(" ", Qt::SkipEmptyParts);
            if (parts.isEmpty()) {
                std::cout << "Invalid input." << std::endl;
                return;
            }

            QString command = parts.first();
            parts.removeFirst();

            if (command == "find") {
                if (parts.size() != 1) {
                    std::cout << "Invalid number of arguments for 'find'. Expected 1." << std::endl;
                    return;
                }
                QString ipAddress = parts.first();
                emit findRequested(ipAddress);
            } else if (command == "upload") {
                if (parts.size() < 3) {
                    std::cout << "Invalid number of arguments for 'upload'. Expected at least 3." << std::endl;
                    return;
                }
                QString ipAddress = parts.first();
                parts.removeFirst();
                QStringList filenames = parts;
                emit uploadRequested(ipAddress, filenames);
            } else if (command == "download") {
                if (parts.size() < 3) {
                    std::cout << "Invalid number of arguments for 'download'. Expected at least 3." << std::endl;
                    return;
                }
                QString ipAddress = parts.first();
                parts.removeFirst();
                QStringList filenames = parts;
                emit downloadRequested(ipAddress, filenames);
            } else {
                std::cout << "Invalid command. Please enter 'find', 'upload', or 'download'." << std::endl;
            }
        }
    }

signals:
    void findRequested(const QString &ipAddress);
    void uploadRequested(const QString &ipAddress, const QStringList &filenames);
    void downloadRequested(const QString &ipAddress, const QStringList &filenames);

private:
    QTextStream m_inputStream{stdin};
    QSocketNotifier *m_stdinNotifier;
};

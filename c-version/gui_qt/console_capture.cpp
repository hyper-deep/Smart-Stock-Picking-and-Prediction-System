#include "console_capture.h"

#include <QDir>
#include <QFile>
#include <QDebug>

#include <cstdio>
#include <io.h>
#include <fcntl.h>

#ifdef _MSC_VER
#define _freopen freopen
#endif

// 保存当前句柄(可能无效, 返回-1表示无需恢复), 供执行后还原
static int dupCurrentFd(int fd)
{
    if (fd < 0)
        return -1;
    return _dup(fd);
}

// 把 stdout 句柄重定向到文件; 返回旧的 stdout 句柄(用于恢复, -1表示无需恢复)
static int redirectStdoutToFile(const QString& path)
{
    fflush(stdout);
    int oldFd = dupCurrentFd(_fileno(stdout));
    FILE* f = _freopen(path.toLocal8Bit().constData(), "w", stdout);
    (void)f;
    return oldFd;
}

// 把 stdin 重定向到文件(内容为 inputs 每行);
// 返回: >=0 旧句柄(需恢复), -1 输入文件创建失败(错误)
static int redirectStdinFromInputs(const QStringList& inputs, const QString& path)
{
    QFile in(path);
    if (!in.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return -1;
    }
    QByteArray all;
    for (const QString& s : inputs) {
        all += s.toUtf8();
        all += "\n";
    }
    in.write(all);
    in.close();

    int oldFd = dupCurrentFd(_fileno(stdin));
    FILE* f = _freopen(path.toLocal8Bit().constData(), "r", stdin);
    (void)f;
    return oldFd;
}

static void restoreFd(int oldFd, int targetFd)
{
    if (oldFd >= 0) {
        _dup2(oldFd, targetFd);              // 恢复原句柄
        _close(oldFd);
    }
}

QString ConsoleCapture::run(const QStringList& inputs, std::function<void()> func)
{
    // 输出文件
    QString outPath = QDir::temp().filePath("stock_gui_out.txt");
    QString inPath  = QDir::temp().filePath("stock_gui_in.txt");

    int oldOut = redirectStdoutToFile(outPath);
    int oldIn  = redirectStdinFromInputs(inputs, inPath);
    if (oldIn == -1) {                       // 仅输入文件创建失败才是错误
        restoreFd(oldOut, _fileno(stdout));
        return QStringLiteral("输入文件创建失败!\n");
    }

    // 执行目标函数
    if (func) {
        func();
    }
    fflush(stdout);

    // 恢复标准流
    restoreFd(oldIn,  _fileno(stdin));
    restoreFd(oldOut, _fileno(stdout));

    // 读回输出
    QFile out(outPath);
    if (!out.open(QIODevice::ReadOnly)) {
        return QString();
    }
    QByteArray data = out.readAll();
    return QString::fromUtf8(data);
}

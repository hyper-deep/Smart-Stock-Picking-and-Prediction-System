#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "mainwindow.h"

// 定位项目根目录(包含 stocks.txt 的目录), 并切换工作目录到该处,
// 保证各 C 函数 fopen("stocks.txt") / "detail/..." / "entity.txt" 均可用
static bool setupWorkingDirectory()
{
    // 1) 当前目录已经有数据文件
    if (QFileInfo::exists(QDir::currentPath() + "/stocks.txt"))
        return true;

    // 2) 依次尝试: exe 所在目录、其上级、上上级
    QStringList candidates;
    QString exeDir = QCoreApplication::applicationDirPath();
    candidates << exeDir;
    QDir d(exeDir);
    candidates << d.filePath("..") << d.filePath("../..") << d.filePath("../../..");

    for (const QString& c : candidates) {
        QDir cd(c);
        if (cd.exists("stocks.txt")) {
            QDir::setCurrent(cd.absolutePath());
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("智能股票分析与预测系统");

    if (!setupWorkingDirectory()) {
        qWarning() << "未找到 stocks.txt, 请将程序放在项目根目录运行!";
    } else {
        qInfo() << "工作目录:" << QDir::currentPath();
    }

    // 在堆上创建主窗口: MainWindow 内含 Stock stocks[400] (约1.2MB),
    // 若在栈上创建会超过默认1MB栈大小导致栈溢出(0xC00000FD)
    MainWindow* w = new MainWindow;
    w->show();
    int rc = app.exec();
    delete w;
    return rc;
}

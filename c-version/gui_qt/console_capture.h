#pragma once
#include <QString>
#include <functional>

/* 控制台捕获工具:
 * 将 C 函数中的 printf/scanf 与 GUI 隔离 —— 调用前把输入写入临时文件并
 * 重定向 stdin, 将 stdout 重定向到临时文件; 函数执行完毕后读回输出并恢复。
 */
class ConsoleCapture
{
public:
    // inputs: 依次送入 stdin 的每一行输入(内部自动补换行)
    // func:   要执行的 C 函数(可带捕获参数)
    // 返回:   函数执行期间 stdout 捕获到的全部文本
    static QString run(const QStringList& inputs, std::function<void()> func);
};

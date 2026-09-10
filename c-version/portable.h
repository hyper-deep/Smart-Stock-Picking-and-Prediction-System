#ifndef PORTABLE_H
#define PORTABLE_H

#include <stdio.h>
#include <string.h>

/*
 * MSVC 提供 Annex K 安全函数(strcpy_s / sprintf_s / scanf_s / fopen_s 等),
 * 但 MinGW 的 gcc 不提供这些函数。这里为 gcc 环境做一层兼容映射,
 * 使同一份代码在两种编译器下都能编译运行。
 */
#if !defined(_MSC_VER)
    typedef int errno_t;

    #define strcpy_s(dst, size, src)  ((void)snprintf((dst), (size), "%s", (src)))
    #define sprintf_s(dst, size, ...) ((void)snprintf((dst), (size), __VA_ARGS__))
    #define scanf_s scanf
    #define fopen_s(ppf, name, mode)  ((*(ppf) = fopen((name), (mode))) == NULL ? 1 : 0)
#endif

#endif /* PORTABLE_H */

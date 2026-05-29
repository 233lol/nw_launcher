/**
 * run-nwjs - 启动 NW.js 项目的可执行文件
 * 
 * 功能：
 *   将该文件放在 NW.js 项目目录下，双击即可调用系统 PATH 中的 nw 命令启动当前项目。
 *   编译为 Windows GUI 程序，运行时不会弹出控制台窗口。
 *   静态编译，不依赖外部运行时库。
 * 
 * 编译要求：
 *   MinGW-w64 GCC (mingw32-gcc 或 x86_64-w64-mingw32-gcc)
 * 
 * 编译命令：
 *   gcc -O2 -Wall -mwindows -static -static-libgcc -o run-nwjs.exe main.c -lkernel32 -lshell32
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>

/* 检查指定目录下是否存在 package.json */
static int has_package_json(const char *dir)
{
    char path[MAX_PATH];
    _snprintf(path, sizeof(path), "%s\\package.json", dir);
    path[sizeof(path) - 1] = '\0';

    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

/* 获取当前可执行文件所在目录路径 */
static int get_exe_directory(char *buffer, DWORD size)
{
    DWORD len = GetModuleFileNameA(NULL, buffer, size);
    if (len == 0 || len >= size) {
        return -1;
    }

    /* 去掉文件名，只保留目录路径 */
    char *p = strrchr(buffer, '\\');
    if (p) {
        *p = '\0';
        return 0;
    }
    return -1;
}

/**
 * 使用 CreateProcess 启动 nw 命令
 * 
 * @param project_dir 项目目录路径
 * @return 0 成功，-1 失败
 */
static int launch_nwjs(const char *project_dir)
{
    char cmdline[MAX_PATH + 32];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    BOOL ret;

    /* 构建命令行：nw "项目目录路径" */
    _snprintf(cmdline, sizeof(cmdline), "nw \"%s\"", project_dir);

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    /*
     * CREATE_NO_WINDOW: 不为 nw 进程创建控制台窗口
     * 实际 nw 是 GUI 应用，该标志主要确保不产生多余控制台
     */
    ret = CreateProcessA(
        NULL,           /* 不指定模块名，从 PATH 中查找 */
        cmdline,        /* 命令行 */
        NULL,           /* 进程安全属性 */
        NULL,           /* 线程安全属性 */
        FALSE,          /* 不继承句柄 */
        CREATE_NO_WINDOW, /* 不创建控制台窗口 */
        NULL,           /* 使用父进程环境变量 */
        NULL,           /* 使用父进程工作目录（当前目录） */
        &si,            /* 启动信息 */
        &pi             /* 进程信息 */
    );

    if (!ret) {
        return -1;
    }

    /* 关闭进程和线程句柄，不等待 nw 退出 */
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}

/**
 * Windows GUI 程序入口点
 * 
 * 使用 WinMain 而非 main，配合 -mwindows 编译选项，
 * 程序运行时不关联控制台，不会弹出黑窗口。
 */
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    char exe_dir[MAX_PATH];
    int ret;

    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    /* 获取可执行文件所在目录（即 NW.js 项目目录） */
    ret = get_exe_directory(exe_dir, sizeof(exe_dir));
    if (ret != 0) {
        MessageBoxW(NULL,
                    L"获取程序目录失败！",
                    L"run-nwjs 错误",
                    MB_OK | MB_ICONERROR);
        return 1;
    }

    /* 检查项目目录中是否存在 package.json */
    if (!has_package_json(exe_dir)) {
        MessageBoxW(NULL,
                    L"当前目录未找到 package.json 文件！\n"
                    L"请确认本程序已放在有效的 NW.js 项目目录中。",
                    L"run-nwjs 错误",
                    MB_OK | MB_ICONERROR);
        return 1;
    }

    /* 启动 NW.js */
    ret = launch_nwjs(exe_dir);
    if (ret != 0) {
        MessageBoxW(NULL,
                    L"启动 NW.js 失败！\n"
                    L"请确认 nw 命令已在系统 PATH 环境变量中。",
                    L"run-nwjs 错误",
                    MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
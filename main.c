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
 *   gcc -Os -s -static -static-libgcc -static-libstdc++ -mwindows -Wl,--gc-sections -fdata-sections -ffunction-sections
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>

/* 检查指定目录下是否存在 package.json - 使用宽字符 */
static int has_package_json(const WCHAR *dir)
{
    WCHAR path[MAX_PATH * 2];
    _snwprintf(path, sizeof(path)/sizeof(WCHAR), L"%s\\package.json", dir);
    path[sizeof(path)/sizeof(WCHAR) - 1] = L'\0';
    
    DWORD attr = GetFileAttributesW(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

/* 获取当前可执行文件所在目录路径 - 宽字符版 */
static int get_exe_directory(WCHAR *buffer, DWORD size)
{
    DWORD len = GetModuleFileNameW(NULL, buffer, size);
    if (len == 0 || len >= size) {
        return -1;
    }
    
    WCHAR *p = wcsrchr(buffer, L'\\');
    if (p) {
        *p = L'\0';
        return 0;
    }
    return -1;
}

/* 启动 NW.js - 使用宽字符并转换命令行 */
static int launch_nwjs(const WCHAR *project_dir)
{
    /* 构建命令行需要 ANSI 或直接使用宽字符版本 */
    size_t len = wcslen(project_dir);
    WCHAR *cmdline = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
                               (len + 10) * sizeof(WCHAR));
    if (!cmdline) return -1;
    
    _snwprintf(cmdline, len + 10, L"nw \"%s\"", project_dir);
    
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    BOOL ret = CreateProcessW(NULL, cmdline, NULL, NULL, FALSE,
                              CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    
    HeapFree(GetProcessHeap(), 0, cmdline);
    
    if (!ret) return -1;
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}

/* WinMain 改为宽字符版本 */
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    WCHAR exe_dir[MAX_PATH];
    
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
    
    if (get_exe_directory(exe_dir, MAX_PATH) != 0) {
        MessageBoxW(NULL,
                    L"获取程序目录失败！",
                    L"run-nwjs 错误",
                    MB_OK | MB_ICONERROR);
        return 1;
    }
    
    if (!has_package_json(exe_dir)) {
        MessageBoxW(NULL,
                    L"当前目录未找到 package.json 文件！\n"
                    L"请确认本程序已放在有效的 NW.js 项目目录中。",
                    L"run-nwjs 错误",
                    MB_OK | MB_ICONERROR);
        return 1;
    }
    
    if (launch_nwjs(exe_dir) != 0) {
        MessageBoxW(NULL,
                    L"启动 NW.js 失败！\n"
                    L"请确认 nw 命令已在系统 PATH 环境变量中。",
                    L"run-nwjs 错误",
                    MB_OK | MB_ICONERROR);
        return 1;
    }
    
    return 0;
}
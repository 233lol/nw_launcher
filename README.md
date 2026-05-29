# run-nwjs

一个轻量级的 C 语言可执行文件，用于启动 NW.js 项目。

## 功能

- 将 `run-nwjs.exe` 放在 NW.js 项目目录下，双击即可启动项目
- 通过系统 `PATH` 环境变量查找全局安装的 `nw` 命令
- 编译为 Windows GUI 程序，**运行时不显示控制台窗口**
- **静态编译**，单个 EXE 文件，不依赖外部 DLL 运行库

## 使用方式

1. 将编译好的 `run-nwjs.exe` 复制到 NW.js 项目目录中
2. 双击 `run-nwjs.exe` 即可启动项目
3. 确保 `nw` 命令已加入系统 `PATH` 环境变量

## 编译方法

### 环境要求

- [MinGW-w64](https://www.mingw-w64.org/)（Windows 下的 GCC 编译器）
- `make` 工具（可选，可以直接用 gcc 命令）

### 使用 Make 编译

```bash
# 编译标准版本
make

# 编译精简 Release 版本（体积更小）
make release

# 清理编译产物
make clean
```

### 直接使用 GCC 编译

```bash
# 先编译资源文件（生成图标等）
windres resource.rc -O coff -o resource.o

# 编译主程序
gcc -O2 -Wall -mwindows -static -static-libgcc -o run-nwjs.exe main.c resource.o -lkernel32 -lshell32
```

### 交叉编译（Linux 下编译 Windows 可执行文件）

```bash
# 先编译资源文件
x86_64-w64-mingw32-windres resource.rc -O coff -o resource.o

# 编译主程序
x86_64-w64-mingw32-gcc -O2 -Wall -mwindows -static -static-libgcc -o run-nwjs.exe main.c resource.o -lkernel32 -lshell32
```

## 编译参数说明

| 参数 | 作用 |
|------|------|
| `-mwindows` | 编译为 GUI 程序，不关联控制台 |
| `-static` | 静态链接所有库 |
| `-static-libgcc` | 静态链接 GCC 运行时库 |
| `-s` | 去除符号表，缩小文件体积（Release 用） |

## 图标

`run-nwjs.exe` 自带应用图标，由 `app.ico` 通过 `resource.rc` 资源文件编译嵌入。

如需替换图标，请将自定义的 `app.ico` 文件放在项目目录中，然后重新编译即可。

## 错误处理

启动失败时会弹出错误提示对话框：
- **获取程序目录失败**：程序文件可能已损坏
- **当前目录未找到 package.json 文件**：请确认程序已放在有效的 NW.js 项目目录中
- **启动 NW.js 失败**：请确认 `nw` 命令已在系统 PATH 环境变量中

## 工作原理

1. 程序获取自身所在目录路径
2. 检查该目录下是否存在 `package.json`，若不存在则提示错误并退出
3. 执行 `nw "项目目录路径"` 命令
4. 使用 `CREATE_NO_WINDOW` 标志创建进程，不产生多余控制台窗口
5. 启动后立即退出，NW.js 进程独立运行

## 项目文件

| 文件 | 说明 |
|------|------|
| `main.c` | 主程序源码 |
| `app.ico` | 应用程序图标（32×32 32位色） |
| `resource.rc` | Windows 资源脚本，嵌入图标 |
| `Makefile` | 构建脚本 |
| `README.md` | 本说明文档 |

## 许可证

MIT License
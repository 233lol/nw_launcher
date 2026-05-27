package main

import (
	"os"
	"os/exec"
	"path/filepath"
	"syscall"
	"unsafe"
)

// showError 使用系统原生 MessageBox 弹出错误提示
func showError(title, msg string) {
	user32 := syscall.NewLazyDLL("user32.dll")
	msgBox := user32.NewProc("MessageBoxW")
	titlePtr, _ := syscall.UTF16PtrFromString(title)
	msgPtr, _ := syscall.UTF16PtrFromString(msg)
	// MB_OK | MB_ICONERROR = 16
	syscall.SyscallN(msgBox.Addr(), 0, uintptr(unsafe.Pointer(msgPtr)), uintptr(unsafe.Pointer(titlePtr)), 16)
}

func main() {
	// 1. 获取启动器自身所在目录（即当前项目根目录）
	exePath, err := os.Executable()
	if err != nil {
		showError("路径错误", "无法获取启动器自身路径。")
		return
	}
	projectDir, _ := filepath.Abs(filepath.Dir(exePath))

	// 2. 检查 package.json 是否存在（NW.js 项目标识）
	pkgJsonPath := filepath.Join(projectDir, "package.json")
	if _, err := os.Stat(pkgJsonPath); os.IsNotExist(err) {
		showError("非 NW.js 项目", "当前目录下未找到 package.json。\n请将此启动器放在 NW.js 项目根目录下使用。")
		return
	} else if err != nil {
		// 其他错误：权限不足、文件被占用等
		showError("文件访问异常", "无法读取 package.json：\n"+err.Error())
		return
	}

	// 3. 从系统 PATH 中查找 nw.exe
	nwPath, err := exec.LookPath("nw")
	if err != nil {
		showError("环境未配置", "未在系统 PATH 中找到 nw.exe。\n请将 NW.js 主程序目录添加到系统环境变量 PATH 中。")
		return
	}

	// 4. 构造启动命令
	cmd := exec.Command(nwPath, projectDir)
	cmd.Dir = projectDir

	// 5. 启动 NW.js 进程
	// 使用 Start() 而非 Run()：启动器无需阻塞等待 NW.js 退出，符合常规启动器体验
	if err := cmd.Start(); err != nil {
		showError("启动失败", "调用 NW.js 时发生错误：\n"+err.Error())
		os.Exit(1)
	}
}
package main

import (
	"os"
	"os/exec"
	"path/filepath"
	"syscall"
	"unsafe"
)

var (
	user32 = syscall.NewLazyDLL("user32.dll")
	msgBox = user32.NewProc("MessageBoxW")
)

const (
	mbOk       = 0x00000000
	mbIconStop = 0x00000010
)

func showError(title, message string) {
	titlePtr, _ := syscall.UTF16PtrFromString(title)
	msgPtr, _ := syscall.UTF16PtrFromString(message)
	msgBox.Call(0, uintptr(unsafe.Pointer(msgPtr)), uintptr(unsafe.Pointer(titlePtr)), mbOk|mbIconStop)
}

func main() {
	// 获取可执行文件自身的路径
	exePath, err := os.Executable()
	if err != nil {
		showError("错误", "无法获取可执行文件路径: "+err.Error())
		os.Exit(1)
	}

	// 获取可执行文件所在目录（即NW.js项目目录）
	projectDir := filepath.Dir(exePath)

	// 切换到项目目录
	if err := os.Chdir(projectDir); err != nil {
		showError("错误", "无法切换到项目目录:\n"+projectDir+"\n"+err.Error())
		os.Exit(1)
	}

	// 检查 package.json 是否存在
	packageJSON := filepath.Join(projectDir, "package.json")
	if _, err := os.Stat(packageJSON); os.IsNotExist(err) {
		showError("NW.js 启动器", "在当前目录找不到 package.json\n请确认本程序放在NW.js项目目录下")
		os.Exit(1)
	}

	// 在 PATH 中查找 nw 可执行文件
	nwPath, err := exec.LookPath("nw")
	if err != nil {
		showError("NW.js 启动器", "找不到 NW.js 运行时 (nw)\n请确认 NW.js 已安装并添加到 PATH 环境变量中")
		os.Exit(1)
	}

	// 启动 NW.js，以项目目录作为工作目录
	cmd := exec.Command(nwPath, ".")
	cmd.Dir = projectDir
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		if exitErr, ok := err.(*exec.ExitError); ok {
			os.Exit(exitErr.ExitCode())
		}
		showError("NW.js 启动器", "启动 NW.js 失败:\n"+err.Error())
		os.Exit(1)
	}
}
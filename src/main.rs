#![windows_subsystem = "windows"]

use std::env;
use std::process::Command;

fn main() {
    let exe_path = match env::current_exe() {
        Ok(p) => p,
        Err(e) => {
            show_error(&format!("无法获取可执行文件路径: {}", e));
            return;
        }
    };

    let project_dir = match exe_path.parent() {
        Some(d) => d.to_path_buf(),
        None => {
            show_error("无法获取项目目录");
            return;
        }
    };

    // 检查 package.json
    let pkg = project_dir.join("package.json");
    if !pkg.exists() {
        show_error(&format!(
            "未找到 package.json\n\n\
             启动 NW.js 需要 package.json 文件。\n\
             请在以下目录创建 package.json：\n\
             {}",
            project_dir.display()
        ));
        return;
    }

    if let Err(e) = std::fs::metadata(&pkg) {
        show_error(&format!("无法读取 package.json: {}\n路径: {}", e, pkg.display()));
        return;
    }

    match Command::new("nw")
        .arg(&project_dir)
        .current_dir(&project_dir)
        .stdin(std::process::Stdio::null())   // 断开 stdin
        .stdout(std::process::Stdio::null())   // 断开 stdout
        .stderr(std::process::Stdio::null())   // 断开 stderr
        .spawn()
    {
        Ok(_) =>{}
        Err(e) => {
            show_error(&format!(
                "启动 NW.js 失败，请确认 NW.js 已安装且 'nw' 已在 PATH 环境变量中。\n\n错误: {}\n项目目录: {}",
                e,
                project_dir.display()
            ));
        }
    }
}

#[link(name = "user32")]
unsafe extern "system" {
    fn MessageBoxW(
        hWnd: *mut std::ffi::c_void,
        lpText: *const u16,
        lpCaption: *const u16,
        uType: u32,
    ) -> i32;
}

fn show_error(msg: &str) {
    let wmsg: Vec<u16> = msg.encode_utf16().chain(std::iter::once(0)).collect();
    let title: Vec<u16> = "Run.js - 错误"
        .encode_utf16()
        .chain(std::iter::once(0))
        .collect();
    unsafe {
        MessageBoxW(std::ptr::null_mut(), wmsg.as_ptr(), title.as_ptr(), 0x00000010 | 0x00040000);
    }
}
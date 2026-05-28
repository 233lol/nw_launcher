fn main() {
    // 仅在 Windows 上嵌入图标资源
    if std::env::var("CARGO_CFG_TARGET_OS").unwrap_or_default() == "windows" {
        embed_resource::compile("icon.rc", embed_resource::NONE);
    }
}
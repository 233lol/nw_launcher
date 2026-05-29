# Makefile for Game
# 编译为 Windows GUI 程序，静态链接，不显示控制台
#
# 使用方法：
#   make          - 编译生成 run-nwjs.exe
#   make release  - 编译 release 版本（带 LTO 优化）
#   make clean    - 清理编译产物

CC      = gcc
CFLAGS  = -Os -Wall -Wextra -flto -funroll-loops 
LDFLAGS = -mwindows -static -static-libgcc -static-libstdc++ -flto -s -Wl,--gc-sections -fdata-sections -ffunction-sections
LIBS    = -lkernel32 -lshell32

TARGET  = Game.exe
RES_RC  = icon.rc
RES_OBJ = resource.o

# 默认目标
all: $(RES_OBJ) $(TARGET)

# 编译资源文件（需要在目录下有 app.ico）
$(RES_OBJ): $(RES_RC)
	windres $< -O coff -o $@

# 编译规则
$(TARGET): main.c $(RES_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c $(RES_OBJ) $(LIBS)
ifdef windir
endif

# Release 目标：使用 LTO 优化，缩小文件体积
release: CFLAGS = -Os -Wall -Wextra -flto -funroll-loops 
release: LDFLAGS = -mwindows -static -static-libgcc -static-libstdc++ -flto -s -Wl,--gc-sections -fdata-sections -ffunction-sections
release: clean all
	@$(if $(windir),for %%I in ($(TARGET)) do @echo    %%~zI 字节, ls -lh $(TARGET) 2>/dev/null || stat -c %s $(TARGET))

# 清理
clean:
	$(RM) $(TARGET) $(RES_OBJ)

.PHONY: all release clean
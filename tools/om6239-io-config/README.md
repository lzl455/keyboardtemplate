# OM62xx 引脚配置器

类似 STM32CubeMX 的引脚配置工具，单个 HTML 文件，不用安装。用浏览器（Chrome / Edge）直接打开 `index.html` 即可，不需要联网（没网时字体会退回系统字体）。

## 能做什么

- **封装视图**：支持两种封装，在顶部切换：
  - OM6252 QFN48（默认）：SE73K 板子实际用的封装，27 个 GPIO。PA0–PA7、PB0–PB5 是键盘列扩展输出，由 P32（KEY_EX）打时钟依次切换，不经 GPIO 寄存器配置，图上用虚线表示。
  - OM6239 QFN32：手册 V0.1 Figure 1.1，26 个 GPIO。

  点引脚设置功能、标签、上下拉、驱动能力、初始电平、深睡唤醒、输入缓冲，并显示该脚占用的寄存器位。
- **外设**：USB、UART、ARGB（LEDC）、滚轮编码器、LVD、SWD、传感器扫描，选了自动占脚；每个脚只列出它支持的功能。
- **检查**：UART TX/RX 不同组、编码器 A/B 不成对、唤醒脚没上下拉、矩阵行列没上拉、用了封装上没引出的脚等。
- **生成代码**：`io_config.h`（引脚序号、分组掩码、读写宏）和 `io_config.c`（`io_config_init()`、`io_config_wakeup()`）。
- **寄存器解码**：粘贴寄存器值，逐脚列出含义并和配置器生成值对比。内置了 `main.c`、`drv_pmu.c` 里的初始化值作为示例。
- **其他**：撤销（Ctrl+Z）、JSON 导入导出。配置保存在浏览器本地存储里，换电脑请用导出 / 导入。

打开时默认载入本工程（SE73K 键盘）的引脚配置，是从 `projects/mouse_demo/main/main.h` 和 `main.c:199-207` 还原的。

## 把生成的代码接进工程

1. 把 `io_config.h`、`io_config.c` 放到 `projects/mouse_demo/periph/`，在 Keil 的 periph 组里加上 `io_config.c`。
2. 在 `main()` 里用 `io_config_init();` 替换 `main.c:197-207` 直接写寄存器的几行。
3. 进深睡前调用 `io_config_wakeup(true)`，醒来后调用 `io_config_wakeup(false)`。

## 依据和注意事项

- 引脚复用（UART、LEDC、编码器、LVD、SWD、传感器扫描）取自手册 15.3 节 IO pin maps 和 SDK 的 `drv_pmu.h`、`drv_encoder.h`。
- 寄存器位定义取自 SDK 的 `hal/device/om6239/pmu_reg.h`。
- **上拉编码**：按手册 V0.1，01 = 30k、10 = 150k。SDK `pmu_reg.h` 里原来两个宏的值写反了，已按手册改正。另外 SDK 注释说 P04-P07 选 01 时上拉是 2M，阻值以实测为准。
- **保留脚**：P14–P17 接芯片内部 Flash，P37 是 BOOT 启动脚，不能配置。生成代码时这几个脚保持工程现有的值（无上下拉、驱动 24mA、输入缓冲按复位值）。
- **PWM 通道**（来自 OM6252 封装图）：P00 = PWM0、P01 = PWM1、P04 = PWM2、P05 = PWM3、P26 = PWM6、P32 = PWM7。P06、P22 图上只标了 PWM，是 PWM4/PWM5 中的一个，具体对应待确认。
- **P35**：OM6252 QFN48 引出了 P35（第 3 脚），本工程用它做模式开关；手册的 QFN32 没有引出。
- **`main.c:206` 疑似笔误**：`OUTENSET = 0x040800003` 是 9 位十六进制，按数值等于 `0x40800003`，会把 P36（USB 5V 检测）设成输出。原意很可能是 `0x04080003`，即配置器生成的值。建议上板验证后修正。
- 生成的代码用 clang（Cortex-M0，`-Wall`）对着本仓库的 SDK 头文件做过语法检查，0 警告；尚未在 Keil 里编译或上板运行。

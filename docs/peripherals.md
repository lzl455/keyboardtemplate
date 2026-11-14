# 可选外设：换灯、加编码器、新增外设

可选外设（灯光、旋转编码器，以及以后新增的外设）都从主流程里拆出来，各自放在独立目录里。主流程只调用一组统一入口，不直接调用任何外设模块里的函数。需要某个外设时打开开关、把它的目录加进 Keil 工程；不需要时关掉开关，目录也可以整个移除。

## 结构

```
projects/mouse_demo/
├─ main/ mode_24g/ mode_usb/ periph/ ...   核心：三种模式、按键、指示灯、电量、Flash
│     │
│     │  只调用 periph/periph.h 的统一入口
│     ▼
├─ periph/periph.c            外设表 + 统一入口（核心的一部分）
│     │  按 board_config.h 的开关，把启用的模块挂进表里
│     ▼
└─ modules/                   可选外设，每个目录一个模块
   ├─ light/                  灯光（BOARD_LIGHT）
   │   ├─ light.c             模块入口 light_periph + ARGB 灯效引擎
   │   ├─ light_modes.c       ARGB 灯效花样（只给炫彩灯）
   │   ├─ light_tables.h      ARGB 灯效表
   │   ├─ light_pwm_fx.c      PWM 灯（单色背光 / RGB 灯）专用的灯效
   │   ├─ light_fx.h          模块内部共享的声明
   │   └─ light_port.* / light_argb.c / light_pwm.c   输出后端，ARGB 和 PWM 二选一
   └─ knob/                   旋转编码器（BOARD_HAS_KNOB）
       └─ knob.c              模块入口 knob_periph + 编码器中断
```

统一入口和调用位置：

| 入口 | 谁调用 | 时机 | 灯光 | 编码器 |
| --- | --- | --- | --- | --- |
| `periph_init_all()` | `main.c` 主循环 | 进入某个模式 | | 初始化编码器 |
| `periph_scan_all()` | `matrixkey_scan()` | 每次扫键后、生成报文前 | | 发音量键 / 滚轮 |
| `periph_frame_all()` | `ui_frame_process()` | 每帧（约 7–16ms） | 刷新一帧灯效 | |
| `periph_flush_all()` | 各模式主循环、2.4G RTC 中断 | 每帧 | 把颜色送到灯上 | |
| `periph_blank_all()` | 同步中、休眠前、低电关机、恢复出厂 | 需要关灯时 | 关灯 | |
| `periph_sleep_all(mask)` | `mouse_periph_wakeup_enable()` | 休眠级别变化 | | 浅睡可唤醒、深睡关掉 |
| `periph_busy_any()` | 蓝牙 `pm_checker_callback()` | 判断能否进 STOP1 | 灯亮着返回 true | |

核心里留下、不随模块移走的：

- **指示灯**（Num / Caps / 模式灯，`led/led.c`）：和矩阵列共用引脚，属于键盘本身。
- **灯效设置**（模式、亮度、速度、颜色、开关，`led/led.c`）：按键、Flash、USB 配置工具都会读写。去掉灯光模块后这些设置照常保存，只是没有灯来显示。
- **配置保存**（`flash_save_pending()`）：原来藏在 `logo_process()` 开头，现在由 `ui_frame_process()` 每帧调用，和灯光模块无关。

所有开关和参数都在 `projects/mouse_demo/config/board_config.h`，其中灯光和编码器部分可以由配置器生成到同目录的 `io_board.h`，优先级：Keil Define > `io_board.h` > `board_config.h` 默认值。驱动开关 `RTE_LEDC`、`RTE_PWM`、`RTE_ENCODER` 由它自动推出，不用再去改 `RTE_om6239.h`。

## 选灯：ARGB 和 PWM 二选一

灯光只能选一种：ARGB 灯珠（LEDC）、PWM 单色背光（1 路）、PWM RGB 灯（3 路），或者不用灯。`board_config.h` 的 `BOARD_LIGHT` 只能取 `LIGHT_NONE / LIGHT_ARGB / LIGHT_PWM`，写错会直接编译报错。

推荐用引脚配置器（`tools/om6239-io-config`）：

1. 在「外设」栏的「灯光」里选一种，下面会出现对应的设置：
   - ARGB：数据脚（LEDC 只能出在 P01 / P05 / P21 / P33）
   - PWM 单色：背光脚；PWM RGB：R、G、B 三个脚（下拉框里标了每个脚的 PWM 通道号）
   - PWM：点亮电平（高 / 低）
   - 灯电源开关脚：低功耗时给灯断电，没有就选「没有」
2. 切换灯光类型时，上一种用到的脚会被清掉；在单个引脚上选 ARGB 或 PWM 功能，也会自动清掉另一种。占用了别的功能的脚会提示被替换了什么，可以撤销。
3. 到「生成代码」页，重新复制三个文件：`io_board.h` 覆盖 `projects/mouse_demo/config/io_board.h`，`io_config.h`、`io_config.c` 覆盖 `projects/mouse_demo/periph/` 里的同名文件，然后重新编译。

PWM 通道号按 OM6252 QFN48 封装图：P00 = PWM0、P01 = PWM1、P04 = PWM2、P05 = PWM3、P26 = PWM6、P32 = PWM7。P06、P22 是 PWM4/PWM5 中的一个，配置器先按 PWM4、PWM5 生成并给出警告，要上板确认。注意本工程 P00 是灯电源、P01 是 ARGB 数据、P04/P05 是矩阵行、P26 是矩阵列、P32 是列扩展时钟，换成 PWM 灯时要一起调整引脚分配。

也可以不用配置器，直接改 `board_config.h`（`io_board.h` 里没设的项才会用这里的默认值）：

```c
#define BOARD_LIGHT        LIGHT_PWM
#define BOARD_PWM_LIGHT    PWM_LIGHT_MONO   // 单色背光；一颗 RGB 灯用 PWM_LIGHT_RGB
#define BOARD_PWM_CH_MONO  1                // RGB 灯填 BOARD_PWM_CH_R / G / B
#define BOARD_PWM_ACTIVE_HIGH 1             // 低电平点亮改成 0
#define BOARD_LIGHT_PWR_PIN   0             // 没有电源开关脚设成 -1
```

### PWM 灯的灯效（不和 ARGB 共用）

ARGB 的灯效是按一排灯珠设计的，流光、拖尾这类效果合成到一路 / 三路 PWM 上只剩常亮或者忽明忽暗，所以 PWM 灯用单独的一套（`modules/light/light_pwm_fx.c`），选 `LIGHT_PWM` 时自动换上，ARGB 那套不编进固件：

| 快捷键 | 单色背光 | RGB 灯 |
|---|---|---|
| 灯效 0 | 呼吸 | 色彩渐变 |
| 灯效 1 | 闪烁 | 呼吸 |
| 灯效 2 | 心跳 | 心跳 |
| 灯效 3 | 按键触发（按键时亮，松开后慢慢熄灭） | 按键触发 |
| 灯效 4 | 常亮 | 常亮（已经是常亮时再按换颜色） |

- 「灯效切换」键只在这几种之间循环：单色 5 种，RGB 6 种（多一个闪烁，只能用切换键选到）。恢复出厂后的灯效：单色是常亮，RGB 是色彩渐变。
- 颜色（RGB 灯）沿用原来的 8 种：红、绿、蓝、紫、黄、青、白、七彩。选七彩时：常亮慢慢变色，呼吸 / 闪烁 / 心跳每个周期换一种颜色，按键触发每按一次换一种颜色。
- 亮度 3 档（约 12% / 50% / 100%）、速度 3 档沿用原来的按键和 Flash 保存。亮度加了平方曲线校正，呼吸看起来更均匀。
- 灯效按 RTC 实际经过的时间推进，蓝牙、2.4G、USB 下速度一样。
- 编号和快捷键对应在 `led/led.h` 的 `PWM_FX_*`、`LIGHT_MODE_*`、`LIGHT_SHORTCUT_*`。Flash 里存的灯效编号超出范围时（比如从 ARGB 固件换成 PWM 固件）自动用默认灯效。

## 加旋转编码器

推荐用引脚配置器（`tools/om6239-io-config`）：

1. 在「外设」栏的「旋转编码器」里选一组引脚，下面会出现编码器设置：用途（音量旋钮 / 鼠标滚轮）、方向反转、去抖时间。
2. 到「生成代码」页，重新复制三个文件：`io_board.h` 覆盖 `projects/mouse_demo/config/io_board.h`，`io_config.h`、`io_config.c` 覆盖 `projects/mouse_demo/periph/` 里的同名文件（编码器脚的上拉在这里）。
3. 重新编译。

也可以不用配置器，直接改 `board_config.h`（`io_board.h` 里没设的项才会用这里的默认值）：

1. 在 `board_config.h` 里设置：
   ```c
   #define BOARD_HAS_KNOB     1
   #define BOARD_KNOB_PINS    ENCODER_PINMUX_SEL_IO_20_21   // 四组可选，见注释
   #define BOARD_KNOB_ACTION  KNOB_ACTION_VOLUME            // 或 KNOB_ACTION_WHEEL
   ```
2. 编码器两根脚需要上拉，在引脚配置器里设置。
3. 方向反了就把 `BOARD_KNOB_INVERT` 改成 1；计数抖动就把 `BOARD_KNOB_DEBOUNCE` 调大（最大 `ENCODER_DEBOUNCE_TIME_1MS`）。

按现在的引脚分配，四组可选脚里只有 P20/P21 是空的：P00/P01 被 ARGB 占用，P05/P06 是矩阵行，P24/P25 是矩阵列。

音量模式下每转一格，发一次「音量加 / 减」的按下和松开，走现有的多媒体键通路，三种模式都支持。滚轮模式把转动量写进鼠标报文的滚轮字段。浅睡时转动编码器可以唤醒，深睡前编码器会关掉。

## 新增一个外设

1. 建目录 `projects/mouse_demo/modules/xxx/`，写 `xxx.c`，照着 `modules/knob/knob.c`：
   ```c
   #include "periph.h"

   #if (BOARD_HAS_XXX)
   static void xxx_init(void)  { ... }
   static void xxx_scan(void)  { ... }

   const periph_ops_t xxx_periph = {
       .name  = "xxx",
       .init  = xxx_init,     // 进入模式时
       .scan  = xxx_scan,     // 每次扫键后，可往按键缓冲里注入按键
       // .frame / .flush / .blank / .sleep / .busy 用不到就不写
   };
   #endif
   ```
   整个文件用 `#if (BOARD_HAS_XXX)` 包住，关掉时编译为空。需要中断的外设，中断函数（如 `XXX_IRQHandler`）也写在这个文件里，启动文件里有默认的弱定义。
2. 在 `config/board_config.h` 加开关 `BOARD_HAS_XXX`（默认 0）和引脚参数。
3. 在 `periph/periph.c` 的外设表里加一行，同样用 `#if (BOARD_HAS_XXX)` 包住：
   ```c
   #if (BOARD_HAS_XXX)
   extern const periph_ops_t xxx_periph;
   #endif
   ...
   #if (BOARD_HAS_XXX)
       &xxx_periph,
   #endif
   ```
4. Keil 工程里新建一个组（例如 `module_xxx`）加上这个目录的 `.c` 文件，并把 `..\modules\xxx` 加进头文件路径。

主流程、休眠代码都不用改。引脚先用配置器检查冲突。

## 去掉一个外设

1. 把它的开关设成 0（灯光是 `BOARD_LIGHT = LIGHT_NONE`，编码器是 `BOARD_HAS_KNOB = 0`），可以在 `board_config.h`、`io_board.h` 或 Keil Define 里设。
2. 只改开关就够了，模块文件会编译为空。想让工程更干净，也可以在 Keil 里删掉对应的 `module_xxx` 组。
3. 如果开关打开了但模块文件没加进工程，链接时会报找不到 `xxx_periph`，按提示把模块加回来即可。

## 可选功能：USB、FCC 测试、灯光 Fn 键

这几项不是独立外设，但同样可以关掉，关掉后对应代码不编进固件。开关在 `board_config.h`，也可以在配置器的「固件功能」里选，生成到 `io_board.h`。

| 开关 | 默认 | 关掉后去掉的代码 | 大小（估算） |
| --- | --- | --- | --- |
| `BOARD_HAS_USB` | 1 | USB 协议栈：`hal_usb_decs.c`、`main_usb.c`、`usb_user_process.c`，以及进入 USB 模式的分支 | 约 9KB |
| `BOARD_HAS_FCC_TEST` | 1 | FCC / SRRC 认证测试 `rf_fcc.c`，以及配置命令、配对时进入测试的入口 | 约 1.8KB |
| `BOARD_LIGHT_KEYS` | 跟着灯光 | 灯光 Fn 键（亮度、模式、速度、颜色、开关灯）的处理，没有灯时自动去掉 | 约 0.7KB |

说明：

- **USB**：目前 `scan_mode_swtich()` 里检测 5V 并进入 USB 模式的代码是注释掉的，所以 USB 代码现在编进去了也不会运行。打开 `BOARD_HAS_USB` 后还要恢复那段代码，键盘才会真正进入 USB 模式。配置协议 `hid_process.c` 在 2.4G 模式下也要用（通过接收器转发），所以留在核心，不受这个开关影响。
- **灯光 Fn 键**：这两个按键处理函数会被预编译的 `userdefined_key.o` 调用，关掉时函数名保留、函数体为空。
- **电量检测（LVD）是必需功能**，没有开关，始终编进固件。配置器里的「电压检测」只用来选检测脚。

全部关掉（无灯、无编码器、无 USB、无 FCC 测试）时，用 clang 估算，整个工程的代码从约 58KB 降到约 43KB。实际大小以 Keil 编译结果为准。

## 一套代码出多个型号

`board_config.h` 里带 `#ifndef` 的开关，可以在 Keil 的 C/C++ Define 里覆盖，例如：

```
BOARD_LIGHT=LIGHT_PWM,BOARD_PWM_LIGHT=PWM_LIGHT_RGB,BOARD_HAS_KNOB=1
```

可以给每个型号建一个 Keil Target，用同一份源码。

## 上板要验证的

默认配置（ARGB、无编码器）下，固件行为应该和改动前一样，建议重点看：

- 灯效、亮度档位、模式指示灯正常。
- 三种模式下浅睡、深睡、唤醒正常，休眠时灯会灭。

PWM 后端和编码器模块只在代码层面检查过，打开后需要：

- 确认 PWM 通道和引脚对应（尤其 P06/P22）、点亮电平、闪烁频率。
- PWM 灯效：5 / 6 种灯效、亮度 3 档、速度 3 档、颜色切换（RGB）、按键触发。
- 确认编码器方向、每格计数，以及浅睡时转动能唤醒。

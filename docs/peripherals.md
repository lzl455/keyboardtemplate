# 可选外设：换灯、加编码器、新增外设

可选外设（灯、编码器，以及以后新增的外设）都从 USB、2.4G、蓝牙三种模式的主循环里拆出来了。换灯、加外设只需要改配置或新增文件，主循环和休眠代码不用动。

## 结构

```
三种模式主循环 + 休眠路径
  │
  ├─ 灯光：灯效算法 logo_mode.c / led.c 写颜色缓冲 pwm_rgb_tbl
  │        └─ 输出层 light_port.h，按 BOARD_LIGHT 编进一个后端
  │             ├─ light_argb.c   LEDC 驱动 WS2812 类灯珠
  │             ├─ light_pwm.c    PWM 驱动单色背光（1 路）或一颗 RGB 灯（3 路）
  │             └─ light_port.c   无灯时的空实现
  │
  └─ 其他外设：periph.h 的模块表，三个统一入口
       ├─ periph_init_all()        进入模式时（main.c）
       ├─ periph_task_all()        每拍扫键后、生成报文前（matrix_key.c）
       └─ periph_sleep_all(mask)   休眠级别变化时（main.c 的 mouse_periph_wakeup_enable）
            └─ knob.c  旋转编码器（音量 / 滚轮），也是新增外设的样板
```

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

灯效算法不用改。PWM 后端把 8 颗灯的颜色折算成占空比：单色取每颗灯最亮的一路再平均，RGB 按颜色分别平均。所以呼吸、常亮、颜色切换照常可用，流水类灯效表现为整体亮度变化。

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

1. 新建 `projects/mouse_demo/periph/xxx.c`，照着 `knob.c` 定义：
   ```c
   const periph_ops_t xxx_periph = {
       .name  = "xxx",
       .init  = xxx_init,     // 进入模式时
       .task  = xxx_task,     // 每拍扫键后，可往按键缓冲里注入按键
       .sleep = xxx_sleep,    // 休眠级别变化时，参数同 mouse_periph_wakeup_enable()
   };
   ```
   用不到的函数填 NULL。
2. 在 `board_config.h` 加开关 `BOARD_HAS_XXX` 和引脚参数。
3. 在 `periph.c` 的 `periph_table` 里加一行，用 `#if (BOARD_HAS_XXX)` 包住。
4. 在 Keil 工程的 periph 组里加上 `xxx.c`。

引脚先用配置器检查冲突。需要中断的外设，在 `hal/driver/om6239/drv_isr.c` 对应的中断函数里调用模块提供的函数，参考 `ENCODER_IRQHandler` 调用 `knob_isr()` 的写法。

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
- 确认编码器方向、每格计数，以及浅睡时转动能唤醒。

# MultiLED

一个面向嵌入式系统的轻量级 LED 状态管理框架。

设计思想参考 [MultiButton](https://github.com/0x1abin/MultiButton)，通过对象化管理和状态机实现多个 LED 的统一控制。

## 特性

- 非阻塞，状态机驱动
- 与 MCU GPIO 解耦，通过函数指针抽象
- 支持裸机和 FreeRTOS
- 轻量级，适合资源受限的嵌入式环境

## 支持模式

| 模式 | 说明 | 默认参数 |
|------|------|----------|
| `LED_MODE_OFF` | 常灭 | - |
| `LED_MODE_ON` | 常亮 | - |
| `LED_MODE_BLINK_SLOW` | 慢闪 | 1000ms |
| `LED_MODE_BLINK_FAST` | 快闪 | 200ms |
| `LED_MODE_BLINK_N_TIMES` | 指定次数闪烁 | - |
| `LED_MODE_HEARTBEAT` | 心跳灯 | 2000ms |

## 文件结构

```
MultiLED
├── inc/
│   ├── multi_led.h            # 头文件
│   └── multi_led_config.h     # 默认配置 (可覆盖)
├── src/
│   └── multi_led.c            # 实现
└── examples/
    └── main.c                 # 使用示例
```

## 集成方式

### 方式一：Git Submodule（推荐）

```bash
git submodule add git@github.com:darwinstudio/MultiLED.git drivers/MultiLED
```

然后在 IDE 中添加头文件路径 `drivers/MultiLED/inc` 和源文件 `drivers/MultiLED/src/multi_led.c`。

### 方式二：复制文件

将 `inc/` 和 `src/` 目录复制到你的项目中。

### 自定义配置

默认参数在 `inc/multi_led_config.h` 中定义。如需修改，在自己项目中创建同名文件 `multi_led_config.h`，并将自己的目录优先加入头文件搜索路径：

```
your_project/
├── config/
│   └── multi_led_config.h   # 你的自定义配置 (优先级高于库的默认配置)
├── drivers/
│   └── MultiLED/            # submodule
└── ...
```

IDE 中将 `config/` 目录放在 `drivers/MultiLED/inc` 前面即可生效。

自定义配置示例：

```c
#ifndef __MULTI_LED_CONFIG_H
#define __MULTI_LED_CONFIG_H

#define BLINK_SLOW_PERIOD       500     // 改为 500ms
#define BLINK_FAST_PERIOD       100     // 改为 100ms
#define HEARTBEAT_PERIOD        1500    // 改为 1500ms

#endif
```

### 使用步骤

#### 1. 包含头文件

```c
#include "multi_led.h"
```

#### 2. 实现 GPIO 回调

```c
void led_write_pin(uint8_t id, uint8_t level)
{
    switch (id)
    {
        case 0: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (GPIO_PinState)level); break;
        case 1: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (GPIO_PinState)level); break;
    }
}
```

#### 3. 初始化和使用

```c
// 初始化
multi_led_init();

// 创建 LED
led_t led_run;
led_create(&led_run, 0, led_write_pin);

// 设置模式
led_set_mode(&led_run, LED_MODE_HEARTBEAT);
```

#### 4. 周期调用

**裸机：**

```c
while (1)
{
    multi_led_process(HAL_GetTick());
}
```

**FreeRTOS：**

```c
void led_task(void *argument)
{
    for (;;)
    {
        multi_led_process(HAL_GetTick());
        osDelay(10);  // 推荐 10~20ms
    }
}
```

## API

| 函数 | 说明 |
|------|------|
| `multi_led_init()` | 初始化 LED 管理器 |
| `led_create(led, id, write_pin)` | 创建 LED 对象并绑定 GPIO 回调 |
| `led_set_mode(led, mode)` | 设置 LED 模式 |
| `led_set_blink_times(led, times)` | 设置闪烁次数 (BLINK_N_TIMES) |
| `multi_led_process(tick)` | 状态机处理，传入当前时间戳 (ms) |

## 应用场景

- STM32 / ESP32 等嵌入式平台
- 工业控制设备
- 医疗设备
- 仪器仪表

## 开发工具

本项目代码由 [Claude Code](https://claude.ai/claude-code) 工具辅助生成，底层大模型为 MiMo 2.5 Pro。

## License

[MIT](LICENSE)

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
│   └── multi_led.h       # 头文件
├── src/
│   └── multi_led.c       # 实现
└── examples/
    └── main.c             # 使用示例
```

## 集成方式

### 1. 复制文件

将 `inc/` 和 `src/` 目录复制到你的项目中。

### 2. 包含头文件

```c
#include "multi_led.h"
```

### 3. 实现 GPIO 回调

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

### 4. 初始化和使用

```c
// 初始化
multi_led_init();

// 创建 LED
led_t led_run;
led_create(&led_run, 0, led_write_pin);

// 设置模式
led_set_mode(&led_run, LED_MODE_HEARTBEAT);
```

### 5. 周期调用

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

## License

[MIT](LICENSE)

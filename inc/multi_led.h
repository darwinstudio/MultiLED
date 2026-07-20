/**
 * @file    multi_led.h
 * @brief   MultiLED - 轻量级 LED 状态管理框架
 * @version 1.0
 */

#ifndef __MULTI_LED_H
#define __MULTI_LED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * LED 模式定义
 */
typedef enum
{
    LED_MODE_OFF = 0,           /* 常灭              */
    LED_MODE_ON,                /* 常亮              */
    LED_MODE_BLINK_SLOW,        /* 慢闪 (1000ms)     */
    LED_MODE_BLINK_FAST,        /* 快闪 (300ms)      */
    LED_MODE_BLINK_N_TIMES,     /* 指定次数闪烁      */
    LED_MODE_HEARTBEAT,         /* 心跳灯 (亮灭各半, 1000ms) */

    LED_MODE_MAX

} led_mode_t;

/*
 * LED 对象
 */
typedef struct led led_t;

struct led
{
    uint8_t id;                                 /* LED 标识          */

    led_mode_t mode;                            /* 当前模式          */

    uint8_t state;                              /* 当前输出电平      */

    uint32_t tick;                              /* 上次更新时间戳    */

    uint32_t period;                            /* 点亮时间 (ms)     */

    uint32_t off_period;                        /* 熄灭时间 (ms)     */

    uint16_t blink_count;                       /* 已闪烁次数        */

    uint16_t blink_target;                      /* 目标闪烁次数      */

    uint8_t mode_dirty;                         /* 模式刚切换标志    */

    void (*write_pin)(uint8_t id, uint8_t level);   /* GPIO 输出回调 */

    led_t *next;                                /* 链表指针          */
};

/**
 * @brief  初始化 LED 管理器
 */
void multi_led_init(void);

/**
 * @brief  创建 LED 对象并加入链表
 * @param  led       LED 对象指针
 * @param  id        LED 标识
 * @param  write_pin GPIO 输出回调函数
 */
void led_create(led_t *led, uint8_t id, void (*write_pin)(uint8_t, uint8_t));

/**
 * @brief  设置 LED 模式
 * @param  led   LED 对象指针
 * @param  mode  目标模式
 */
void led_set_mode(led_t *led, led_mode_t mode);

/**
 * @brief  启动指定次数闪烁 (自动切换为 BLINK_N_TIMES 模式)
 * @param  led    LED 对象指针
 * @param  times  闪烁次数
 * @param  on_ms  每次点亮持续时间 (ms)
 * @param  off_ms 每次熄灭持续时间 (ms)
 * @note   无需先调用 led_set_mode，本函数内部完成模式切换
 */
void led_set_blink_times(led_t *led, uint16_t times, uint32_t on_ms, uint32_t off_ms);

/**
 * @brief  LED 状态机处理函数
 * @param  tick  当前时间戳 (ms)，由用户提供
 * @note   裸机: 在主循环中调用
 *         RTOS: 在独立任务中以 10~20ms 周期调用
 */
void multi_led_process(uint32_t tick);

/**
 * @brief  获取 LED 当前模式
 * @param  led  LED 对象指针
 * @return 当前模式
 */
led_mode_t led_get_mode(const led_t *led);

/**
 * @brief  获取 LED 当前输出电平
 * @param  led  LED 对象指针
 * @return 0=灭, 1=亮
 */
uint8_t led_get_state(const led_t *led);

#ifdef __cplusplus
}
#endif

#endif /* __MULTI_LED_H */

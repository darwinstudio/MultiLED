/**
 * @file    multi_led.c
 * @brief   MultiLED 实现
 */

#include "multi_led.h"
#include <string.h>

/*
 * 默认周期定义 (ms)
 */
#define BLINK_SLOW_PERIOD       1000
#define BLINK_FAST_PERIOD       200
#define HEARTBEAT_PERIOD        2000

/*
 * 心跳灯时序 (ms)
 * 亮100 -> 灭100 -> 亮100 -> 灭1700
 */
#define HEARTBEAT_ON1           100
#define HEARTBEAT_OFF1          100
#define HEARTBEAT_ON2           100
#define HEARTBEAT_OFF2          1700

/*
 * LED 链表头
 */
static led_t *led_head = NULL;

/*
 * 内部辅助：设置输出电平
 */
static void led_output(led_t *led, uint8_t level)
{
    if (led->write_pin != NULL)
    {
        led->write_pin(led->id, level);
    }
    led->state = level;
}

/*
 * 内部辅助：计算时间差 (处理溢出)
 */
static uint32_t tick_diff(uint32_t now, uint32_t prev)
{
    return now - prev;
}

/*
 * 内部辅助：心跳灯状态机
 *
 * 时间线:
 *   0          100   200   300        2000
 *   |--- ON ---|--- OFF ---|--- ON ---|--- OFF ---|
 */
static void heartbeat_process(led_t *led, uint32_t elapsed)
{
    uint32_t pos = elapsed % HEARTBEAT_PERIOD;

    if (pos < HEARTBEAT_ON1)
    {
        led_output(led, 1);
    }
    else if (pos < HEARTBEAT_ON1 + HEARTBEAT_OFF1)
    {
        led_output(led, 0);
    }
    else if (pos < HEARTBEAT_ON1 + HEARTBEAT_OFF1 + HEARTBEAT_ON2)
    {
        led_output(led, 1);
    }
    else
    {
        led_output(led, 0);
    }
}

/*
 * 内部：单个 LED 状态机处理
 */
static void led_process(led_t *led, uint32_t tick)
{
    uint32_t elapsed;

    switch (led->mode)
    {
        case LED_MODE_OFF:
            led_output(led, 0);
            break;

        case LED_MODE_ON:
            led_output(led, 1);
            break;

        case LED_MODE_BLINK_SLOW:
            elapsed = tick_diff(tick, led->tick);
            if (elapsed >= BLINK_SLOW_PERIOD)
            {
                led->tick = tick;
                led->state = !led->state;
                led_output(led, led->state);
            }
            break;

        case LED_MODE_BLINK_FAST:
            elapsed = tick_diff(tick, led->tick);
            if (elapsed >= BLINK_FAST_PERIOD)
            {
                led->tick = tick;
                led->state = !led->state;
                led_output(led, led->state);
            }
            break;

        case LED_MODE_BLINK_N_TIMES:
            if (led->blink_count >= led->blink_target)
            {
                /* 达到目标次数，停止闪烁并保持灭 */
                led_output(led, 0);
                break;
            }
            elapsed = tick_diff(tick, led->tick);
            if (elapsed >= led->period)
            {
                led->tick = tick;
                led->state = !led->state;
                led_output(led, led->state);
                /* 每次灭->亮算一次完整闪烁 */
                if (led->state == 1)
                {
                    led->blink_count++;
                }
            }
            break;

        case LED_MODE_HEARTBEAT:
            elapsed = tick_diff(tick, led->tick);
            heartbeat_process(led, elapsed);
            break;

        default:
            led_output(led, 0);
            break;
    }
}

/* ========== 公开 API ========== */

void multi_led_init(void)
{
    led_head = NULL;
}

void led_create(led_t *led, uint8_t id, void (*write_pin)(uint8_t, uint8_t))
{
    if (led == NULL)
    {
        return;
    }

    /* 初始化字段 */
    memset(led, 0, sizeof(led_t));
    led->id = id;
    led->write_pin = write_pin;
    led->mode = LED_MODE_OFF;
    led->state = 0;
    led->next = NULL;

    /* 追加到链表尾部 */
    if (led_head == NULL)
    {
        led_head = led;
    }
    else
    {
        led_t *p = led_head;
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = led;
    }
}

void led_set_mode(led_t *led, led_mode_t mode)
{
    if (led == NULL || mode >= LED_MODE_MAX)
    {
        return;
    }

    led->mode = mode;
    led->state = 0;
    led->tick = 0;
    led->blink_count = 0;

    /* 根据模式设置默认周期 */
    switch (mode)
    {
        case LED_MODE_BLINK_SLOW:
            led->period = BLINK_SLOW_PERIOD;
            break;
        case LED_MODE_BLINK_FAST:
            led->period = BLINK_FAST_PERIOD;
            break;
        case LED_MODE_BLINK_N_TIMES:
            /* 周期由用户或默认值决定，这里给一个默认 */
            if (led->period == 0)
            {
                led->period = BLINK_FAST_PERIOD;
            }
            break;
        default:
            led->period = 0;
            break;
    }
}

void led_set_blink_times(led_t *led, uint16_t times)
{
    if (led == NULL)
    {
        return;
    }
    led->blink_target = times;
    led->blink_count = 0;
}

void multi_led_process(uint32_t tick)
{
    led_t *p = led_head;

    while (p != NULL)
    {
        led_process(p, tick);
        p = p->next;
    }
}

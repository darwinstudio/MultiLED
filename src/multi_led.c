/**
 * @file    multi_led.c
 * @brief   MultiLED 实现
 */

#include "multi_led.h"
#include "multi_led_config.h"
#include <string.h>

/*
 * LED 链表头
 */
static led_t *led_head = NULL;

/*
 * 内部辅助：设置输出电平 (仅在电平变化时写 GPIO)
 */
static void led_output(led_t *led, uint8_t level)
{
    if (led->state == level)
    {
        return;
    }
    led->state = level;
    if (led->write_pin != NULL)
    {
        led->write_pin(led->id, level);
    }
}

/*
 * 内部辅助：计算时间差 (无符号减法天然处理 32 位溢出)
 */
static uint32_t tick_diff(uint32_t now, uint32_t prev)
{
    return now - prev;
}

/*
 * 内部：单个 LED 状态机处理
 */
static void led_process(led_t *led, uint32_t tick)
{
    uint32_t elapsed;

    /* 模式刚切换：用当前 tick 初始化计时起点 */
    if (led->mode_dirty)
    {
        led->tick = tick;
        led->mode_dirty = 0;

        /* BLINK_N_TIMES: 立即点亮第一次，本 tick 不再进入 switch */
        if (led->mode == LED_MODE_BLINK_N_TIMES && led->blink_target > 0)
        {
            led_output(led, 1);
            led->blink_count = 1;
            return;
        }
    }

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
                led_output(led, !led->state);
            }
            break;

        case LED_MODE_BLINK_FAST:
            elapsed = tick_diff(tick, led->tick);
            if (elapsed >= BLINK_FAST_PERIOD)
            {
                led->tick = tick;
                led_output(led, !led->state);
            }
            break;

        case LED_MODE_BLINK_N_TIMES:
            elapsed = tick_diff(tick, led->tick);
            if (led->state == 1)
            {
                /* 当前亮：等 period 后熄灭 */
                if (elapsed >= led->period)
                {
                    led->tick = tick;
                    led_output(led, 0);
                }
            }
            else
            {
                /* 当前灭：检查是否已完成所有闪烁 */
                if (led->blink_count >= led->blink_target)
                {
                    break;
                }
                /* 等 off_period 后点亮，计数+1 */
                if (elapsed >= led->off_period)
                {
                    led->tick = tick;
                    led_output(led, 1);
                    led->blink_count++;
                }
            }
            break;

        case LED_MODE_HEARTBEAT:
            elapsed = tick_diff(tick, led->tick);
            if (elapsed >= led->period)
            {
                led->tick = tick;
                led_output(led, !led->state);
            }
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

    LED_LOCK();

    /* 重复 ID 检查：已存在则不重复加入 */
    led_t *p = led_head;
    while (p != NULL)
    {
        if (p->id == id)
        {
            LED_UNLOCK();
            return;
        }
        p = p->next;
    }

    /* 初始化字段 */
    memset(led, 0, sizeof(led_t));
    led->id = id;
    led->write_pin = write_pin;
    led->mode = LED_MODE_OFF;
    led->state = 0;
    led->mode_dirty = 0;
    led->next = NULL;

    /* 追加到链表尾部 */
    if (led_head == NULL)
    {
        led_head = led;
    }
    else
    {
        p = led_head;
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = led;
    }

    LED_UNLOCK();
}

void led_set_mode(led_t *led, led_mode_t mode)
{
    if (led == NULL || mode >= LED_MODE_MAX)
    {
        return;
    }

    LED_LOCK();

    led->mode = mode;
    led->state = 0;
    led->blink_count = 0;
    led->blink_target = 0;
    led->mode_dirty = 1;

    /* 立即熄灭 GPIO，避免残留上一模式的电平 */
    if (led->write_pin != NULL)
    {
        led->write_pin(led->id, 0);
    }

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
            /* period/off_period 由 led_set_blink_times 设置 */
            break;
        case LED_MODE_HEARTBEAT:
            led->period = HEARTBEAT_PERIOD / 2;
            break;
        default:
            led->period = 0;
            break;
    }

    LED_UNLOCK();
}

void led_set_blink_times(led_t *led, uint16_t times, uint32_t on_ms, uint32_t off_ms)
{
    if (led == NULL)
    {
        return;
    }

    LED_LOCK();
    led->blink_target = times;
    led->blink_count = 0;
    led->period = on_ms;
    led->off_period = off_ms;
    LED_UNLOCK();
}

void multi_led_process(uint32_t tick)
{
    LED_LOCK();

    led_t *p = led_head;
    while (p != NULL)
    {
        led_process(p, tick);
        p = p->next;
    }

    LED_UNLOCK();
}

led_mode_t led_get_mode(const led_t *led)
{
    if (led == NULL)
    {
        return LED_MODE_OFF;
    }
    return led->mode;
}

uint8_t led_get_state(const led_t *led)
{
    if (led == NULL)
    {
        return 0;
    }
    return led->state;
}

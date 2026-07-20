/**
 * @file    multi_led_config.h
 * @brief   MultiLED 默认配置
 *
 * 用户可在自己项目中创建同名文件覆盖以下参数，
 * 通过 include 路径优先级使本文件不被包含。
 */

#ifndef __MULTI_LED_CONFIG_H
#define __MULTI_LED_CONFIG_H

/* 慢闪周期 (ms) */
#ifndef BLINK_SLOW_PERIOD
#define BLINK_SLOW_PERIOD       1000
#endif

/* 快闪周期 (ms) */
#ifndef BLINK_FAST_PERIOD
#define BLINK_FAST_PERIOD       300
#endif

/* 心跳灯完整周期 (ms)，亮灭各占一半，如 1000 = 亮500ms + 灭500ms */
#ifndef HEARTBEAT_PERIOD
#define HEARTBEAT_PERIOD        1000
#endif

/*
 * 临界区保护 (线程安全)
 *
 * 裸机环境默认空实现；FreeRTOS 场景请覆盖为:
 *   #define LED_LOCK()    taskENTER_CRITICAL()
 *   #define LED_UNLOCK()  taskEXIT_CRITICAL()
 */
#ifndef LED_LOCK
#define LED_LOCK()              ((void)0)
#endif

#ifndef LED_UNLOCK
#define LED_UNLOCK()            ((void)0)
#endif

#endif /* __MULTI_LED_CONFIG_H */

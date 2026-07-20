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

/* 心跳灯总周期 (ms) */
#ifndef HEARTBEAT_PERIOD
#define HEARTBEAT_PERIOD        2000
#endif

/* 心跳灯：第一次亮 (ms) */
#ifndef HEARTBEAT_ON1
#define HEARTBEAT_ON1           300
#endif

/* 心跳灯：第一次灭 (ms) */
#ifndef HEARTBEAT_OFF1
#define HEARTBEAT_OFF1          300
#endif

/* 心跳灯：第二次亮 (ms) */
#ifndef HEARTBEAT_ON2
#define HEARTBEAT_ON2           300
#endif

/* 心跳灯：第二次灭 (ms) */
#ifndef HEARTBEAT_OFF2
#define HEARTBEAT_OFF2          (HEARTBEAT_PERIOD-HEARTBEAT_ON1-HEARTBEAT_OFF1-HEARTBEAT_ON2)
#endif

#endif /* __MULTI_LED_CONFIG_H */

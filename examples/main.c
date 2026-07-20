/**
 * @file    main.c
 * @brief   MultiLED 使用示例 (STM32 + FreeRTOS)
 *
 * 硬件假设:
 *   LED_RUN   - PA1 (运行指示)
 *   LED_COMM  - PA2 (通信指示)
 *   LED_FAULT - PA3 (故障报警)
 */

#include "main.h"
#include "multi_led.h"

/*
 * LED ID 定义
 */
#define LED_RUN_ID      0
#define LED_COMM_ID     1
#define LED_FAULT_ID    2

/*
 * LED 对象
 */
static led_t led_run;
static led_t led_comm;
static led_t led_fault;

/*
 * GPIO 输出回调
 */
static void led_write_pin(uint8_t id, uint8_t level)
{
    switch (id)
    {
        case LED_RUN_ID:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (GPIO_PinState)level);
            break;
        case LED_COMM_ID:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (GPIO_PinState)level);
            break;
        case LED_FAULT_ID:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (GPIO_PinState)level);
            break;
    }
}

/*
 * FreeRTOS LED 任务
 */
static void led_task(void *argument)
{
    (void)argument;

    for (;;)
    {
        multi_led_process(xTaskGetTickCount());
        osDelay(10);
    }
}

/*
 * 初始化 LED
 */
void app_led_init(void)
{
    multi_led_init();

    led_create(&led_run,   LED_RUN_ID,   led_write_pin);
    led_create(&led_comm,  LED_COMM_ID,  led_write_pin);
    led_create(&led_fault, LED_FAULT_ID, led_write_pin);

    /* 运行指示: 心跳灯 */
    led_set_mode(&led_run, LED_MODE_HEARTBEAT);

    /* 故障指示: 默认灭，有故障时切换为快闪 */
    led_set_mode(&led_fault, LED_MODE_OFF);

    /* 创建 LED 任务 */
    osThreadNew(led_task, NULL, NULL);
}

/*
 * 示例: 通信指示 - UART 接收数据时闪烁1次 (亮100ms 灭100ms)
 */
void on_uart_rx(void)
{
    led_set_mode(&led_comm, LED_MODE_BLINK_N_TIMES);
    led_set_blink_times(&led_comm, 1, 100, 100);
}

/*
 * 示例: 故障报警
 */
void on_fault_detected(void)
{
    led_set_mode(&led_fault, LED_MODE_BLINK_FAST);
}

void on_fault_cleared(void)
{
    led_set_mode(&led_fault, LED_MODE_OFF);
}

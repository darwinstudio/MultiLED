# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

MultiLED is a lightweight LED state management framework for embedded systems, inspired by [MultiButton](https://github.com/0x1abin/MultiButton). It uses a linked list of LED objects driven by a non-blocking state machine, decoupled from hardware via function pointers.

## Repository Structure

- `inc/multi_led.h` — Public API header (led_t struct, mode enum, function declarations)
- `inc/multi_led_config.h` — Default configuration (periods, heartbeat timing, LED_LOCK/LED_UNLOCK macros), overridable via include path priority
- `src/multi_led.c` — Implementation (~220 lines, zero dynamic allocation)
- `examples/main.c` — Usage example (STM32 + FreeRTOS, 3 LEDs)

## Architecture

- Linked list of `led_t` objects, managed by `multi_led_process(tick)` called periodically (10~20ms)
- Modes: OFF, ON, BLINK_SLOW, BLINK_FAST, BLINK_N_TIMES, HEARTBEAT
- Thread safety via configurable `LED_LOCK()`/`LED_UNLOCK()` macros (default no-op, override for RTOS)
- `mode_dirty` flag ensures timing starts from the actual process tick after mode switch

## Build

No build system configured. Integrate by adding `inc/` to include path and compiling `src/multi_led.c`.

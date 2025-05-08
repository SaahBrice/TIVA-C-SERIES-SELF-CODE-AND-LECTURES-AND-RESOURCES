# SysTick Timer (System Timer) TM4C123G ARM Cortex M4 Microcontroller

In this tutorial, we will explore what the SysTick timer (System Timer) is in the TM4C123G ARM Cortex M4 microcontroller. We will start with an introduction to the SysTick timer, discuss its major applications, and conclude with an example of configuring and using the system timer using Tiva LaunchPad.

---

## SysTick Timer Introduction

The **SysTick timer** is a dedicated hardware timer integrated inside the ARM Cortex M4 CPU. It can generate interrupts at fixed intervals, making it useful for precise timing operations.

**Key points:**
- Built-in 24-bit down counter
- Can generate periodic interrupts
- Used for delays, RTOS tick timing, time-sharing, etc.

![SysTick Timer TM4C123G ARM Cortex M4 Microcontroller](image_placeholder)

The timer counts down from a preloaded value to zero, then generates an interrupt and reloads automatically.

---

## TM4C123G Microcontroller System Timer

- Provides a **24-bit** down decrement counter.
- Counts down starting from a preloaded value.
- The decrement rate depends on the system clock frequency.
- Supports continuous periodic interrupts.

---

## Applications of SysTick Timer

- Generate delays
- Time delays between events
- Periodic task invocation (e.g., RTOS tick)
- Time-sharing in real-time operating systems

---

## How does the SysTick Timer Work?

- The timer loads a value into the reload register.
- Counts down on each clock cycle.
- When it reaches zero, it triggers an interrupt.
- Counter reloads automatically to start the next cycle.

### Block Diagram

![SysTick Timer Block Diagram ARM Cortex M4](image_placeholder)

*In summary:*  
The timer counts down from a set value, triggers an interrupt when reaching zero, then reloads and repeats.

---

## Configuring the SysTick Timer

### Example: Setting up a 1-second delay with 16 MHz clock

- Clock period = 1 / 16 MHz = 62.5 ns
- Desired delay = 1 second

Calculate reload value:

```
Reload = (Delay / Clock Period) - 1
       = (1 / 62.5ns) - 1
       = 16,000,000 - 1
       = 15,999,999
```

### Steps to Configure:

1. Disable the timer (clear ENABLE bit).
2. Set the reload value.
3. Reset current value register.
4. Select clock source (system clock or internal oscillator).
5. Enable the timer and interrupt if needed.

---

## Register Details

### 1. SysTick Control and Status Register (STCTRL)

| Bit | Description |
|-------|--------------|
| ENABLE (bit 0) | Enable/disable the counter |
| TICKINT (bit 1) | Enable/disable interrupt generation |
| CLKSOURCE (bit 2) | Select clock source (system clock or internal oscillator) |
| COUNTFLAG (bit 16) | Indicates timer reached zero (write-1 to clear) |

### 2. SysTick Reload Value Register (STRELOAD)

- Holds the reload value (24-bit).
- When counter reaches zero, reloads from this value.

### 3. SysTick Current Value Register (STCURRENT)

- Indicates current countdown value.
- Reading gives current counter value.

---

## Example: Generating a 1-Second Delay Without Interrupt

```c
#include <TM4C123.h>

int main() {
    // Enable clock for GPIO port F
    SYSCTL->RCGCGPIO |= 0x20; 
    GPIOF->DIR |= 0x08;          // Set PF3 as output
    GPIOF->DEN |= 0x08;          // Enable digital function on PF3

    // Configure SysTick for 1 second delay
    SysTick->LOAD = 15999999;    // Load value for 1 second delay
    SysTick->CTRL = 0x05;        // Enable SysTick, use system clock
    SysTick->VAL = 0;            // Clear current value

    while (1) {
        // Wait until COUNTFLAG is set
        while ((SysTick->CTRL & 0x10000) == 0);
        // Toggle LED on PF3
        GPIOF->DATA ^= 0x08;
    }
}
```

---

## How the Example Works:

- Initializes GPIOF pin 3 (connected to the green LED).
- Sets up SysTick timer for 1 second based on 16 MHz clock.
- Enters an infinite loop:
  - Waits until the timer reaches zero (polls COUNTFLAG).
  - Toggles the LED state.
- Results: LED blinks every second.

---

## Summary

- The SysTick timer is a simple, hardware-based 24-bit countdown timer inside ARM Cortex M4 MCUs.
- It is widely used for creating delays, periodic interrupts, and real-time system ticks.
- Proper configuration involves setting reload and control registers.
- The timer can generate precise delays or periodic interrupts depending on application needs.

---

*Note:* Replace placeholder images with actual diagrams when creating your documentation.

---
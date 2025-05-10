# SysTick Timer Interrupt Programming for TM4C123G ARM Cortex M4

In this tutorial, we will learn how to generate a delay using the SysTick timer interrupt on the TM4C123 microcontroller. Specifically, we will toggle an LED with a 1-second interval by leveraging the SysTick interrupt service routine (ISR). 

In our previous tutorial, we discussed an in-depth introduction to the SysTick timer, how to configure its control and status registers, and how to generate delays using polling. Now, we'll implement an interrupt-based approach for better efficiency and responsiveness.

---

## Introduction to SysTick Interrupts

The SysTick timer has three key control and configuration registers, with **STCTRL** (SysTick Control and Status Register) being the primary register:

![SysTick Control and Status Register TM4C123G](image_placeholder)

### STCTRL (SysTick Control and Status Register)

| Bit | Description |
|-------|--------------|
| **ENABLE (bit 0)** | Starts/stops the counter. Setting 1 enables counting. |
| **INTEN (bit 1)** | Enables/disables interrupt request when counter reaches zero. |
| **CLKSOURCE (bit 2)** | Selects clock source: `1` for system clock, `0` for internal oscillator. |
| **COUNTFLAG (bit 16)** | Set to 1 when timer reaches zero; indicates timeout. (Write-1 clears it) |

### How it works:
- When the counter reaches zero, **COUNTFLAG** is set.
- If **INTEN** is enabled, an interrupt request is sent to the Nested Vectored Interrupt Controller (NVIC).
- The CPU transfers control to the **SysTick_Handler** ISR.

---

## Applications of SysTick Interrupts

- Creating periodic tasks (e.g., sensor readings)
- Synchronizing task execution
- Implementing RTOS tick timing
- Generating delays in a non-blocking manner

---

## How the Interrupt Works

- When the counter counts down to zero, **COUNTFLAG** is set.
- If **INTEN** is enabled, an interrupt request is sent.
- The CPU switches to **exception mode** and executes **SysTick_Handler**.
- The handler toggles the LED (or performs other tasks).
- The counter automatically reloads, preparing for the next cycle.

---

## Example: Generating 1-Second Delay Using SysTick Interrupt

### Calculating Reload Value:

Given:
- System clock = 16 MHz
- Desired delay = 1 second

Calculate reload value:

```
Reload = (Delay / Clock period) - 1
       = (1 second / (1/16MHz)) - 1
       = 16,000,000 - 1
       = 15,999,999
```

Set **SysTick->LOAD** to `15999999`. The timer counts down from this value, and upon reaching zero, triggers an interrupt every second.

---

## Complete Example Code

```c
#include "TM4C123GH6PM.h"

void SysTick_Handler(void); // Declare ISR

int main() {
    // Enable clock for GPIO Port F
    SYSCTL->RCGCGPIO |= 0x20;  // Port F
    GPIOF->DIR |= 0x08;        // PF3 as output (Green LED)
    GPIOF->DEN |= 0x08;        // Digital enable

    // Configure SysTick for 1 second delay
    SysTick->LOAD = 15999999;  // Reload value for 1s delay
    SysTick->CTRL = 0x07;      // Enable SysTick, enable interrupt, use system clock
    SysTick->VAL = 0;          // Clear current value

    while (1) {
        // Main loop does nothing; all action in ISR
    }
}

// ISR: executes every 1 second
void SysTick_Handler(void) {
    // Toggle the LED connected to PF3
    GPIOF->DATA ^= 0x08;
}
```

---

## Explanation of the Code:

- **GPIO setup**: Enables clock, sets PF3 as output, and enables digital function.
- **SysTick configuration**:
  - Loads reload value for 1 second delay.
  - Enables SysTick with `CTRL` register:
    - Bit 0 (ENABLE) = 1
    - Bit 1 (TICKINT) = 1 (enable interrupt)
    - Bit 2 (CLKSOURCE) = 1 (system clock)
- **Interrupt Handler**:
  - Toggles the LED each time the timer reaches zero (every second).

---

## Summary

- The SysTick timer can generate periodic interrupts, enabling efficient time-based task execution.
- Proper configuration of **LOAD**, **CTRL**, and **VAL** registers is essential.
- Using interrupts allows the main program to perform other tasks or remain idle.
- The example demonstrates toggling an LED every second using the interrupt method.

---


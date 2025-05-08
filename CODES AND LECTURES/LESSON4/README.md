# Accessing Memory Mapped Peripherals Registers of Microcontrollers

In this tutorial, you will learn how to access memory-mapped peripheral registers of microcontrollers through their memory addresses. We will explore applications of pointers in embedded systems programming and understand their role in embedded software architecture development. Additionally, we will demonstrate how to use pointers to read and write memory-mapped peripheral registers with two methods.

---

## Memory Mapping in Embedded Processors and Microcontrollers

Microcontrollers and microprocessors have two types of memory regions:

- **Memory Mapped Region**
- **Non-Memory Mapped Region**

---

### Non-Memory Mapped Region

This region includes internal general-purpose and special function registers of the CPU. These registers do **not** have addresses. They are accessed using register names in assembly language or via inline assembly in C.

For example, an ARM Cortex M4 32-bit microcontroller has 13 general-purpose registers and nine special function registers. These are accessed by their names, not addresses.

---

### Memory Mapped Region of Microcontrollers

The addressable memory space depends on the processor's address bus width. For instance, the **ARM Cortex M4 32-bit** microcontroller has a **4 GB** address space (`2^32` bytes). Each byte has a unique address, and the microcontroller can read/write data from/to each location.

**Memory-mapped peripheral registers** are special registers mapped to specific addresses. They control and interact with peripherals like GPIOs, timers, UARTs, ADCs, etc., via standard memory operations.

---

## Key Concepts

### Memory Mapping

- Regions of the memory address space are reserved for peripherals.
- Each peripheral’s registers are mapped to specific addresses.
- Accessing these addresses interacts directly with the peripheral.

### Registers

- Peripherals have multiple registers for control, status, and data.
- These allow configuration and data exchange with peripherals.

### Access Methods

- Using pointers in programming languages like C.
- For example, a GPIO register at address `0x40021000` can be accessed via a pointer.

---

## Memory-Mapped Peripheral Registers

Microcontrollers also have memory-mapped I/O regions for peripherals such as:

- GPIO
- ADC
- UART
- SPI
- I2C
- Timers

### Example: TM4C123GH6PM ARM Cortex M4 Memory Map

This microcontroller's memory map includes:

- Flash memory
- SRAM
- Peripheral registers
- External memory (e.g., SD cards)

**Total addressable space:** 4 GB

---

## ARM Cortex M4 Memory Regions

| Region | Address Range | Description |
|---------|-----------------|--------------|
| Program/Code | `0x0000_0000` to `0x1FFF_FFFF` | Program and static data |
| SRAM | `0x2000_0000` to `0x3FFF_FFFF` | Temporary data, stack, heap |
| Peripheral Registers | `0x4000_0000` to `0x5FFF_FFFF` | GPIO, UART, Timers, etc. |
| External RAM | `0x6000_0000` to `0x9FFF_FFFF` | External memory devices |
| System Memory | Last 0.5 GB | NVIC, system timers, SCB |

*Note:* Our focus is on the peripheral memory regions.

---

## Peripheral Memory Region Example: TM4C123G Microcontroller

This memory map includes registers for peripherals like GPIO ports, timers, UART, SPI, etc.

### GPIO Ports

- Each GPIO port has **4 KB** of memory.
- Contains **36 registers** per port, supporting bit-banding.

### Example: GPIO Port Registers

| Register | Offset Address | Description |
|------------|------------------|--------------|
| GPIODATA | `0x000` | Data register |
| GPIODIR | `0x400` | Direction register |
| GPIOIS | `0x404` | Interrupt sense |
| ... | ... | ... |

**Calculating Physical Address:**

For example, to find the address of the `GPIODATA` register for PORTA:

```
Physical Address = Base Address + Offset
```

Suppose:

- `Base Address` of PORTA = `0x40004000`
- Offset for DATA = `0x000`

Then:

```c
Physical Address = 0x40004000 + 0x000 = 0x40004000
```

---

## How to Access Peripheral Registers

Peripheral registers are part of RAM memory and can be accessed similarly to variables:

- Reading a value from a register
- Writing data to a register

---

## Using Pointers to Access Peripheral Registers

### Method 1: Using Pointer Variables

```c
unsigned int *GPIO_PORTF_DIR_R = (unsigned int*)0x40025400;
*GPIO_PORTF_DIR_R = 0xF0; // Set first 4 pins as input, last 4 as output
```

**Drawback:** Uses extra memory for the pointer variable, which is limited in microcontrollers.

---

### Method 2: Direct Dereferencing

```c
// Write value
(*(volatile unsigned int *)0x40025400) = 0xF0;

// Read value
unsigned int data = (*(volatile unsigned int *)0x40025400);
```

**Advantages:**

- No extra memory used
- Faster execution

**Disadvantage:**

- Harder to read and maintain

### Improving Readability with `#define`

```c
#define GPIO_PORTF_DIR_R (*(volatile unsigned int *)0x40025400)
```

Now, you can simply:

```c
GPIO_PORTF_DIR_R = 0xF0;
unsigned int data = GPIO_PORTF_DIR_R;
```

---

## Practical Example: Blinking an LED Using GPIO

```c
#define GPIO_BASE_ADDR   0x40021000

#define GPIO_DIR_REG     (*(volatile uint32_t *)(GPIO_BASE_ADDR + 0x00))
#define GPIO_OUT_REG     (*(volatile uint32_t *)(GPIO_BASE_ADDR + 0x04))
#define GPIO_IN_REG      (*(volatile uint32_t *)(GPIO_BASE_ADDR + 0x08))

void configure_gpio_as_output() {
    GPIO_DIR_REG |= (1 << 0);  // Set pin 0 as output
}

void set_gpio_pin_high() {
    GPIO_OUT_REG |= (1 << 0);  // Set pin 0 high
}

void set_gpio_pin_low() {
    GPIO_OUT_REG &= ~(1 << 0); // Set pin 0 low
}

uint32_t read_gpio_pin() {
    return (GPIO_IN_REG & (1 << 0)); // Read pin 0
}
```

---

## Conclusion

Memory-mapped peripheral registers are fundamental in microcontroller programming, enabling direct and efficient control of hardware peripherals through standard memory operations. Understanding how to access and manipulate these registers is essential for embedded system development.

---
## **Using GPIO on TM4C123G LaunchPad**

This tutorial will guide you through using the GPIO (General Purpose Input/Output) pins on the **Tiva C Series TM4C123GH6PM** microcontroller to drive simple output like LEDs. We’ll break down how the microcontroller actually sets up and controls a pin so you can use it however you want—whether to blink an LED or send digital control signals.

### Why GPIO?

GPIO pins are **basic communication points** between the microcontroller and the outside world. You can configure them to **output** signals (to turn on a light or motor), or to **input** signals (like detecting if a switch is pressed). Here, we focus on **output**, specifically blinking the onboard LEDs.

---

## **GPIO Ports Overview**

The TM4C123GH6PM has **6 GPIO ports** labeled A through F. Each of these has 8 pins (from 0 to 7), though not all are fully available due to overlapping alternate functions.

| Port | Pin Range |
|------|-----------|
| A    | PA0–PA7   |
| B    | PB0–PB7   |
| C    | PC0–PC7   |
| D    | PD0–PD7   |
| E    | PE0–PE5   |
| F    | PF0–PF7   |

Each port sits at a specific **memory-mapped address range**. This means the CPU accesses hardware registers by simply reading and writing to specific memory locations.

---

## **Step-by-Step: Making a Pin Work**

To make a GPIO pin output a signal, you need to do **several steps in sequence**. This might seem like a lot at first, but it’s straightforward once you understand what each step is doing.

Let’s go step by step using **Port F** as an example (which has 3 onboard LEDs: Red = PF1, Blue = PF2, Green = PF3).

---

### **1. Enable Clock for the GPIO Port**

Before using any GPIO port, you must **give it power** by enabling its clock through the **RCGCGPIO register**.

- **Why?** Like any other digital circuit, unused peripherals are turned off by default to save power.
- **How?** Each bit in `RCGCGPIO` corresponds to one port (bit 0 = Port A, ..., bit 5 = Port F).

```c
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long *)0x400FE608))
SYSCTL_RCGCGPIO_R |= 0x20; // Bit 5 = Port F
```

This tells the microcontroller: *“Hey, I want to use Port F. Turn on its internal clock.”*

---

### **2. Set the Direction of Each Pin**

Next, you need to tell the microcontroller **how each pin should behave**—as an input (read signals) or output (send signals). This is done using the **GPIODIR register**.

- **Why?** A microcontroller pin is like a switchable wire. It can either:
  - Watch for voltage coming in (input)
  - Drive voltage out (output)

If we want to blink LEDs, we need to **send signals out**, so we configure the pins as **outputs**.

- **Register**: `GPIODIR`
- **Address for Port F**: `0x40025400`

```c
#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))
GPIO_PORTF_DIR_R = 0x0E; // Set bits 1, 2, 3 (PF1, PF2, PF3) as output
```

- Bit values:
  - `PF1` = 0x02 (Red LED)
  - `PF2` = 0x04 (Blue LED)
  - `PF3` = 0x08 (Green LED)
  - So 0x02 + 0x04 + 0x08 = 0x0E (enable all three as output)

---

### **3. Enable Digital Function on the Pins**

Even after setting direction, the pins won’t work unless you **enable the digital circuitry** on them. This is done via the **GPIODEN** register (Digital Enable).

- **Why?** GPIO pins can be used in **analog or digital** mode. By default, neither is on. If you want to send/receive logic-level signals (like 0V or 3.3V), you must **enable digital mode**.
- **Register**: `GPIODEN`
- **Address for Port F**: `0x4002551C`

```c
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))
GPIO_PORTF_DEN_R = 0x0E; // Enable digital function on PF1, PF2, PF3
```

This effectively tells the chip: *“I want these pins to work as normal digital GPIOs.”*

---

## **Next Step: Writing to the Pins (GPIODATA)**

At this point, our pins are:

- **Powered (via clock)**
- **Set as output**
- **Enabled for digital logic**

Now we want to actually **turn the LED on or off** — this means writing a `1` or `0` to a **data register**. This is done through `GPIODATA`, but here's where things get a bit tricky, because of **bit-specific addressing**.


## **4. Controlling the Output: `GPIODATA`**

Now that the pins are set up, we want to **turn them ON or OFF**. That means writing logic high (1) or low (0) to the corresponding pin bits using the `GPIODATA` register.

But here’s the catch: unlike other registers that live at one single address, `GPIODATA` lives in a **range of addresses** — because of how the TM4C123 uses something called **bit-specific addressing**.

---

### 🧠 What is Bit-Specific Addressing?

Normally, when you write to a GPIO port, you’d risk changing all 8 pins at once, even if you only meant to affect one. Bit-specific addressing solves this by giving you a **special address for each bit-mask**. That way, you can write to just PF1 (red LED), or PF3 (green LED), **without disturbing the others**.

### 📘 Official Formula (From Datasheet):

The address for `GPIODATA` is:
```
GPIODATA(masked) = Base + (mask << 2)
```

- **Base**: 0x40025000 (for Port F)
- **mask**: Bit pattern for the pins you want to access (e.g., PF1 = 0x02)

So, to write to PF1 (Red LED):
```
0x40025000 + (0x02 << 2) = 0x40025008
```

To write to PF1, PF2, and PF3 (Red, Blue, Green):
```
mask = 0x0E → (0x0E << 2 = 0x38)
→ Address = 0x40025000 + 0x38 = 0x40025038
```

---

### ✅ Writing a Value to LEDs:

Here’s how we turn ON or OFF the LEDs:

```c
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))
```

This 0x400253FC is a special address that accesses **all 8 bits** of Port F at once (full register access). But again, if you want to be safe and only touch PF1-PF3, use:

```c
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x40025038))
```

To turn on the **red LED (PF1)**:
```c
GPIO_PORTF_DATA_R = 0x02; // Only PF1 is high
```

To turn on **green LED (PF3)**:
```c
GPIO_PORTF_DATA_R = 0x08;
```

To turn **all LEDs ON**:
```c
GPIO_PORTF_DATA_R = 0x0E;
```

To turn **all LEDs OFF**:
```c
GPIO_PORTF_DATA_R = 0x00;
```

And to **toggle** one, you'd use XOR:
```c
GPIO_PORTF_DATA_R ^= 0x02; // Toggles red LED
```

---

## ✅ Recap of Full Setup Code:

Putting it all together, here’s the minimal full code snippet to configure and control the onboard LEDs (Red, Blue, Green):

```c
#define SYSCTL_RCGCGPIO_R     (*((volatile unsigned long *)0x400FE608))
#define GPIO_PORTF_DIR_R      (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R      (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_DATA_R     (*((volatile unsigned long *)0x40025038)) // For PF1–3

void PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;        // 1. Enable clock to Port F
    GPIO_PORTF_DIR_R   |= 0x0E;       // 2. Set PF1–3 as output
    GPIO_PORTF_DEN_R   |= 0x0E;       // 3. Enable digital function on PF1–3
}

void BlinkLEDs(void) {
    GPIO_PORTF_DATA_R = 0x0E;         // All LEDs ON
    for (int i = 0; i < 1000000; i++); // crude delay
    GPIO_PORTF_DATA_R = 0x00;         // All LEDs OFF
    for (int i = 0; i < 1000000; i++); // crude delay
}
```


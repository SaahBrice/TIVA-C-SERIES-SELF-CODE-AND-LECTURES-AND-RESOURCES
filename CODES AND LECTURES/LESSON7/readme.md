# What is the Interrupt Vector Table (IVT)?

In this tutorial, we will explore the concept of the **Interrupt Vector Table (IVT)**—what it is, its role in handling interrupts and exceptions, and how it is implemented in the TM4C123G ARM Cortex M4 microcontroller.

---

## Definition of the Interrupt Vector Table (IVT)

The **IVT** is a table that holds the **memory addresses** (also called *vectors*) of all **Interrupt Service Routines (ISRs)** and **exception handlers** supported by a microcontroller or microprocessor.

**In simple terms:**  
It’s like a directory that tells the CPU where to find the code to handle various hardware or software events.

### Why is IVT important?
When an interrupt or exception occurs, the processor needs to quickly jump to the correct handler routine. The IVT provides the **starting addresses** of these routines, enabling fast, event-driven responses instead of polling (checking status repeatedly).

---

## What are Vectors?

In embedded systems, a **vector** refers to a **memory address** where an ISR is located.  
So, the **IVT** is a table of these addresses, each corresponding to a specific interrupt or exception.

---

## Role of the IVT in Interrupts and Exceptions

- The **ARM Cortex-M CPU** operates mainly in **thread mode**.
- When an **interrupt** or **exception** occurs, the CPU **transfers** from thread mode to **exception mode**.
- The **Nested Vectored Interrupt Controller (NVIC)** receives the interrupt request.
- NVIC uses the **exception number** (or interrupt number) to **look up** the address of the corresponding handler in the **IVT**.
- The CPU **loads** this address into the **Program Counter (PC)**.
- The handler routine executes, responding to the event.

This process allows the system to respond **immediately** to hardware signals or errors with minimal delay.

---

## The IVT in TM4C123G ARM Cortex M4 Microcontroller

- The **IVT** contains **154 entries**:
  - **15 system exceptions** (like Reset, NMI, HardFault, SysTick, etc.)
  - **138 peripheral interrupts**
  - Some entries are **reserved** for future use or unavailable peripherals.
  
- The **IVT** is stored in the **flash (program memory)**, starting at address **0x0000_0000**.

- The **first two entries** are:
  - **Initial Main Stack Pointer (MSP) value**  
  - **Reset handler address**

- Each subsequent entry contains the **address of an ISR** (pointer to the handler function).

### Example:
- At address `0x0000_003C`, the **SysTick_Handler** address is stored, so when a SysTick interrupt occurs, the CPU jumps to that routine.

---

## Where is IVT Stored?

- In **flash memory** (program memory) at fixed starting addresses.
- The **startup file** and **linker script** define the **layout** of the vector table.
- The **first 154 words** (for TM4C123G) are dedicated to the **IVT**.

---

## How does the Processor Use the IVT?

1. An **interrupt** occurs.
2. The **NVIC** accepts and processes the request.
3. The **exception number** is used to **index** into the **IVT**.
4. The **address** stored at that index is loaded into the **Program Counter (PC)**.
5. The CPU **executes** the ISR routine located at that address.

---

## Customizing the Vector Table

- The **vector table** can often be **redefined** or **relocated** by the developer.
- In CMSIS-based systems, the **startup file** defines the default vector table.
- For **real-time** or **specialized applications**, you might **modify** the table to point to custom handlers.

---

## Summary

- The **IVT** is a **table of function pointers** (addresses) for all **interrupts** and **exceptions**.
- It is stored in **flash memory** at a fixed location, typically at the start of program memory.
- When an event occurs, the processor **looks up** the handler address in the IVT and **jumps** to execute it.
- Proper configuration of the **IVT** ensures the system can **respond quickly** and **efficiently** to hardware or software events.

---

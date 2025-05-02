# **📝 GPIO Digital Input & Output: Controlling an LED with Push Buttons on TM4C123G LaunchPad**  

## **🔍 Introduction**  
Welcome to this comprehensive guide on using GPIO pins for digital input and output on the TM4C123G LaunchPad. If you're just starting with embedded systems, this tutorial will walk you through every concept carefully, ensuring you understand not just *how* things work, but *why* they work that way.  

We'll learn how to:  
- **Read a push button** (digital input)  
- **Control an LED** (digital output)  
- **Use internal pull-up resistors** to stabilize input signals  
- **Understand switch bouncing** and its effects  

By the end, you'll have a fully functional program that turns an LED on when a button is pressed and off when released.  

---

## **📌 Hardware Overview**  

### **1. The TM4C123G LaunchPad**  
The TM4C123G LaunchPad is a development board featuring an ARM Cortex-M4 microcontroller. It includes:  
- **Three onboard LEDs** (Red, Blue, Green)  
- **Two onboard push buttons** (Switch 1 and Switch 2)  

### **2. Pin Connections**  
| **Component** | **Microcontroller Pin** | **Behavior** |  
|--------------|------------------------|-------------|  
| **Red LED**  | `PF1` (Port F, Pin 1)  | **Active-High** (LED turns ON when `PF1 = 1`) |  
| **Switch 2** | `PF4` (Port F, Pin 4)  | **Active-Low** (Pressing the button connects `PF4` to ground, making it `0`) |  

⚠️ **Important Note:**  
- The switches **do not have external pull-up resistors**, meaning if we don’t enable an internal pull-up, the input pin will be **floating** (unstable) when the button is not pressed.  
- A **floating pin** can randomly read `0` or `1`, leading to unreliable behavior.  

---

## **⚙️ Key Concepts**  

### **1️⃣ Pull-Up Resistors & Why We Need Them**  
When a button is **not pressed**, the input pin (`PF4`) is **not connected to anything**—it’s "floating." This means:  
- The microcontroller might read random noise (`0` or `1` unpredictably).  
- To fix this, we use a **pull-up resistor**, which weakly pulls the pin to `1` (HIGH) when the button is open.  

🔹 **How It Works:**  
- **Button released:** `PF4` is pulled to `1` (via the internal pull-up).  
- **Button pressed:** `PF4` connects to ground (`0`), overriding the pull-up.  

🔹 **Enabling the Pull-Up:**  
The TM4C123G has **internal pull-up resistors** that we can enable in software (using the `GPIOPUR` register).  

---

### **2️⃣ Switch Bouncing (And Why It Matters)**  
Mechanical switches don’t make perfect contact instantly—they **bounce** (rapidly open and close) for a few milliseconds before stabilizing.  

🔹 **What Happens Without Debouncing?**  
- A single button press might be **detected multiple times** (e.g., LED flickers instead of cleanly turning on).  
- This happens because the bouncing causes the input pin to fluctuate between `0` and `1` rapidly.  

🔹 **Solutions:**  
1. **Hardware Debouncing:** Adding a small RC (resistor-capacitor) circuit to filter out the noise.  
2. **Software Debouncing:** Adding a small delay (e.g., 20ms) after detecting a button press to ignore the bouncing period.  

*(For simplicity, this tutorial does **not** implement debouncing, but you should be aware of the issue!)*  

---

## **🛠️ Step-by-Step Code Explanation**  

### **1️⃣ Enable Clock for GPIO Port F**  
```c
SYSCTL->RCGCGPIO |= 0x20;  // Enable clock for Port F
```
🔹 **Why?**  
- The microcontroller **disables clocks to unused peripherals** to save power.  
- Before using any GPIO port, we must **enable its clock**.  
- `0x20` in binary is `00100000`, meaning **bit 5** (Port F) is set to `1`.  

*(Reference: Page 340 of the TM4C123G datasheet.)*  

---

### **2️⃣ Unlock PF4 for Configuration (Special Case for PF0 & PF4)**  
```c
GPIOF->LOCK = 0x4C4F434B;  // Unlock the GPIO Commit register  
GPIOF->CR = 0x01;          // Allow changes to PF4  
```
🔹 **Why?**  
- Pins `PF0` and `PF4` are **locked by default** (to prevent accidental changes).  
- We must **unlock them first** before modifying their settings.  
- `0x4C4F434B` is the **magic unlock value** (like a password).  
- `GPIOF->CR = 0x01` allows modifying `PF4`.  

*(This step is **not needed for most other pins**.)*  

---

### **3️⃣ Enable Pull-Up Resistor on PF4 (Switch Input)**  
```c
GPIOF->PUR |= 0x10;  // Enable pull-up on PF4
```
🔹 **Why?**  
- Ensures `PF4` reads `1` when the button is **not pressed**.  
- `0x10` in binary is `00010000`, meaning **bit 4** (`PF4`) is set.  

---

### **4️⃣ Set Pin Directions (Input vs. Output)**  
```c
GPIOF->DIR |= 0x02;  // PF1 = output (LED), PF4 = input (button)
```
🔹 **How It Works:**  
- `GPIODIR` (Direction Register) controls whether a pin is an **input (`0`)** or **output (`1`)**.  
- `0x02` = `00000010` → **PF1 (LED) = output**, **PF4 (button) = input**.  

---

### **5️⃣ Enable Digital Functionality on Pins**  
```c
GPIOF->DEN |= 0x12;  // Enable digital I/O on PF1 and PF4
```
🔹 **Why?**  
- GPIO pins can also be used for **analog functions** (e.g., ADC).  
- `GPIODEN` (Digital Enable Register) **must be set** for digital use.  
- `0x12` = `00010010` → enables **PF1 (LED) and PF4 (button)**.  

---

### **6️⃣ Read Button & Control LED (Main Loop)**  
```c
while(1) {
    state = GPIOF->DATA & 0x10;      // Read PF4 (mask bit 4)
    GPIOF->DATA = (~state >> 3);     // Drive PF1 based on button
}
```
🔹 **Breaking It Down:**  
1. **`state = GPIOF->DATA & 0x10`**  
   - Reads `PF4` (bit 4) and masks all other bits.  
   - If button is **pressed**, `state = 0x00`.  
   - If button is **released**, `state = 0x10`.  

2. **`(~state >> 3)`**  
   - **`~state`**: Inverts `state` (so pressed = `0xEF`, released = `0xDF`).  
   - **`>> 3`**: Shifts bit 4 to bit 1 (for `PF1`).  
   - **Final result**:  
     - Button pressed → `PF1 = 1` (LED ON).  
     - Button released → `PF1 = 0` (LED OFF).  

*(This works because the button is **active-low** and the LED is **active-high**.)*  

---

## **✅ Complete Code**  
```c
#include "TM4C123GH6PM.h"

int main(void) {
    unsigned int state;
    
    // 1. Enable clock for Port F
    SYSCTL->RCGCGPIO |= 0x20;
    
    // 2. Unlock PF4 (needed only for PF0 & PF4)
    GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR = 0x01;
    
    // 3. Enable pull-up on PF4 (button)
    GPIOF->PUR |= 0x10;
    
    // 4. Set PF1 as output, PF4 as input
    GPIOF->DIR |= 0x02;
    
    // 5. Enable digital I/O on PF1 and PF4
    GPIOF->DEN |= 0x12;
    
    // 6. Main loop: Read button, control LED
    while(1) {
        state = GPIOF->DATA & 0x10;
        GPIOF->DATA = (~state >> 3);
    }
}
```

---

## **🔧 Testing & Troubleshooting**  
- **Expected Behavior:**  
  - Pressing Switch 2 (`PF4`) turns the **Red LED (`PF1`)** ON.  
  - Releasing it turns the LED OFF.  

- **Common Issues:**  
  - **LED doesn’t respond:**  
    - Check if `PF1` and `PF4` are correctly configured.  
    - Verify the pull-up resistor is enabled (`GPIOPUR`).  
  - **LED flickers when pressing the button:**  
    - This is **switch bouncing**—you’ll need debouncing (next tutorial!).  

---

## **📌 Summary & Next Steps**  
✅ **Learned:**  
- How to configure **GPIO pins** for input (button) and output (LED).  
- Why **pull-up resistors** are needed for stable input.  
- The basics of **switch bouncing** and its impact.  

➡️ **Next Steps:**  
- **Debouncing techniques** (software delay vs. hardware filtering).  
- **Interrupt-driven button handling** (no polling!).  

Happy coding! 🚀
# **📝 Toggling an LED with a Push Button on TM4C123G LaunchPad**  

## **🔍 Introduction**  
In this tutorial, we will learn how to **toggle an LED** using a push button on the **TM4C123G LaunchPad**. Unlike the previous example (where the LED turns ON only while the button is pressed), this time we want the LED to **change state (ON/OFF) each time the button is pressed**.  

### **🎯 Key Learning Objectives**  
✔ **Edge Detection** – Detecting a button press (not just holding it).  
✔ **Debouncing** – Preventing false triggers due to switch bouncing.  
✔ **Bitwise Toggling** – Using `XOR` (`^`) to flip an LED’s state.  

---

## **📌 Hardware Setup**  
The TM4C123G LaunchPad has:  
- **Two onboard push buttons:**  
  - **SW1 (PF4)** – Used in this example.  
  - **SW2 (PF0)** – (Locked by default; requires unlocking).  
- **Three onboard LEDs:**  
  - **Red (PF1), Blue (PF2), Green (PF3)** – We’ll use **Green (PF3)**.  

### **🔧 Pin Connections**  
| **Component** | **Microcontroller Pin** | **Behavior** |  
|--------------|------------------------|-------------|  
| **Green LED** | `PF3` | **Active-High** (ON when `PF3 = 1`) |  
| **SW1** | `PF4` | **Active-Low** (Pressed = `0`, Released = `1`) |  

⚠️ **Important Notes:**  
- **No external pull-up resistor** → Must enable **internal pull-up** on `PF4`.  
- **Switch bouncing** can cause multiple false triggers → We’ll implement **software debouncing**.  

---

## **⚙️ Core Concepts**  

### **1️⃣ Edge Detection vs. Level Detection**  
- **Level Detection:** Continuously checks if the button is pressed (as in the previous tutorial).  
- **Edge Detection:** Detects **a change** (e.g., from `1` to `0`), useful for toggling.  

🔹 **Why Edge Detection?**  
- If we used **level detection**, the LED would flicker rapidly while the button is held.  
- **Edge detection** ensures **one toggle per press**.  

### **2️⃣ Debouncing: Why It’s Necessary**  
Mechanical switches **bounce** (rapidly oscillate between `0` and `1`) for a few milliseconds when pressed/released.  

🔹 **Without Debouncing:**  
- A single press might be detected **multiple times** → LED toggles erratically.  

🔹 **Solution:**  
- **Add a delay (~20ms)** after detecting a press to ignore bouncing.  

### **3️⃣ Toggling with XOR (`^`)**  
The `XOR` operation flips a bit:  
- `1 ^ 1 = 0`  
- `0 ^ 1 = 1`  

🔹 **Example:**  
```c
GPIO_PORTF_DATA_R ^= 0x08;  // Toggles PF3 (Green LED)
```  

---

## **🛠️ Step-by-Step Code Breakdown**  

### **1️⃣ Register Definitions (Memory-Mapped I/O)**  
We define macros for register addresses to avoid hardcoding:  
```c
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long *)0x400FE608))  
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))  
#define GPIO_PORTF_DIR_R  (*((volatile unsigned long *)0x40025400))  
#define GPIO_PORTF_DEN_R  (*((volatile unsigned long *)0x4002551C))  
#define GPIO_PORTF_PUR_R  (*((volatile unsigned long *)0x40025510))  
```
🔹 **Why?**  
- These addresses are from the **TM4C123G datasheet**.  
- `volatile` tells the compiler **not to optimize** these reads/writes.  

---

### **2️⃣ Clock & GPIO Initialization**  
```c
SYSCTL_RCGCGPIO_R |= 0x20;  // Enable Port F clock
GPIO_PORTF_DEN_R |= 0x18;   // Enable PF3 (LED) & PF4 (Button)
GPIO_PORTF_DIR_R |= 0x08;   // PF3 = Output, PF4 = Input
GPIO_PORTF_PUR_R |= 0x10;   // Enable pull-up on PF4
```
🔹 **What’s Happening?**  
1. **Enable Clock:** Port F needs a clock signal to work.  
2. **Digital Enable (`DEN`):** PF3 (LED) and PF4 (Button) are set as digital pins.  
3. **Direction (`DIR`):** PF3 = Output, PF4 = Input.  
4. **Pull-Up (`PUR`):** Ensures `PF4` reads `1` when the button is released.  

---

### **3️⃣ Debouncing Delay Function**  
```c
void Delay(unsigned long counter) {
    while(counter--);  // Simple delay loop
}
```
🔹 **Why?**  
- Adds a small delay (~20ms) to **ignore switch bouncing**.  
- `SYSTEM_CLOC_FREQUENCY` (16MHz) helps calculate the delay.  

---

### **4️⃣ Main Loop (Edge Detection + Toggle Logic)**  
```c
while(1) {
    if((GPIO_PORTF_DATA_R & 0x10) == 0) {  // If button pressed (PF4 = 0)
        Delay(DELAY_DEBOUNCE);             // Wait for debounce
        if((GPIO_PORTF_DATA_R & 0x10) == 0) {  // Check again
            GPIO_PORTF_DATA_R ^= 0x08;      // Toggle LED (PF3)
            while((GPIO_PORTF_DATA_R & 0x10) == 0); // Wait for release
        }
    }
}
```
🔹 **How It Works:**  
1. **Check Button Press:** `PF4 = 0` means pressed.  
2. **Debounce Delay:** Waits ~20ms to avoid false triggers.  
3. **Confirm Press:** Checks again after the delay.  
4. **Toggle LED:** Flips PF3’s state using `XOR`.  
5. **Wait for Release:** Prevents multiple toggles while holding the button.  

---

## **✅ Full Code**  
```c
#include <stdint.h>

#define SYSCTL_RCGCGPIO_R (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTF_DATA_R (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R  (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R  (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_PUR_R  (*((volatile uint32_t *)0x40025510))

#define DELAY_MS 20
#define SYSTEM_CLOCK_FREQ 16000000
#define DEBOUNCE_DELAY (SYSTEM_CLOCK_FREQ / 1000 * DELAY_MS)

void Delay(uint32_t count) {
    while(count--);
}

int main() {
    // 1. Enable Port F Clock
    SYSCTL_RCGCGPIO_R |= 0x20;
    
    // 2. Configure PF3 (LED) & PF4 (Button)
    GPIO_PORTF_DEN_R |= 0x18;   // Digital Enable PF3 & PF4
    GPIO_PORTF_DIR_R |= 0x08;   // PF3 = Output, PF4 = Input
    GPIO_PORTF_PUR_R |= 0x10;   // Pull-Up on PF4
    
    while(1) {
        if((GPIO_PORTF_DATA_R & 0x10) == 0) {  // Button Pressed?
            Delay(DEBOUNCE_DELAY);             // Debounce
            if((GPIO_PORTF_DATA_R & 0x10) == 0) {  // Confirm Press
                GPIO_PORTF_DATA_R ^= 0x08;     // Toggle LED
                while((GPIO_PORTF_DATA_R & 0x10) == 0); // Wait for release
            }
        }
    }
}
```

---

## **🔧 Testing & Troubleshooting**  
✅ **Expected Behavior:**  
- Press SW1 → Green LED toggles (ON/OFF).  
- Each press should **only toggle once**.  

⚠️ **Common Issues:**  
- **LED doesn’t toggle:**  
  - Check if `PF3` and `PF4` are correctly configured.  
  - Verify the pull-up resistor is enabled (`PUR`).  
- **LED toggles multiple times per press:**  
  - Increase the debounce delay (`DEBOUNCE_DELAY`).  

---

## **📌 Summary & Next Steps**  
✔ Learned **edge detection** for button presses.  
✔ Implemented **software debouncing** to prevent false triggers.  
✔ Used **bitwise XOR (`^`)** to toggle an LED.  

➡️ **Next Steps:**  
- **Interrupts:** Replace polling with button-triggered interrupts.  
- **Multiple Buttons:** Control different LEDs with SW1 and SW2.  

Happy coding! 🚀
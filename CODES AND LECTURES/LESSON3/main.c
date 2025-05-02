



#include "TM4C123GH6PM.h"

#define SYSTEM_CLOCK_FREQUENCY 16000000

#define DELAY_DEBOUNCE				SYSTEM_CLOCK_FREQUENCY/1000

void Delay(unsigned long counter){

	unsigned long i=0;
	for (i=0;i<counter;i++){};
}

int main(void) {
    unsigned int state;
    static char flag = 0;
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
				
				if (state == 0){
					Delay(DELAY_DEBOUNCE);
					
					if((flag ==0) && (state == 0)){
						GPIOF->DATA ^= (~state >> 3);
						flag = 1;
					}
				
				}
				else{
						flag=0;
				}
			
        
    }
}
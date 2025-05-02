#include <TM4C123GH6PM.h>

int main(void) {
	
	unsigned int state;
	
	SYSCTL ->RCGCGPIO |= 0x20;  //Enable clock to GPIOF
	GPIOF -> LOCK = 0x4C4F434B; //unlock GPIOCR register
	GPIOF -> CR = 0x01;					//Enable GPIOPUR register enable to commit
	GPIOF -> PUR |=0x10; 				//Enable pull up resistor PF4
	GPIOF -> DIR |= 0x02;				//sET pf1 AS AN OUTPUT AND pf4 AS AN INPUT PIN
	GPIOF -> DEN |=	0x12;				//Enable PF1 and PF4 as digital GPIO pins
	
	while(1) {
		
		state = GPIOF -> DATA & 0x10;
		GPIOF -> DATA =  (~state >>3); //put it on red LED
		
	}
	
	
}
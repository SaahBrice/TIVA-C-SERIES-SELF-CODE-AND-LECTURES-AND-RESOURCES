

/*************************************************************
This code demonstrates how to blink an LED connected to pin PF3 
(assumed to be the green LED on the microcontroller board) 
using the SysTick timer for generating a delay.
*************************************************************/



#include <TM4C123.h>

int main(){

	SYSCTL -> RCGCGPIO |= 0x20; // TURN OFF BUS CLICK FOR GPIOF
	GPIOF -> DIR |=0x08; //Set Green Pin as a digital output
	GPIOF -> DEN |=0x08; //Enable PF3 pin as a digital pin
	
	
	SysTick -> LOAD = 15999999; //one second delay reload value
	SysTick -> CTRL = 5; // Enable counter and select the sstem bus clock
	SysTick -> VAL =0; // Initialize current value register
	
	while (1){
	
		if(SysTick -> CTRL & 0X10000){ //this is a bit wise and between the the (*SysTick).CTRL and the 0X10000 (1 0000 0000 0000 0000 in binary) so this condition is true only if the but wise and is 1 for the 16th bit considering you start counting from 0
		
			GPIOF ->DATA ^=8;
		
		}
		
	}
	
}
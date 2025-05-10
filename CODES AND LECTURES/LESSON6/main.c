


#include<TM4C123.h>

int main(){

	SYSCTL -> RCGCGPIO |= 0x20;  //turn on bus click for GPIOF
	GPIOF -> DIR |=0x08; //set GREEN pin as a digital output pin
	GPIOF -> DEN |= 0x08; //Enable PF3 pin as a digital pin
	
	
	SysTick -> LOAD = 15999999; //ONE SECOND DELAY RELOAD VALUE
	SysTick -> CTRL = 7; //enable counter, interrupt and select bus clock
	SysTick -> VAL = 0;  //initialize current value 
	
	while (1){
	
		//do nothing since now we use the systick handler
		
	}
	
	//this routine will execute after every one second
	


}

	void SysTick_Handler(void){
	
	GPIOF -> DATA ^=8; //toggle PF3 pin;
	
	}
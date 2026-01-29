/* Program to turn Green LED(PF3) ON & OFF with a Delay of 1sec */

#include <stdint.h>
#include "tm4c123gh6pm.h"

int main()
{
	SYSCTL_RCGCGPIO_R |= 0x20;    // turn on bus clock for GPIOF
  GPIO_PORTF_DIR_R      |= 0x08; //set PF3 pin as an output
  GPIO_PORTF_DEN_R       |= 0x08;  // Enable PF3 pin as a digital pin
	
	NVIC_ST_RELOAD_R = 16000000-1; // one second delay relaod value
	NVIC_ST_CTRL_R = 5 ; // enable counter and select system clock as clock source
	NVIC_ST_CURRENT_R  = 0; //initialize counter by clearing Current value register 
	
	while (1)
	{
		if(NVIC_ST_CTRL_R & 0x10000)
		{
			GPIO_PORTF_DATA_R  ^= 8;  // Toggle GREEN LED on PF3
	  }
	}
}

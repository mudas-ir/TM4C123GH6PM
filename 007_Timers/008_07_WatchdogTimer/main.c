/* WATCHDOG TIMER & RED LED */
#include <stdint.h>
#include "tm4c123gh6pm.h"

int main(void)
{
	/* enable clock to Watchdog Timer0 */
 	SYSCTL_RCGCWD_R |=0x01;  
	/* enable clock to GPIOF at clock gating control register */
  SYSCTL_RCGCGPIO_R |= 0x20;  // Enable PortF
 
	/* enable the GPIO pin 1 of PortF as output */
 GPIO_PORTF_DIR_R = 0x02; /*Enable PF1 as output*/
 /* enable the GPIO pins for digital function */
 GPIO_PORTF_DEN_R = 0x02;
 
 if(WATCHDOG0_LOCK_R & 0x01)
	 WATCHDOG0_LOCK_R= 0x1ACCE551;  /*A write of the value 0x1ACC.E551 unlocks the watchdog registers for write access */
 
 WATCHDOG0_LOAD_R = 16000000-1;  //Approx 1s. 8000000->500msec
 WATCHDOG0_CTL_R =0x02; // reset Enable
 NVIC_EN0_R |= 0x00040000;  /* enable IRQ18 (D18 of ISER[0]) */
 WATCHDOG0_CTL_R =0x01; // Interrupt Enable
 
 while(1)
 {
 }

}


void WDT0_Handler(void)
{
    while(WATCHDOG0_RIS_R&1)
			WATCHDOG0_ICR_R=0;
   		
		GPIO_PORTF_DATA_R  ^= 2; //Toggle RED LED on PF1
  
}
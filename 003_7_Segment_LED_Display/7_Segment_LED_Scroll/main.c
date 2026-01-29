/* ************************************************
 *Interfacing of 7 segment LED
 **************************************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"

void delayMs(int n);
void shift_out1(unsigned char str);

//unsigned char a[16] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0xEE,0x3E,0x9C,0x7A,0x9E,0x8E};
//unsigned char a[5] = {0xB6,0x0C,0x1E,0x1C,0x00};  /* sitl*/
unsigned char a[5] = {0x60,0xDA,0xF2,0x66,0x00}; /*1234*/
unsigned int i, T=100;

int main(void)
{
  SYSCTL_RCGCGPIO_R |= 0x24;
  GPIO_PORTC_DIR_R |= 0x10;
  GPIO_PORTF_DIR_R |= 0x0F;
  GPIO_PORTC_DEN_R |= 0x10;
  GPIO_PORTF_DEN_R |= 0x0F;

	
	while(1)
		{
			/*shift_out1(a[3]);
			shift_out1(a[2]);
			shift_out1(a[1]);
			shift_out1(a[0]);
			shift_out1(a[4]);
		  delayMs(T);
			shift_out1(a[2]);
			shift_out1(a[1]);
			shift_out1(a[0]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			delayMs(T);
			shift_out1(a[1]);
			shift_out1(a[0]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			delayMs(T);
		  shift_out1(a[0]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			delayMs(T);*/
			
			shift_out1(a[0]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			delayMs(T);
			shift_out1(a[1]);
			shift_out1(a[0]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			delayMs(T);
			shift_out1(a[2]);
			shift_out1(a[1]);
			shift_out1(a[0]);
			shift_out1(a[4]);
			shift_out1(a[4]);
			delayMs(T);
			shift_out1(a[3]);
			shift_out1(a[2]);
			shift_out1(a[1]);
			shift_out1(a[0]);
			shift_out1(a[4]);
			delayMs(T);
		  }
}

void shift_out1(unsigned char str)
{
unsigned char j=0,check;
	
	GPIO_PORTC_DATA_R = 0x00;
	for(j=0;j<8;j++)
	{		
		GPIO_PORTF_DATA_R &= 0xF7;	//PF3 pin(sclk) is low (0000 0000)
				
		if(str & 0x01)
			GPIO_PORTF_DATA_R |=  0x04;	//PF2 pin(sdat) is high (0000 0100)
		 
     else
		 GPIO_PORTF_DATA_R &=  0xFB; // PF2 pin(sdat) is low (0000 0000)
                  
   str >>= 1;
   GPIO_PORTF_DATA_R |=  0x08;	//PF3 pin(sclk) is high (0000 1000)
		   
	}
	GPIO_PORTC_DATA_R =  0x10;
}

void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3200; j++)
        {}  /* do nothing for 1 ms */
}

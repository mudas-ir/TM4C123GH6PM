/* I2C_DAC7571 via I2C
 *
 * The program generates sinewave output through DAC.
 *  I2C1SCL PA6
 *  I2C1SDA PA7
 * Built and tested with Keil MDK-ARM v4 and TM4C_
 */

#include "tm4c123gh6pm.h"
#include <stdint.h>
#define SLAVE_ADDR 0x4C     // 1001 1x0.

void delayMs(int n);

volatile signed short Wave[128]=
	{2047,2147,2247,2347,2446,2544,2641,2736,2830,2922,3011,3099,3184,3266,3345,3421,3494,
	3563,3629,3691,3749,3803,3852,3897,3938,3974,4006,4033,4055,4072,4084,4092,4094,4092,
	4085,4073,4056,4034,4007,3975,3939,3899,3854,3804,3751,3693,3631,3566,3496,3424,3348,
	3269,3187,3102,3014,2925,2833,2739,2644,2547,2449,2350,2250,2150,2050,1950,1850,1750,
	1651,1553,1456,1361,1267,1175,1085,998,913,831,751,675,602,533,467,405,347,293,243,
	198,157,121,89,62,40,22,10,2,0,2,9,21,38,60,86,117,153,194,239,288,342,399,461,526,
	595,668,744,823,905,990,1077,1166,1258,1352,1447,1544,1642,1741,1840,1941};


/*volatile unsigned short Wave[256] =
    {127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
 242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
 221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
 76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
 33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124};  //Sine Wave*/

//unsigned short Wave[24] ={2047,2576,3070,3494,3820,4024,4094,4025,3821,3496,3073,2579,2050,1521,1027,602,276,70,0,68,271,595,1018,1511};

//volatile unsigned short Wave[24] ={3800,4800,3800,4800,3800,4800,3800,4800,3800,4800,3800,4800,3800,4800,3800,4800,3800,4800,3800,4800,3800,4800,3800,4800};  /* Square Wave */
//volatile unsigned short Wave[24] ={2800,2850,2900,2950,3000,3050,3100,3150,3200,3250,3300,3350,3400,3350,3300,3250,3200,3150,3100,3050,3000,2950,2900,2850}; /* Triangular Wave */
	
unsigned char datah, datal; 
unsigned short value1;
		 
int main(void) 
{
    short i;
		char error;
		
		//SYSCTL_RCC_R = 0x02401551;   // 40MHz Clock from PIOS=16MHz Divisor=0x04(200Mhz/5)
    SYSCTL_RCGCI2C_R |= 0x02;    // enable clock to I2C1
    SYSCTL_RCGCGPIO_R |= 0x01;   // enable clock to GPIOA

    // PORTA 7, 6 for I2C1
    GPIO_PORTA_AFSEL_R |= 0xC0;       // PORTA 7, 6 for I2C1
    GPIO_PORTA_PCTL_R &= ~0xFF000000; // PORTA 7, 6 for I2C1
    GPIO_PORTA_PCTL_R |= 0x33000000;
    GPIO_PORTA_DEN_R |= 0xC0;         // PORTA 7, 6 as digital pins
    GPIO_PORTA_ODR_R |= 0x80;         // PORTA 7 as open drain

    
	  I2C1_MCR_R =  0x00;           // master mode
    I2C1_MCR_R =  0x10;           // master mode
	  //I2C1_MTPR_R = 0x13;            // 100 kHz @ 40 MHz
	  I2C1_MTPR_R = 0x08;            // 93 kbps @ 16.7 MHz
		
	  // send slave address and starting address
    I2C1_MSA_R = SLAVE_ADDR<<1 ;
    delayMs(100);   // wait until write is complete
		 		
		while(1)
	  {
	      for(i=0;i<128;i++)
				{
						value1 = Wave[i] >> 8;
						datah = (unsigned char) value1; 
						datal = (unsigned char) Wave[i];
	
					// send data->Most Significant byte 
						I2C1_MDR_R = datah;             // write the MS byte
						I2C1_MCS_R = 3;                 // Repeated START condition followed by a TRANSMIT(master remains in Master Transmit state)
						while(I2C1_MCS_R & 1);					//wait until I2C master is not busy
 

					// send data-Lest Significantt byte and a STOP
						I2C1_MDR_R = datal;                 // write the last byte
						I2C1_MCS_R = 5;                     // Repeated START condition followed by a TRANSMIT(master remains in Master Transmit state)
						while(I2C1_MCS_R & 1);							//wait until I2C master is not busy
				
				} 
			
    }
}


/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
 int i, j;
 for(i = 0 ; i < n; i++)
 for(j = 0; j < 400; j++)
 {} /* do nothing for 1 ms */
}

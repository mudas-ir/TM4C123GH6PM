 /* ************************************************
 *Project name :Interfacing of analog hex keypad and Output displaying in LCD 16x2 using TIVA C launchpad
 *Date : 12/04/2018
 ***************************************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"


#define LCD_clear()   LCD_command(0x1)        /* Clear display LCD */
#define LCD_origin()  LCD_command(0x2)        /* Set to origin LCD */
#define LCD_row1()    LCD_command(0x80)        /* Begin at Line 1 */
#define LCD_row2()    LCD_command(0xC0)  /* Begin at Line 2 */
void shift_out1(unsigned char str);
void LCD_enable(void);
void LCD_command(unsigned char command);
void LCD_putc(unsigned char ascii);
void LCD_puts(unsigned char *lcd_string);
void LCD_init(void);
unsigned char PP2 = 0x00;
unsigned char PP0 = 0x00;
void delayMs(int n);
unsigned char key_scan(unsigned int volatile rec_val);//stores key pressed

unsigned char Dig_val;//values for LCD
int main(void)
{
	unsigned int ADCValue;
	unsigned char Dig_val;//values for LCD
	volatile uint32_t ui32TempAvg,Temp_val;
	
	
	
	SYSCTL_RCGCGPIO_R |= 0x038;   /* enable clock to GPIOD/E/A/F */
  SYSCTL_RCGCADC_R |= 1;       /* enable clock to ADC0 */
    
    /* initialize PD2 for AIN5 input  */
    GPIO_PORTD_AFSEL_R |= 4;       /* enable alternate function */
    GPIO_PORTD_DEN_R &= ~4;        /* disable digital function */
    GPIO_PORTD_AMSEL_R |= 4;       /* enable analog function */
	GPIO_PORTE_DIR_R |= 0x20;			/*   */
	GPIO_PORTE_DEN_R |= 0x20;
	//GPIO_PORTA_DIR_R |= 0x20;
	//GPIO_PORTA_DEN_R |= 0x20;
	GPIO_PORTF_DIR_R |= 0x0C;
	GPIO_PORTF_DEN_R |= 0x0C;
    /* initialize ADC0 */
   ADC0_ACTSS_R &= ~1;        /* disable SS0 during configuration */
   ADC0_EMUX_R &= ~0x000F;    /* software trigger conversion */ 
   ADC0_SSMUX0_R |= 0x05;         /* get input from channel 4 */
   ADC0_SSCTL0_R |= 0x06;        /* take one sample at a time, set flag at 1st sample */
   ADC0_ACTSS_R |= 0x01;           /* enable ADC0 sequencer 0 */
    
    
	  LCD_init();	//LCD 16x2 initializing
	  LCD_row1();	//Cursor position at 1st line
	  LCD_puts("Interfacing");
    LCD_row2();	//Cursor position at 2nd line
    LCD_puts("Analog KEYPAD");
    delayMs(2000);
    LCD_clear();
		
		while(1)
    {
    ADC0_PSSI_R |= 1;        /* start a conversion sequence 0 */
    while((ADC0_RIS_R & 1) == 0) ;   /* wait for conversion complete */
    ADCValue = ADC0_SSFIFO0_R; /* read conversion result */
    ADC0_ISC_R = 1;          /* clear completion flag */
   	Dig_val = key_scan(ADCValue);//compare the key pressed
		delayMs(100);
		LCD_row1();
		LCD_puts("Press any Key");
		LCD_row2();
		if(Dig_val <= 'F')//if the ascii is less or equal to F put the value in LCD
		LCD_putc(Dig_val);
	 }
 }


unsigned char key_scan(unsigned int volatile rec_val)
{
		if( (rec_val>>4) == 0xB9 || (rec_val>>4) == 0xB8 || (rec_val>>4) == 0xB7 || (rec_val>>4) == 0xB6  )	return '0';	    // 0 key pressed
	else if( (rec_val>>4) == 0xB5 || (rec_val>>4) == 0xB4 || (rec_val>>4) == 0xB3 || (rec_val>>4) == 0XB2 )  return '1'; 	// 1st key pressed
	else if( (rec_val>>4) == 0xB0 || (rec_val>>4) == 0xAF || (rec_val>>4) == 0xAE || (rec_val>>4) == 0xAD )  return '2';	  // 2nd key pressed
	else if( (rec_val>>4) == 0xAC || (rec_val>>4) == 0xAB || (rec_val>>4) == 0xAA || (rec_val>>4) == 0xA9 )  return '3'; 	  // 3rd key pressed
	else if( (rec_val>>4) == 0xA8 || (rec_val>>4) == 0xA7 || (rec_val>>4) == 0xA6 )  return '4';    // 4th key pressed
	else if( (rec_val>>4) == 0xA2 || (rec_val>>4) == 0xA1 || (rec_val>>4) == 0xA0 || (rec_val>>4) == 0x9F )  return '5';    // 5th key pressed
	else if( (rec_val>>4) == 0x9A || (rec_val>>4) == 0x99 || (rec_val>>4) == 0x98 || (rec_val>>4) == 0x97 )  return '6';    // 6th key pressed
	else if( (rec_val>>4) == 0x94 || (rec_val>>4) == 0x93 || (rec_val>>4) == 0x92 || (rec_val>>4) == 0x91 )  return '7';   	// 7th key pressed
	else if( (rec_val>>4) == 0x8D || (rec_val>>4) == 0x8C || (rec_val>>4) == 0x8B )  return '8';    	// 8th key pressed
	else if( (rec_val>>4) == 0x82 || (rec_val>>4) == 0x81 || (rec_val>>4) == 0x80 ) return '9';   	// 9th key pressed
	else if( (rec_val>>4) == 0x74 || (rec_val>>4) == 0x73 || (rec_val>>4) == 0x72 )  return 'A';   	// A  key pressed
	else if( (rec_val>>4) == 0x67 || (rec_val>>4) == 0x66 || (rec_val>>4) == 0x65 )  return 'B';   	// B key pressed
	else if( (rec_val>>4) == 0x5B || (rec_val>>4) == 0x5A)  return 'C';   	// C key pressed
	else if( (rec_val>>4) == 0x43 || (rec_val>>4) == 0x42 || (rec_val>>4) == 0x41  )  return 'D';    	// D key pressed
	else if( (rec_val>>4) == 0x21 || (rec_val>>4) == 0x20  )  return 'E';			// E key pressed
	else if( (rec_val>>4) == 0x00  )  return 'F';   	// F key pressed
	else 
	return 'G';
}   //*/

void LCD_init()
{
		delayMs(20);
	    LCD_command(0x30);
		delayMs(5);
      LCD_command(0x30);
	delayMs(5);
      LCD_command(0x30);
	delayMs(5);
      LCD_command(0x20);
  	delayMs(5);
		  LCD_command(0x20);
	  delayMs(5);
	    LCD_command(0x28); //Selecting 4bit LCD 16x2
    delayMs(5);
	    LCD_command(0x0C); //LCD cursor off
    delayMs(5);
		  LCD_command(0x06); //LCD display on
    delayMs(5);
		  LCD_command(0x01); //LCD clear screen
    delayMs(5);	
}

void LCD_puts(unsigned char *lcd_string)
{
      while (*lcd_string)
      {
              LCD_putc(*lcd_string++);
      }
}

void LCD_command(unsigned char command)
{
	unsigned char volatile num;
	num = command;
	num = ((num & 0x11)<<3)| ((num & 0x22)<<1)|((num & 0x44)>>1)|((num & 0x88)>>3);
	command = num;
    PP2 = (PP2 & 0xF0)|((command>>4) & 0x0F);//ok
	PP2 = PP2 & (~(3 << 7)); //  setting 7th pin as low RS = 0 , RW = 0
	PP2 = PP2 | 0x20;	// EN = 1
	shift_out1(PP2);
		//delayUs(1);
		PP2 = PP2 & ~(0x20); //EN = 0
	shift_out1(PP2);
		//delayUs(1);
	  PP2 = (PP2 & 0xF0)|(command & 0x0F);
	PP2 = PP2 & (~(3 << 7));   // RS = 0,RW = 0
	PP2 = PP2 | 0x20;	// EN = 1
	shift_out1(PP2);
		//delayUs(1);
		PP2 = PP2 & ~(0x20); // EN = 0
	shift_out1(PP2);
		//delayUs(1);
}

void LCD_putc(unsigned char ascii)
{
	unsigned char volatile  num;
	num = ascii;
	num = ((num & 0x11)<<3)| ((num & 0x22)<<1)|((num & 0x44)>>1)|((num & 0x88)>>3);
	ascii = num;
    PP2 = (PP2 & 0xF0)|((ascii>>4) & 0x0F);
	PP2 = PP2 | 0xA0; // setting 7th pin as high RS = 1, EN = 1;
	PP2 = PP2 & ~(0x40); //RW = 0
	shift_out1(PP2);
	//delayUs(1);
	PP2 = PP2 & ~(0x20); // En = 0
	shift_out1(PP2);
	//delayUs(1);
  PP2 = (PP2 & 0xF0)|(ascii & 0x0F);
	PP2 = PP2 | 0xA0; // setting 7th bit as high RS = 1, EN = 1;
	PP2 = PP2 & ~(0x40); //RW = 0
	shift_out1(PP2);
	//delayUs(1);
	PP2 = PP2 & ~(0x20); // EN = 0
	shift_out1(PP2);
	//delayUs(1);
}

void shift_out1(unsigned char str)
{
   unsigned char j=0,check;
	GPIO_PORTE_DATA_R = 0x00;   //PE5 pin(stk) is low (0000 0000) 
	for(j=0;j<=7;j++)
	{
    GPIO_PORTF_DATA_R = 0x00;   //PE3 pin(sclk) is low (0000 0000)
		check = (str &(1<<j));
		if(check)
				GPIO_PORTF_DATA_R |= 0x04;   //PF2 pin(sdat) is high (0000 0100)
		else
			  GPIO_PORTF_DATA_R |= 0x00;   //PF2 pin(sdat) is low (0000 0000)

		GPIO_PORTF_DATA_R |= 0x08;   //PF3 pin(sclk) is high (0000 1000), sclk=1;
		}
	GPIO_PORTE_DATA_R = 0x20;   //PE5 pin(stk) is high (0010 0000) , stk=1,storing completed;
}

/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
 int i, j;
 for(i = 0 ; i < n; i++)
 for(j = 0; j < 2000; j++)
 {} /* do nothing for 1 ms */
}

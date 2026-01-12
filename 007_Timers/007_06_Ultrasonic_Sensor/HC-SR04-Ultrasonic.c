/* This example code Measures the distance using HC-SR04 Ultrasonic range sensor*/
/* Timer0A is used to measure distance by measuring pulse duration of Echo output signal
 on GPIO PORTB PIN 6*/
/* Timer1A is used to make percise microsecond delay function */

#include <stdint.h>
#include "tm4c123gh6pm.h"

uint32_t Measure_distance(void);
void Timer0ACapture_init(void);
void Delay_MicroSecond(int time);
void Delay(unsigned long counter);

/* global variables to store and display distance in cm */
unsigned int time;              /*stores pulse on time */
volatile unsigned int distance; /* stores measured distance value */

int main(void)
{
  Timer0ACapture_init(); /*initialize Timer0A in edge edge time */
  while (1)
  {
    time = Measure_distance();            /* take pulse duration measurement */
    distance = (time * 10625) / 10000000; /* convert pulse duration into distance */
    // sprintf(mesg, "\r\nDistance = %d cm", distance); /*convert float type distance data into string */
    Delay(2000);
  }
}

uint32_t Measure_distance(void)
{
  int lastEdge, thisEdge;

  /* Given 10us trigger pulse */
  GPIO_PORTA_DATA_R &= ~(1 << 4); /* make trigger  pin low */
  Delay_MicroSecond(10);          /*10 seconds delay */
  GPIO_PORTA_DATA_R |= (1 << 4);  /* make trigger  pin high */
  Delay_MicroSecond(10);          /*10 seconds delay */
  GPIO_PORTA_DATA_R &= ~(1 << 4); /* make trigger  pin low */

  while (1)
  {
    TIMER0_ICR_R = 4; /* clear timer0A capture flag */
    while ((TIMER0_RIS_R & 4) == 0)
      ;                               /* wait till captured */
    if (GPIO_PORTB_DATA_R & (1 << 6)) /*check if rising edge occurs */
    {
      lastEdge = TIMER0_TAR_R; /* save the timestamp */
                               /* detect falling edge */
      TIMER0_ICR_R = 4;        /* clear timer0A capture flag */
      while ((TIMER0_RIS_R & 4) == 0)
        ;                           /* wait till captured */
      thisEdge = TIMER0_TAR_R;      /* save the timestamp */
      return (thisEdge - lastEdge); /* return the time difference */
    }
  }
}

/* Timer0A initialization function */
/* Initialize Timer0A in input-edge time mode with up-count mode */
void Timer0ACapture_init(void)
{
  SYSCTL_RCGCTIMER_R |= 1; /* enable clock to Timer Block 0 */
  SYSCTL_RCGCGPIO_R |= 3;  /* enable clock to PORTA & PORTB */

  /* PA4 as a digital output signal to provide trigger signal */
  GPIO_PORTA_DIR_R |= (1 << 4); /* set PA4 as a digial output pin */
  GPIO_PORTA_DEN_R |= (1 << 4); /* make PA4 as digital pin */

  /* PB6 as an input pin configure as TIMER0 to capture echo pulse */
  GPIO_PORTB_DIR_R &= ~0x40;        /* make PB6 an input pin */
  GPIO_PORTB_DEN_R |= 0x40;         /* make PB6 as digital pin */
  GPIO_PORTB_AFSEL_R |= 0x40;       /* use PB6 alternate function */
  GPIO_PORTB_PCTL_R &= ~0x0F000000; /* configure PB6 for T0CCP0 */
  GPIO_PORTB_PCTL_R |= 0x07000000;

  TIMER0_CTL_R &= ~1;                                 /* disable timer0A during setup */
  TIMER0_CFG_R = 4;                                   /* 16-bit timer mode */
  TIMER0_TAMR_R = 0x17;                               /* up-count, edge-time, capture mode */
  TIMER0_CTL_R |= 0x0C; /* capture the rising edge */ //! both edges
  TIMER0_CTL_R |= (1 << 0);                           /* enable timer0A */
}

/* Create one microsecond second delay using Timer block 1 and sub timer A */

void Delay_MicroSecond(int time)
{
  int i;
  SYSCTL_RCGCTIMER_R |= 2; /* enable clock to Timer Block 1 */
  TIMER1_CTL_R = 0;        /* disable Timer before initialization */
  TIMER1_CFG_R = 0x04;     /* 16-bit option */
  TIMER1_TAMR_R = 0x02;    /* periodic mode and down-counter */
  TIMER1_TAILR_R = 16 - 1; /* TimerA interval load value reg */
  TIMER1_ICR_R = 0x1;      /* clear the TimerA timeout flag */
  TIMER1_CTL_R |= 0x01;    /* enable Timer A after initialization */

  for (i = 0; i < time; i++)
  {
    while ((TIMER1_RIS_R & 0x1) == 0)
      ;                 /* wait for TimerA timeout flag */
    TIMER1_ICR_R = 0x1; /* clear the TimerA timeout flag */
  }
}

void Delay(unsigned long counter)
{
  unsigned long i = 0;

  for (i = 0; i < counter * 1000; i++)
    ;
}
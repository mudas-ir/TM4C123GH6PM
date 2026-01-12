/* Program to toggle Green LED (PF3) ON & OFF every 1 sec using SysTick interrupt */
#include <stdint.h>
#include "tm4c123gh6pm.h"

void SysTick_Handler(void); // ISR declaration

int main(void)
{
    // 1. Enable clock for PORTF
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0); // wait until PortF is ready

    // 2. Configure PF3 as output and digital pin
    GPIO_PORTF_DIR_R |= 0x08;  // PF3 -> output
    GPIO_PORTF_DEN_R |= 0x08;  // PF3 -> digital enable

    // 3. Configure SysTick
    NVIC_ST_RELOAD_R = 15999999;  // 1 second delay (assuming 16 MHz)
    NVIC_ST_CURRENT_R = 0;        // clear current value register
    NVIC_ST_CTRL_R = 0x07;        // enable SysTick, with system clock and interrupt

    while (1)
    {
        // main loop does nothing, LED toggling handled by SysTick interrupt
    }
}

// 4. SysTick Interrupt Service Routine (ISR)
void SysTick_Handler(void)
{
    GPIO_PORTF_DATA_R ^= 0x08;  // Toggle PF3 (Green LED)
}

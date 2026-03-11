/******************************************************************************
*	Project name: Digital Input/Output Interfacing and Programming
*	File name: lab5_gpio_led.c
*	Author: Ali Maaz
*	Date: 27/11/2025
*	Description: This file is for the GPIO interfacing of external LED
*
 ******************************************************************************/

#include "TM4C123.h"

/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/	
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long*) 0x400FE608))
#define GPIO_PORTB_DATA_R (*((volatile unsigned long*) 0x400053FC))
#define GPIO_PORTB_DIR_R (*((volatile unsigned long*) 0x40005400))
#define GPIO_PORTB_DEN_R (*((volatile unsigned long*) 0x4000551C))

/* Enable clock for Port B */
#define GPIO_PORTB_CLK_EN 0x02
/* PB0 GPIO configuration */
#define GPIO_PORTB_PIN0_EN 0x01
/* LED ON */
#define LED_ON GPIO_PORTB_PIN0_EN
/* LED off */
#define LED_OFF ~(GPIO_PORTB_PIN0_EN)
/* Delay */
#define DELAY 200000

//System Initialization for Floating Point Unit
void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

int main ( void )
{
	volatile unsigned long ulLoop ;

	 /* Enable the GPIO port that is used to drive the output */
	SYSCTL_RCGCGPIO_R |= GPIO_PORTB_CLK_EN  ;
	 /* Do a dummy read to insert a few cycles after enabling the peripheral. */
	ulLoop = SYSCTL_RCGCGPIO_R ;
	
	/* Enable the GPIO pin for the LED (PF3) for digital function.
	Set the direction as output.*/
	GPIO_PORTB_DEN_R |= 0x01 ;
	GPIO_PORTB_DIR_R |= 0x01 ;
	
	/* Loop forever */
	while (1)
	{
		 /* Turn on the LED */
		GPIO_PORTB_DATA_R |= LED_ON ;
		/* Delay for a bit. */
		for ( ulLoop = 0 ; ulLoop < DELAY ; ulLoop++) ;
		/* Turn off the LED. */
		GPIO_PORTB_DATA_R &= LED_OFF  ;
		/* Delay for a bit. */
		for ( ulLoop = 0 ; ulLoop < DELAY ; ulLoop++ ) ;
	}
}

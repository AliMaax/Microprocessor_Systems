/******************************************************************************
* � Project name: Digital Input/Output Interfacing and Programming
* � File name: lab5_switch_led.c
* � Author: Ali Maaz
* � Date: 27/11/2025
* � Description: This file implements a switch on PB4 powered by PB0 to control the blinking of the green LED on PF3.
******************************************************************************/

#include "TM4C123.h"

/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/ 
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long*) 0x400FE608))

/* Data Registers for PB0 and PB4 */
#define GPIO_PORTB_PIN_0_DATA_R (*((volatile unsigned long*) 0x40005004)) // Acess only 1 pin i.e. PB0
#define GPIO_PORTB_PIN_4_DATA_R (*((volatile unsigned long*) 0x40005040)) // Acess only 1 pin i.e. PB4

/* Direction, Digital Enable, and Pull-Down Select Registers for PB0 and PB4 */
#define GPIO_PORTB_DIR_R  (*((volatile unsigned long*) 0x40005400))
#define GPIO_PORTB_DEN_R  (*((volatile unsigned long*) 0x4000551C))
#define GPIO_PORTB_PDR_R  (*((volatile unsigned long*) 0x40005514))


/* Data Register for PF3 */
#define GPIO_PORTF_PIN_3_DATA_R (*((volatile unsigned long*) 0x40025020)) // Acess only 1 pin i.e. PF3 (Green LED)

/* Direction and Digital Enable Registers for PF3 */
#define GPIO_PORTF_DIR_R  (*((volatile unsigned long*) 0x40025400))
#define GPIO_PORTF_DEN_R  (*((volatile unsigned long*) 0x4002551C))


/* Enable clock for Port B */
#define GPIO_PORTB_CLK_EN  0x02   
/* Enable clock for Port F */
#define GPIO_PORTF_CLK_EN  0x20  


/* PB0 GPIO configuration */
#define GPIO_PORTB_PIN0_EN 0x01   
/* PB4 GPIO configuration */
#define GPIO_PORTB_PIN4_EN 0x10 
/* PF3 GPIO configuration */
#define GPIO_PORTF_PIN3_EN 0x08


/* PB0 HIGH */
#define PB0_POWER_ON 0x01
/* LED ON */
#define LED_GREEN_ON 0x08
/* LED OFF */
#define LED_GREEN_OFF 0x00


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


		/* Enable the clock for Port B */
		SYSCTL_RCGCGPIO_R |= GPIO_PORTB_CLK_EN;
		/* Enable the clock for Port F */
		SYSCTL_RCGCGPIO_R |= GPIO_PORTF_CLK_EN;
	
	
		/* Do a dummy read to insert a few cycles after enabling the peripheral. */
		ulLoop = SYSCTL_RCGCGPIO_R ;


		/* Enable the GPIO pin PF3 for digital function */
		GPIO_PORTF_DEN_R |= GPIO_PORTF_PIN3_EN ;
		/* Enable the GPIO pin PF3 as output */
		GPIO_PORTF_DIR_R |= GPIO_PORTF_PIN3_EN ;


    /* Enable GPIO pin PB0 for digital function */
    GPIO_PORTB_DEN_R |= GPIO_PORTB_PIN0_EN;
		/* Enable GPIO pin PB4 for digital function */
    GPIO_PORTB_DEN_R |= GPIO_PORTB_PIN4_EN;


    /* Enable the GPIO pin PB0 as output */
    GPIO_PORTB_DIR_R |= GPIO_PORTB_PIN0_EN;
    /* Enable the GPIO pin PB4 as input*/
    GPIO_PORTB_DIR_R &= ~(GPIO_PORTB_PIN4_EN);


    /* Enable the internal pull-down resistor on PB4 */
    GPIO_PORTB_PDR_R |= GPIO_PORTB_PIN4_EN;

 
    /* Set PB0 HIGH initially*/
    GPIO_PORTB_PIN_0_DATA_R = PB0_POWER_ON;
 
	
		/* Loop forever */
		while (1)
		{

	/* Controlled by dip switch */
        if ( GPIO_PORTB_PIN_4_DATA_R != 0 )
        {

						/* Turn on the LED */
						GPIO_PORTF_PIN_3_DATA_R = LED_GREEN_ON;
						/* Delay for a bit. */
						for ( ulLoop = 0 ; ulLoop < DELAY ; ulLoop++);
						/* Turn off the LED. */
						GPIO_PORTF_PIN_3_DATA_R = LED_GREEN_OFF;
						/* Delay for a bit. */
						for ( ulLoop = 0 ; ulLoop < DELAY ; ulLoop++ );
        }
        else
        {
            GPIO_PORTF_PIN_3_DATA_R = LED_GREEN_OFF;
        }
		}
}/******************************************************************************
* � Project name: Digital Input/Output Interfacing and Programming
* � File name: lab5_switch_led.c
* � Author: Sohaib Liaquat
* � Date: 27/11/2025
* � Description: This file implements a switch on PB4 powered by PB0 to control the blinking of the green LED on PF3.
******************************************************************************/

#include "TM4C123.h"

/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/ 
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long*) 0x400FE608))

/* Data Registers for PB0 and PB4 */
#define GPIO_PORTB_PIN_0_DATA_R (*((volatile unsigned long*) 0x40005004)) // Acess only 1 pin i.e. PB0
#define GPIO_PORTB_PIN_4_DATA_R (*((volatile unsigned long*) 0x40005040)) // Acess only 1 pin i.e. PB4

/* Direction, Digital Enable, and Pull-Down Select Registers for PB0 and PB4 */
#define GPIO_PORTB_DIR_R  (*((volatile unsigned long*) 0x40005400))
#define GPIO_PORTB_DEN_R  (*((volatile unsigned long*) 0x4000551C))
#define GPIO_PORTB_PDR_R  (*((volatile unsigned long*) 0x40005514))


/* Data Register for PF3 */
#define GPIO_PORTF_PIN_3_DATA_R (*((volatile unsigned long*) 0x40025020)) // Acess only 1 pin i.e. PF3 (Green LED)

/* Direction and Digital Enable Registers for PF3 */
#define GPIO_PORTF_DIR_R  (*((volatile unsigned long*) 0x40025400))
#define GPIO_PORTF_DEN_R  (*((volatile unsigned long*) 0x4002551C))


/* Enable clock for Port B */
#define GPIO_PORTB_CLK_EN  0x02   
/* Enable clock for Port F */
#define GPIO_PORTF_CLK_EN  0x20  


/* PB0 GPIO configuration */
#define GPIO_PORTB_PIN0_EN 0x01   
/* PB4 GPIO configuration */
#define GPIO_PORTB_PIN4_EN 0x10 
/* PF3 GPIO configuration */
#define GPIO_PORTF_PIN3_EN 0x08


/* PB0 HIGH */
#define PB0_POWER_ON 0x01
/* LED ON */
#define LED_GREEN_ON 0x08
/* LED OFF */
#define LED_GREEN_OFF 0x00


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


		/* Enable the clock for Port B */
		SYSCTL_RCGCGPIO_R |= GPIO_PORTB_CLK_EN;
		/* Enable the clock for Port F */
		SYSCTL_RCGCGPIO_R |= GPIO_PORTF_CLK_EN;
	
	
		/* Do a dummy read to insert a few cycles after enabling the peripheral. */
		ulLoop = SYSCTL_RCGCGPIO_R ;


		/* Enable the GPIO pin PF3 for digital function */
		GPIO_PORTF_DEN_R |= GPIO_PORTF_PIN3_EN ;
		/* Enable the GPIO pin PF3 as output */
		GPIO_PORTF_DIR_R |= GPIO_PORTF_PIN3_EN ;


    /* Enable GPIO pin PB0 for digital function */
    GPIO_PORTB_DEN_R |= GPIO_PORTB_PIN0_EN;
		/* Enable GPIO pin PB4 for digital function */
    GPIO_PORTB_DEN_R |= GPIO_PORTB_PIN4_EN;


    /* Enable the GPIO pin PB0 as output */
    GPIO_PORTB_DIR_R |= GPIO_PORTB_PIN0_EN;
    /* Enable the GPIO pin PB4 as input*/
    GPIO_PORTB_DIR_R &= ~(GPIO_PORTB_PIN4_EN);


    /* Enable the internal pull-down resistor on PB4 */
    GPIO_PORTB_PDR_R |= GPIO_PORTB_PIN4_EN;

 
    /* Set PB0 HIGH initially*/
    GPIO_PORTB_PIN_0_DATA_R = PB0_POWER_ON;
 
	
		/* Loop forever */
		while (1)
		{

	/* Controlled by dip switch */
        if ( GPIO_PORTB_PIN_4_DATA_R != 0 )
        {

						/* Turn on the LED */
						GPIO_PORTF_PIN_3_DATA_R = LED_GREEN_ON;
						/* Delay for a bit. */
						for ( ulLoop = 0 ; ulLoop < DELAY ; ulLoop++);
						/* Turn off the LED. */
						GPIO_PORTF_PIN_3_DATA_R = LED_GREEN_OFF;
						/* Delay for a bit. */
						for ( ulLoop = 0 ; ulLoop < DELAY ; ulLoop++ );
        }
        else
        {
            GPIO_PORTF_PIN_3_DATA_R = LED_GREEN_OFF;
        }
		}
}
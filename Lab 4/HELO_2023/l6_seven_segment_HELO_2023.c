/******************************************************************************
*	Project name: LAB 6
*	File name: l6_seven_segment_TASK
*	Author: Ali Maaz 
*	Date: 2 Nov, 2025
*	Description: This is a template file for GPIO parallel interfacing for
*				Seven Segment Display. This program is written for common anode
*				type seven segment display
* 				Seven segment digits pins:		PA2-PA5*
*				Seven segment data pins:		PB0-PB7*
*				Port B pins:					76543210*
*				Seven Segment LEDs:				pgfedcba*
 ******************************************************************************/

#include "TM4C123.h"

void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}
	

/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/
/* Register for clock */
#define	SYSCTL_RCGCGPIO_R		(*((volatile unsigned long*)0x400FE608))

/* GPIO Registers for port B */	
#define	GPIO_PORTB_DATA_R		(*((volatile unsigned long*)0x400053FC))
#define	GPIO_PORTB_DIR_R		(*((volatile unsigned long*)0x40005400))
#define	GPIO_PORTB_DEN_R		(*((volatile unsigned long*)0x4000551C))
#define	GPIO_PORTB_AFSEL_R		(*((volatile unsigned long*)0x40005420))

/* GPIO Registers for port A */	
#define	GPIO_PORTA_DATA_R		(*((volatile unsigned long*)0x400040F0))
#define	GPIO_PORTA_DIR_R		(*((volatile unsigned long*)0x40004400))
#define	GPIO_PORTA_DEN_R		(*((volatile unsigned long*)0x4000451C))
#define	GPIO_PORTA_AFSEL_R		(*((volatile unsigned long*)0x40004420))

/* Values for enabling seven segments */
#define	SEG_1		0xFB  
#define	SEG_2		0xF7  
#define	SEG_3		0xEF  
#define	SEG_4		0xDF 
#define	SEG_OFF		0xFF

/* Function Declarations */	
void init_gpio(void);
void display_1(void);
void display_2(void);
void delay(unsigned long value);

/*Lookup tables for common anode display */


							
/* lut for display1 */
const char lut_display1[4]={0x89,//H --> 10001001 (0x89) for general hex-mapping
							0x86,//E --> 10000110 (0x86)
							0xC7,//L --> 11000111 (0xC7)
							0xC0 //O --> 11000000 (0xC0)
							};

/*lut for display2*/
const char lut_display2[4]={0xA4,//2 --> 10100100 (0xA4) for general hex-mapping
							0xC0,//0 --> 11000000 (0xC0)
							0xA4,//2 --> 10100100 (0xA4)
							0xB0 //3 --> 10110000 (0xB0) 
							};

/* lut for segment selection */
const char seg_select[]={0xFB,//SEG_1
						 0xF7,//SEG_2
						 0xEF,//SEG_3
						 0xDF//SEG_4
						};
						
/*initialization function for ports */
void init_gpio(void){
	volatile unsigned long delay_clk;
	
	/*enable clock for PortA and PortB */
	SYSCTL_RCGCGPIO_R |= 0x03;
	
	// dummy read for delay for clock,must have 3sys clock delay
	delay_clk=SYSCTL_RCGCGPIO_R;
	
	/* Enable the GPIO pin for PortB pins 0-7 for digital function
	and set the direction as output. Also disable alternate functionality */	
	GPIO_PORTB_DEN_R |= 0xFF;
	GPIO_PORTB_DIR_R |= 0xFF;
	GPIO_PORTB_AFSEL_R = 0x00;

	
	
	/* Enable the GPIO pin for PortA pins 2-5 for digital function.
	and set the direction as output. Also disable alternate functionality */
	GPIO_PORTA_DEN_R |= 0x3C;
	GPIO_PORTA_DIR_R |= 0x3C;
	GPIO_PORTA_AFSEL_R = 0x00;
	
}

/* display_1 on seven segments using Macros */
void display_1(void){
		volatile unsigned long i;
		for(i = 0; i < 4; i++){

			GPIO_PORTA_DATA_R = SEG_OFF;
			GPIO_PORTB_DATA_R = lut_display1[i];
			GPIO_PORTA_DATA_R = seg_select[3-i];
			delay(1000);

		}

}

/* display_2 on seven segments using for loop */
void display_2(void){
		volatile unsigned long i;
		for(i = 0; i < 4; i++){

			GPIO_PORTA_DATA_R = SEG_OFF;
			GPIO_PORTB_DATA_R = lut_display2[i];
			GPIO_PORTA_DATA_R = seg_select[3-i];
			delay(1000);

		}
	
}

/* Delay function */
void delay(unsigned long value){
	volatile unsigned long i ;
	for(i=0;i<value;i++);
}

/* Main function */
int main(void){

	volatile unsigned long count_hello;
	volatile unsigned long  count_2023;
	init_gpio();
	while(1)
		{

        // Display Hello for some time

        for(count_hello = 0; count_hello < 500; count_hello++){
            display_1();
        }

        // Display 2023 for some time
        for(count_2023 = 0; count_2023 < 500; count_2023++){
            display_2();
        }
		}
	
}

/******************************************************************************
*	Project name: Sohaib Liaquat
*	File name: l7_two_interrupts_configuration
*	Author: Ali Maaz
*	Date: 4 Nov 2025
*	Description: This is a template file for Multiple Interrupt configuration for 
*                User button connected to PF4 and PB4. It turns on Red LED for
*                PF4 and Green LED for PB4.
 ******************************************************************************/

/********************************************************************
* Macro definitions and Function Declarations for Pending Interrupts
********************************************************************/
#include "tm4c123.h"

/* Register for providing GPIO clock */
#define 		SYSCTL_RCGCGPIO_R		(*((volatile unsigned long*)0x400FE608))

/* Register Definitions for Port B */
/* NVIC Registers */
#define 	NVIC_EN_R					(*((volatile unsigned long*)0xE000E100))
#define 	NVIC_PRI0_R					(*((volatile unsigned long*)0xE000E400))
/* GPIO Registers */
#define 	GPIO_PORTB_DEN_R			(*((volatile unsigned long*)0x4000551C))
#define 	GPIO_PORTB_PUR_R			(*((volatile unsigned long*)0x40005510))
#define 	GPIO_PORTB_IM_R				(*((volatile unsigned long*)0x40005410))
#define 	GPIO_PORTB_IS_R				(*((volatile unsigned long*)0x40005404))
#define 	GPIO_PORTB_IBE_R			(*((volatile unsigned long*)0x40005408))
#define 	GPIO_PORTB_IEV_R			(*((volatile unsigned long*)0x4000540C))
#define 	GPIO_PORTB_ICR_R			(*((volatile unsigned long*)0x4000541C))
#define 	GPIO_PORTB_DIR_R			(*((volatile unsigned long*)0x40005400))
#define 	GPIO_PORTB_DATA_R			(*((volatile unsigned long*)0x400053FC))

/* Register Definitions for Port F */
/* NVIC Registers */
#define 	NVIC_EN_R					(*((volatile unsigned long*)0xE000E100))
#define 	NVIC_PRI7_R 				(*((volatile unsigned long*)0xE000E41C))
/* GPIO Registers */
#define 	GPIO_PORTF_DEN_R			(*((volatile unsigned long*)0x4002551C))
#define 	GPIO_PORTF_PUR_R			(*((volatile unsigned long*)0x40025510))
#define 	GPIO_PORTF_IM_R				(*((volatile unsigned long*)0x40025410))
#define 	GPIO_PORTF_IS_R				(*((volatile unsigned long*)0x40025404))
#define 	GPIO_PORTF_IBE_R			(*((volatile unsigned long*)0x40025408))
#define 	GPIO_PORTF_IEV_R			(*((volatile unsigned long*)0x4002540C))
#define 	GPIO_PORTF_ICR_R			(*((volatile unsigned long*)0x4002541C))
#define 	GPIO_PORTF_DIR_R			(*((volatile unsigned long*)0x40025400))
#define 	GPIO_PORTF_DATA_R			(*((volatile unsigned long*)0x400253FC))

/* Values for Port B*/
#define 	PORTB_CLK 					0x02
#define 	PORTB_INT 					0x00000002
#define 	PORTB_INT_PRI 				0x00008000
#define 	PORTB_PIN4 					0x10

/* Values for Port F*/
#define 	PORTF_CLK 					0x20
#define 	PORTF_INT 					0x40000000
#define 	PORTF_INT_PRI 				0x00200000
#define 	PORTF_PIN4 					0x10
#define		PORTF_LEDS					0x0A

/* Function Declarations */
void EnableInterrupts(void);								// Enable interrupts
void DisableInterrupts(void);								// Disable interrupts
void EnablePriorityInterrupts(void);						//Enable Priortized Interrupts
void Init_INT_GPIO(void);									// GPIO and Interrupt Initializations
void Delay(unsigned long value);							// Implements delay
void WaitForInterrupt(void);								// Implements WFI

void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

/*************************************************************************
* Delay Function
*************************************************************************/
void Delay(unsigned long value){
	volatile unsigned long i=0;
	for(i=0;i<value;i++);
}

/*************************************************************************
* Port B and Port F NVIC and Peripheral Level Configurations
*************************************************************************/
void Init_INT_GPIO(void)
{
	volatile unsigned long dummy;

	/* Enable clock for Port B & F */
	SYSCTL_RCGCGPIO_R |= PORTB_CLK;
	SYSCTL_RCGCGPIO_R |= PORTF_CLK ;

	/* Dummy read to stable the clock */
	dummy = SYSCTL_RCGCGPIO_R;
	
	/* Enable digital I/O on PB4, enable pull up and set its direction */
	GPIO_PORTB_DEN_R |= PORTB_PIN4;
	GPIO_PORTB_PUR_R |= PORTB_PIN4;
	GPIO_PORTB_DIR_R &= (~PORTB_PIN4); // As an input from outer switch  
	
	
	
	/* Enable digital I/O on PF4-PF1, enable pull up on PF4 and set their direction */
	GPIO_PORTF_DEN_R |= PORTF_PIN4;
	GPIO_PORTF_PUR_R |= PORTF_PIN4;
	GPIO_PORTF_DIR_R &= (~PORTF_PIN4);  // As an input from built-in switch  
	
	/*Enable the LEDs*/
	GPIO_PORTF_DEN_R |= PORTF_LEDS;
	GPIO_PORTF_DIR_R |= PORTF_LEDS;               // LEDs are output
	
	
	
	/* INTERRUPT Configuration */
	/* Globally disable all interrupts */
	DisableInterrupts();
	/* Enable interrupt on Port B and set its priority to 4 */
	NVIC_EN_R |= PORTB_INT;
	NVIC_PRI0_R |= PORTB_INT_PRI;

	/* Enable interrupt on Port F and set its priority to 1 */
	NVIC_EN_R |= PORTF_INT;
	NVIC_PRI7_R |= PORTF_INT_PRI;
	
	
	/* Configure Falling Edge Sensitive Interrupt on PB4 */
	GPIO_PORTB_IM_R  &=  0x00;

	GPIO_PORTB_IS_R &= (~PORTB_PIN4);
	GPIO_PORTB_IEV_R &= (~PORTB_PIN4);

	
	GPIO_PORTB_IM_R  |= PORTB_PIN4;
	
	/* Configure Falling Edge Sensitive Interrupt on PF4 */
	GPIO_PORTF_IM_R  &=  0x00;

	GPIO_PORTF_IS_R &= (~PORTF_PIN4);
	GPIO_PORTF_IEV_R &= (~PORTF_PIN4);

	GPIO_PORTF_IM_R  |= PORTF_PIN4 ;
	
	/* Globally enable all interrupts and set base priority */
	EnablePriorityInterrupts();
	EnableInterrupts();
}

/*************************************************************************
* ISR for Port B
*************************************************************************/
void GPIOB_Handler(void)
{
	/* Clear the interrupt flag */
	GPIO_PORTB_ICR_R |=  PORTB_PIN4;
	
	/* Turn off RED LED */
    GPIO_PORTF_DATA_R &= ~0x02;

	/*Turn on GREEN LED */
    GPIO_PORTF_DATA_R |= 0x08;
    

	
}

/*************************************************************************
* ISR for Port F
*************************************************************************/
void GPIOF_Handler(void)
{
	
	/* Clear the interrupt flag */
	GPIO_PORTF_ICR_R |=  PORTF_PIN4;
	
	/* Turn off GREEN LED */
    GPIO_PORTF_DATA_R &= (~0x08);

	/* Turn on RED LED */
    GPIO_PORTF_DATA_R |= 0x02;
    

	
}

/*************************************************************************
* Main Function which waits for interrupt
*************************************************************************/
int main(){
	Init_INT_GPIO();
	while(1)
	{
		//WaitForInterrupt();
	}
}

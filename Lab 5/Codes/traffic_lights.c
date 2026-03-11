/******************************************************************************
* Project name: Sohaib Liaquat
* File name: l7_traffic_light_interrupts
* Author: Ali Maaz
* Date: 4 Nov 2025
* Description: This is a file for Multiple Interrupt configuration for
* User buttons PF4(SW1), PF0(SW2) and external PB4.
* PF4 -> RED_LED (PF1)
* PB4 -> GREEN_LED (PF3)
* PF0 -> BLUE_LED (PF2)
******************************************************************************/

#include "tm4c123.h"

/********************************************************************
* Macro definitions and Function Declarations for Pending Interrupts
********************************************************************/

/* Register for providing GPIO clock */
#define 	SYSCTL_RCGCGPIO_R		(*((volatile unsigned long*)0x400FE608))

/* Register Definitions for Port B */
/* NVIC Registers */
#define 	NVIC_EN0_R				(*((volatile unsigned long*)0xE000E100))
#define 	NVIC_PRI0_R				(*((volatile unsigned long*)0xE000E400))
/* GPIO Registers */
#define 	GPIO_PORTB_DEN_R		(*((volatile unsigned long*)0x4000551C))
#define 	GPIO_PORTB_PUR_R		(*((volatile unsigned long*)0x40005510))
#define 	GPIO_PORTB_IM_R			(*((volatile unsigned long*)0x40005410))
#define 	GPIO_PORTB_IS_R			(*((volatile unsigned long*)0x40005404))
#define 	GPIO_PORTB_IBE_R		(*((volatile unsigned long*)0x40005408))
#define 	GPIO_PORTB_IEV_R		(*((volatile unsigned long*)0x4000540C))
#define 	GPIO_PORTB_ICR_R		(*((volatile unsigned long*)0x4000541C))
#define 	GPIO_PORTB_DIR_R		(*((volatile unsigned long*)0x40005400))
#define 	GPIO_PORTB_DATA_R		(*((volatile unsigned long*)0x400053FC))

/* Register Definitions for Port F */
/* NVIC Registers */
#define 	NVIC_EN0_R				(*((volatile unsigned long*)0xE000E100))
#define 	NVIC_PRI7_R 			(*((volatile unsigned long*)0xE000E41C))

/* GPIO Registers */
#define 	GPIO_PORTF_LOCK_R		(*((volatile unsigned long*)0x40025520))
#define 	GPIO_PORTF_CR_R			(*((volatile unsigned long*)0x40025524))
#define 	GPIO_PORTF_RIS_R		(*((volatile unsigned long*)0x40025414))
#define 	GPIO_PORTF_DEN_R		(*((volatile unsigned long*)0x4002551C))
#define 	GPIO_PORTF_PUR_R		(*((volatile unsigned long*)0x40025510))
#define 	GPIO_PORTF_IM_R			(*((volatile unsigned long*)0x40025410))
#define 	GPIO_PORTF_IS_R			(*((volatile unsigned long*)0x40025404))
#define 	GPIO_PORTF_IBE_R		(*((volatile unsigned long*)0x40025408))
#define 	GPIO_PORTF_IEV_R		(*((volatile unsigned long*)0x4002540C))
#define 	GPIO_PORTF_ICR_R		(*((volatile unsigned long*)0x4002541C))
#define 	GPIO_PORTF_DIR_R		(*((volatile unsigned long*)0x40025400))
#define 	GPIO_PORTF_DATA_R		(*((volatile unsigned long*)0x400253FC))

/* Values for Port B*/
#define 	PORTB_CLK 				0x02
#define 	PORTB_INT 				0x00000002    // Interrupt 1
#define 	PORTB_INT_PRI 			0x0000E000    // Priority 7 (Lowest) <-- CHANGED
#define 	PORTB_PIN4 				0x10

/* Values for Port F*/
#define 	PORTF_CLK 				0x20
#define 	PORTF_INT 				0x40000000    // Interrupt 30
#define 	PORTF_INT_PRI 			0x00A00000    // Priority 5 (Higher) <-- CHANGED
#define 	PORTF_PIN4 				0x10          // SW1
#define 	PORTF_PIN0 				0x01          // SW2 (Locked)  // <-- NEW
#define		PORTF_LEDS				0x0E          // PF1(Red)+PF2(Blue)+PF3(Green) <-- CHANGED
#define 	RED_LED					0x02          // <-- NEW
#define 	BLUE_LED				0x04          // <-- NEW
#define 	GREEN_LED				0x08          // <-- NEW

/* Function Declarations */
void EnableInterrupts(void);
void DisableInterrupts(void);
void EnablePriorityInterrupts(void);
void Init_INT_GPIO(void);
void Delay(unsigned long value);
void WaitForInterrupt(void);

void SystemInit (void)
{
	/* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) | /* set CP10 Full Access */
                  (3UL << 11*2) ); /* set CP11 Full Access */
	#endif
}

/*************************************************************************
* Delay Function
*************************************************************************/
void Delay(unsigned long value){
	volatile unsigned long i=0; // Correctly volatile
	for(i=0;i<value;i++);
}

/*************************************************************************
* Port B and Port F NVIC and Peripheral Level Configurations
*************************************************************************/
void Init_INT_GPIO(void)
{
	volatile unsigned long dummy;

	/* Enable clock for Port B & F */
	SYSCTL_RCGCGPIO_R |= (PORTB_CLK | PORTF_CLK); // <-- CHANGED (Combined)

	/* Dummy read to stable the clock */
	dummy = SYSCTL_RCGCGPIO_R;
	
	/* --- Port F (PF0, PF4, LEDs) Configuration --- */
	
	// 1. Unlock PF0 (SW2)
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // Magic unlock value
	GPIO_PORTF_CR_R |= (PORTF_PIN0 | PORTF_PIN4); // Allow changes to PF0 and PF4
	
	// 2. Configure Pin Directions
	GPIO_PORTF_DIR_R |= PORTF_LEDS;   // Set LEDs (PF1,2,3) as output
	GPIO_PORTF_DIR_R &= ~(PORTF_PIN0 | PORTF_PIN4); // Set switches (PF0,4) as input
	
	// 3. Configure Pin Properties
	GPIO_PORTF_DEN_R |= (PORTF_LEDS | PORTF_PIN0 | PORTF_PIN4); // Digital enable all
	GPIO_PORTF_PUR_R |= (PORTF_PIN0 | PORTF_PIN4); // Enable pull-up for switches
	
	// 4. (Optional) Re-lock the port
	// GPIO_PORTF_LOCK_R = 0;
	
	
	/* --- Port B (PB4) Configuration --- */
	
	GPIO_PORTB_DEN_R |= PORTB_PIN4;
	GPIO_PORTB_PUR_R |= PORTB_PIN4;
	GPIO_PORTB_DIR_R &= (~PORTB_PIN4); // As an input from outer switch
	
	
	/* --- INTERRUPT Configuration --- */
	
	DisableInterrupts(); // Globally disable
	
	/* Configure Port B (Interrupt 1, Priority 7) */
	NVIC_EN0_R |= PORTB_INT;
	NVIC_PRI0_R |= PORTB_INT_PRI; // Note: Priority 7 is 0xE0, not 0x80

	/* Configure Port F (Interrupt 30, Priority 5) */
	NVIC_EN0_R |= PORTF_INT;
	NVIC_PRI7_R |= PORTF_INT_PRI;
	
	
	/* Configure Falling Edge Interrupt on PB4 */
	GPIO_PORTB_IM_R  &= ~PORTB_PIN4;   // 1. Mask
	GPIO_PORTB_IS_R  &= (~PORTB_PIN4); // 2. Edge-sensitive
	GPIO_PORTB_IBE_R &= (~PORTB_PIN4); // 3. Not both edges
	GPIO_PORTB_IEV_R &= (~PORTB_PIN4); // 4. Falling edge
	GPIO_PORTB_ICR_R  = PORTB_PIN4;    // 5. Clear prior flag
	GPIO_PORTB_IM_R  |= PORTB_PIN4;    // 6. Unmask
	
	/* Configure Falling Edge Interrupts on PF0 AND PF4 */
	GPIO_PORTF_IM_R  &= ~(PORTF_PIN0 | PORTF_PIN4); // 1. Mask
	GPIO_PORTF_IS_R  &= ~(PORTF_PIN0 | PORTF_PIN4); // 2. Edge-sensitive
	GPIO_PORTF_IBE_R &= ~(PORTF_PIN0 | PORTF_PIN4); // 3. Not both edges
	GPIO_PORTF_IEV_R &= ~(PORTF_PIN0 | PORTF_PIN4); // 4. Falling edge
	GPIO_PORTF_ICR_R  = (PORTF_PIN0 | PORTF_PIN4);  // 5. Clear prior flags
	GPIO_PORTF_IM_R  |= (PORTF_PIN0 | PORTF_PIN4);  // 6. Unmask
	
	/* Globally enable all interrupts and set base priority */
	EnablePriorityInterrupts(); // Sets BASEPRI to 0
	EnableInterrupts();         // Clears PRIMASK
}

/*************************************************************************
* ISR for Port B (External Switch PB4)
*************************************************************************/
void GPIOPortB_Handler(void)
{
	/* Clear the interrupt flag */
	GPIO_PORTB_ICR_R |= PORTB_PIN4;
	
	/* Turn on GREEN LED */
	GPIO_PORTF_DATA_R |= GREEN_LED;
	/* Turn off RED and BLUE LEDs */
	GPIO_PORTF_DATA_R &= ~(RED_LED | BLUE_LED);
}

/*************************************************************************
* ISR for Port F (SW1 or SW2)
*************************************************************************/
void GPIOPortF_Handler(void)
{
	// Check which switch caused the interrupt
	
	if( GPIO_PORTF_RIS_R & PORTF_PIN4 ) // Was it SW1 (PF4)?
	{
		/* Clear the interrupt flag for PF4 */
		GPIO_PORTF_ICR_R |= PORTF_PIN4;
		
		/* Turn on RED LED */
		GPIO_PORTF_DATA_R |= RED_LED;
		/* Turn off GREEN and BLUE LEDs */
		GPIO_PORTF_DATA_R &= ~(GREEN_LED | BLUE_LED);
	}
	
	if( GPIO_PORTF_RIS_R & PORTF_PIN0 ) // Was it SW2 (PF0)?
	{
		/* Clear the interrupt flag for PF0 */
		GPIO_PORTF_ICR_R |= PORTF_PIN0;
		
		/* Turn on BLUE LED */
		GPIO_PORTF_DATA_R |= BLUE_LED;
		/* Turn off GREEN and RED LEDs */
		GPIO_PORTF_DATA_R &= ~(GREEN_LED | RED_LED);
	}
}

/*************************************************************************
* Main Function which waits for interrupt
*************************************************************************/
int main(){
	Init_INT_GPIO();
	
	// Turn off all LEDs to start
	GPIO_PORTF_DATA_R &= ~PORTF_LEDS;
	
	while(1)
	{
		WaitForInterrupt(); // Go to sleep and wait for a button
	}
}
/******************************************************************************
*	Project name:
*	File name:
*	Author:
*	Date:
*	Description: This is a template file for Multiple Interrupt configuration for 
*                User button connected to PF4 and PB4. It turns on Red LED for
*                PF4 and Green LED for PB4.
 ******************************************************************************/

/********************************************************************
* Macro definitions and Function Declarations for Pending Interrupts
********************************************************************/

/* Register for providing GPIO clock */
#define 		SYSCTL_RCGCGPIO_R		(*((volatile unsigned long*)0x))

/* Register Definitions for Port B */
/* NVIC Registers */
#define 	NVIC_EN _R					(*((volatile unsigned long*)0x))
#define 	NVIC_PRI _R					(*((volatile unsigned long*)0x))
/* GPIO Registers */
#define 	GPIO_PORTB_DEN_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTB_PUR_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTB_IM_R				(*((volatile unsigned long*)0x))
#define 	GPIO_PORTB_IS_R				(*((volatile unsigned long*)0x))
#define 	GPIO_PORTB_IBE_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTB_IEV_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTB_ICR_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTB_DIR_R			(*((volatile unsigned long*)0x))

/* Register Definitions for Port F */
/* NVIC Registers */
#define 	NVIC_EN _R					(*((volatile unsigned long*)0x))
#define 	NVIC_PRI 
_R					(*((volatile unsigned long*)0x))
/* GPIO Registers */
#define 	GPIO_PORTF_DEN_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_PUR_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_IM_R				(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_IS_R				(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_IBE_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_IEV_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_ICR_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_DIR_R			(*((volatile unsigned long*)0x))
#define 	GPIO_PORTF_DATA_R			(*((volatile unsigned long*)0x))

/* Values for Port B*/
#define 	PORTB_CLK 					0x
#define 	PORTB_INT 					0x
#define 	PORTB_INT_PRI 				0x
#define 	PORTB_PIN4 					0x

/* Values for Port F*/
#define 	PORTF_CLK 					0x
#define 	PORTF_INT 					0x
#define 	PORTF_INT_PRI 				0x
#define 	PORTF_PIN4 					0x
#define		PORTF_LEDS					0x

/* Function Declarations */
void EnableInterrupts(void);								// Enable interrupts
void DisableInterrupts(void);								// Disable interrupts
void EnablePriorityInterrupts(void);						//Enable Priortized Interrupts
void Init_INT_GPIO(void);									// GPIO and Interrupt Initializations
void Delay(unsigned long value);							// Implements delay
void WaitForInterrupt(void);								// Implements WFI

/*************************************************************************
* Delay Function
*************************************************************************/
void Delay(unsigned long value){
	unsigned long i=0;
	for(i=0;i<value;i++);
}

/*************************************************************************
* Port B and Port F NVIC and Peripheral Level Configurations
*************************************************************************/
void Init_INT_GPIO(void)
{
	/* Enable clock for Port B & F */
	
	
	/* Dummy read to stable the clock */
	
	
	/* Enable digital I/O on PB4, enable pull up and set its direction */
	
	
	
	
	/* Enable digital I/O on PF4-PF1, enable pull up on PF4 and set their direction */
	
	
	
	
	/* INTERRUPT Configuration */
	/* Globally disable all interrupts */
	DisableInterrupts();
	/* Enable interrupt on Port B and set its priority to 4 */
	
	
	
	/* Enable interrupt on Port F and set its priority to 1 */
	
	
	
	/* Configure Falling Edge Sensitive Interrupt on PB4 */
	GPIO_PORTB_IM_R  &=  0x00;
	
	
	
	GPIO_PORTB_IM_R  |=  ;
	
	/* Configure Falling Edge Sensitive Interrupt on PF4 */
	GPIO_PORTF_IM_R  &=  0x00;
	
	
	
	
	GPIO_PORTF_IM_R  |=  ;
	
	/* Globally enable all interrupts and set base priority */
	EnablePriorityInterrupts();
	EnableInterrupts();
}

/*************************************************************************
* ISR for Port B
*************************************************************************/
void GPIOPortB_Handler(void)
{
	/* Clear the interrupt flag */
	GPIO_PORTB_ICR_R |=  PORTB_PIN4;
	
	/* Turn on GREEN LED */
	
}

/*************************************************************************
* ISR for Port F
*************************************************************************/
void GPIOPortF_Handler(void)
{
	
	/* Clear the interrupt flag */
	GPIO_PORTF_ICR_R |=  PORTF_PIN4;
	
	/* Turn on RED LED */
	
}

/*************************************************************************
* Main Function which waits for interrupt
*************************************************************************/
int main(){
	Init_INT_GPIO();
	while(1)
	{
		WaitForInterrupt();
	}
}
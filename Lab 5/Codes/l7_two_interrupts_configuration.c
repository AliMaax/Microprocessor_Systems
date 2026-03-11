#include "TM4C123.h"

/* Register for providing GPIO clock */
#define 	SYSCTL_RCGCGPIO_R		(*((volatile unsigned long*)0x400FE608))
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

#define 	PORTF_INT 				0x40000000    // Interrupt 30
#define 	PORTF_INT_PRI 			0x0000E000    // Priority 7 (lower)

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


// Define LED Pins on Port B
#define LED1_PIN 0x04 // PB2
#define LED2_PIN 0x08 // PB3

// Define Switch Pins on Port F
// NOTE: PF0 requires unlocking on the Tiva C Launchpad
#define SW1_PIN  0x10 // PF4
#define SW2_PIN  0x01 // PF0 

// Priority definitions (0 is highest, 7 is lowest)
#define LOW_PRIORITY 0x07 // Use the lowest possible priority

void EnableInterrupts(void);
void DisableInterrupts(void);
void EnablePriorityInterrupts(void);
void Init_INT_GPIO(void);
void Delay(unsigned long value);
void WaitForInterrupt(void);


void Init_INT_GPIO(void) {
    volatile unsigned long delay;

    // 1. Enable Clocks for Port B (LEDs) and Port F (Switches)
    SYSCTL_RCGCGPIO_R |= 0x02; // Enable Port B clock
    SYSCTL_RCGCGPIO_R |= 0x20; // Enable Port F clock
    delay = SYSCTL_RCGCGPIO_R; // Dummy read for delay

    // 2. Unlock PF0 (required for the Tiva C Launchpad switch)
    GPIO_PORTF_LOCK_R = 0x4C4F434B; 
    GPIO_PORTF_CR_R |= 0x01;       // Allow changes to PF0

    // 3. Configure LEDs (Port B) as Output
    GPIO_PORTB_DIR_R |= (LED1_PIN | LED2_PIN); // Set PB2, PB3 as outputs
    GPIO_PORTB_DEN_R |= (LED1_PIN | LED2_PIN); // Enable digital function

    // 4. Configure Switches (Port F) as Input & Pull-Up
    GPIO_PORTF_DIR_R &= ~(SW1_PIN | SW2_PIN); 
    GPIO_PORTF_DEN_R |= (SW1_PIN | SW2_PIN);  
    GPIO_PORTF_PUR_R |= (SW1_PIN | SW2_PIN);  
    GPIO_PORTF_IS_R &= ~(SW1_PIN | SW2_PIN);  
    GPIO_PORTF_IEV_R &= ~(SW1_PIN | SW2_PIN); 
    GPIO_PORTF_ICR_R |= (SW1_PIN | SW2_PIN);  
    
    GPIO_PORTF_IM_R |= (SW1_PIN | SW2_PIN); 

	DisableInterrupts(); // Globally disable
 
	NVIC_EN0_R |= PORTF_INT;
	NVIC_PRI7_R |= PORTF_INT_PRI;
	/* Globally enable all interrupts and set base priority */
	EnablePriorityInterrupts(); // Sets BASEPRI to 0
	EnableInterrupts();         // Clears PRIMASK
  
}
void GPIOF_Handler(void) {

    // Turn ON LED1 (PB2)
    GPIO_PORTB_DATA_R |= LED1_PIN; // Set PB2 HIGH
    
    // Turn OFF LED2 (PB3)
    GPIO_PORTF_DATA_R &= ~LED2_PIN; // Set PB3 LOW

    // Clear the interrupt flags
    if (GPIO_PORTF_RIS_R & SW1_PIN) {
        // SW1 (PF4) caused the interrupt
        GPIO_PORTF_ICR_R  |= SW1_PIN; 
    }
    
    if (GPIO_PORTF_RIS_R & SW2_PIN) {
        // SW2 (PF0) caused the interrupt
        GPIO_PORTB_ICR_R |= SW2_PIN; 
    }
}		
int main(){
     
    // Configure switches and LEDs
    Init_INT_GPIO(); 
    
    // Loop forever
    while (1) {
        
    }
}

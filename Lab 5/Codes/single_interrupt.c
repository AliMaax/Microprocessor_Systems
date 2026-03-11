/******************************************************************************
*   Project name: Lab 7 - Interrupts
*   File name: l7_interrupt_cycle.c
*   Author: Ali Maaz
*   Date: 4 Nov 2025
*   Description: This is a template file for Interrupt configuration for 
*                User button connected to PF4. It cycles different LEDs
*                on falling edge of button press
 ******************************************************************************/

#include "TM4C123.h"
/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/

/* Register for GPIO clock */
#define SYSCTL_RCGCGPIO_R   (*((volatile unsigned long*)0x400FE608))

/* Enable Register for PortF Interrupt (Interrupt 30) */
#define NVIC_EN0_R          (*((volatile unsigned long*)0xE000E100))

/* Priority Register for PortF Interrupt (Interrupt 30 is in PRI7) */
#define NVIC_PRI7_R         (*((volatile unsigned long*)0xE000E41C))

/* Port F GPIO Registers */
#define GPIO_PORTF_DATA_R   (*((volatile unsigned long*)0x400253FC))
#define GPIO_PORTF_DIR_R    (*((volatile unsigned long*)0x40025400))
#define GPIO_PORTF_DEN_R    (*((volatile unsigned long*)0x4002551C))
#define GPIO_PORTF_PUR_R    (*((volatile unsigned long*)0x40025510))
#define GPIO_PORTF_IM_R     (*((volatile unsigned long*)0x40025410))
#define GPIO_PORTF_IS_R     (*((volatile unsigned long*)0x40025404))
#define GPIO_PORTF_IBE_R    (*((volatile unsigned long*)0x40025408))
#define GPIO_PORTF_IEV_R    (*((volatile unsigned long*)0x4002540C))
#define GPIO_PORTF_ICR_R    (*((volatile unsigned long*)0x4002541C))

/* --- Bit-specific values --- */
#define PORTF_CLK_EN        0x20        // Bit 5 for Port F
#define INT_PORTF_EN        0x40000000  // Bit 30 (for Int 30)
#define PRIORITY_5          0x00A00000  // (5 << 21) for PRI7[23:21]
#define INT_PF4             0x10        // Bit 4 for PF4 (SW1)
#define LED_RED             0x02        // Bit 1 for PF1
#define LED_BLUE            0x04        // Bit 2 for PF2
#define LED_GREEN           0x08        // Bit 3 for PF3
#define LEDS_ALL            (LED_RED | LED_BLUE | LED_GREEN) // 0x0E

/* Function Declarations */ 
/*-----These are functions added in startup file-----*/
void EnableInterrupts(void);
void DisableInterrupts(void);                                 
void EnablePriorityInterrupts(void);
void WaitForInterrupt(void);                         

/*----These are C frunctions of this file-------*/
void Init_INT_GPIO(void);
void Delay(unsigned long value);

/* * Global Variable 
 * This stores the "state" of the traffic light.
 * It's 1, 2, or 3 for Red, Green, Blue.
 */
volatile unsigned long i = 1; // Start with state 1 (Red)

//System Initialization for Floating Point Unit
void SystemInit (void)
{
      /* --------------------------FPU settings ----------------------------------*/
    #if (__FPU_USED == 1)
        SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
    #endif
}


void Init_INT_GPIO(void){
    volatile unsigned long delay_clk;
    
    /* Enable clock for PORTF */
    SYSCTL_RCGCGPIO_R |= PORTF_CLK_EN;

    /* Dummy read to stable the clock */    
    delay_clk = SYSCTL_RCGCGPIO_R;      
    
    
    /* Enable digital I/O on PF4 (switch) & PF3-PF1 (LEDs) */
    GPIO_PORTF_DEN_R |= (INT_PF4 | LEDS_ALL);
    
    /* Make PF4 input and PF3-PF1 output */
    GPIO_PORTF_DIR_R &= ~INT_PF4;
    GPIO_PORTF_DIR_R |= LEDS_ALL;
    
    /* Enable weak pullup on PF4 */
    GPIO_PORTF_PUR_R |= INT_PF4;
    
    
    /* INTERRUPT Configuration */
    /* Globally disable all interrupts */
    DisableInterrupts();
    
    /* Disable all interrupts on PortF (good practice) */
    GPIO_PORTF_IM_R = 0x00; // Use '=' for clarity
    
    /* Enable PortF interrupt in NVIC (Int 30) */
    NVIC_EN0_R |= INT_PORTF_EN;
    
    /* Set priority of PortF as 5 */
    // This is the "safe" way: Read, clear bits, set bits
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF0FFFFF) | PRIORITY_5;
    
    
    /* Configure PF4 as edge sensitive,
    * not both edges,
    * falling edge */
    GPIO_PORTF_IS_R &= ~INT_PF4;
    GPIO_PORTF_IBE_R &= ~INT_PF4;
    GPIO_PORTF_IEV_R &= ~INT_PF4;
    
    
s    /* Clear interrupt flag for PF4 */
    GPIO_PORTF_ICR_R = INT_PF4; // Use '=' to clear 
    
    /* Enable interrupt on PF4 */
    GPIO_PORTF_IM_R |= INT_PF4;    

    /* Enable interrupts beyond the defined priority level */
    EnablePriorityInterrupts();
    /* Globally enable all interrupts */
    EnableInterrupts();
}


void Delay(unsigned long value){
    // BUG FIX: 'i' must be volatile or the compiler deletes this loop
    volatile unsigned long i = 0;
    for(i=0;i<value;i++);
}

/* * Interrupt Service Routine for PortF 
 * NOTE: Name must match your startup file (e.g., GPIOPortF_Handler)
 */
void GPIOPortF_Handler(void){ // <-- Check this name!
    
    /* BUG FIX: Clear the interrupt flag *first* */
    GPIO_PORTF_ICR_R = INT_PF4;
    
    /*      * BUG FIX: NEVER use Delay() in an ISR.
     * This new logic just cycles the state and sets the LED.
     */
    
    /* Cycle to the next state (1=Red, 2=Green, 3=Blue) */
    if(i >= 3)
    {
        i = 1;
    }
    else
    {
        i++;
    }

    /* * BUG FIX: Don't use XOR. Set the correct LED
     * and turn the others off.
     */
    if (i == 1) { // State 1: Red ON
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LEDS_ALL) | LED_RED;
    }
    else if (i == 2) { // State 2: Green ON
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LEDS_ALL) | LED_GREEN;
g    }
    else { // State 3: Blue ON
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LEDS_ALL) | LED_BLUE;
    }
}

/* Main Function */
int main(){
    Init_INT_GPIO();
    
    // Set the initial state (Red LED ON)
    GPIO_PORTF_DATA_R = LED_RED;
    
    while(1)
    {
        // Go to sleep and wait for the button press
        WaitForInterrupt();
    }
}
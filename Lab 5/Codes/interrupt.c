//#include "TM4C123.h"

/* -------- Register Addresses -------- */
#define SYSCTL_RCGCGPIO_R   (*((volatile unsigned long*)0x400FE608))

#define GPIO_PORTF_DIR_R    (*((volatile unsigned long*)0x40025400))
#define GPIO_PORTF_AFSEL_R  (*((volatile unsigned long*)0x40025420))
#define GPIO_PORTF_PUR_R    (*((volatile unsigned long*)0x40025510))
#define GPIO_PORTF_DEN_R    (*((volatile unsigned long*)0x4002551C))
#define GPIO_PORTF_LOCK_R   (*((volatile unsigned long*)0x40025520))
#define GPIO_PORTF_CR_R     (*((volatile unsigned long*)0x40025524))
#define GPIO_PORTF_IS_R     (*((volatile unsigned long*)0x40025404))
#define GPIO_PORTF_IBE_R    (*((volatile unsigned long*)0x40025408))
#define GPIO_PORTF_IEV_R    (*((volatile unsigned long*)0x4002540C))
#define GPIO_PORTF_ICR_R    (*((volatile unsigned long*)0x4002541C))
#define GPIO_PORTF_IM_R     (*((volatile unsigned long*)0x40025410))
#define GPIO_PORTF_MIS_R    (*((volatile unsigned long*)0x40025418))
#define GPIO_PORTF_DATA_R   (*((volatile unsigned long*)0x400253FC))

#define NVIC_EN0_R          (*((volatile unsigned long*)0xE000E100))
#define NVIC_PRI7_R         (*((volatile unsigned long*)0xE000E41C))

#define GPIO_LOCK_KEY       0x4C4F434B

void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}
	

void GPIOF_Handler(void);

int main(void)
{
    volatile unsigned long delay;

    /* Enable clock for Port F */
    SYSCTL_RCGCGPIO_R |= 0x20;
    delay = SYSCTL_RCGCGPIO_R;

    /* Unlock PF0 */
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R  |= 0x01;

    /* Disable alternate functions */
    GPIO_PORTF_AFSEL_R &= ~0x1D;     // PF0, PF2, PF3, PF4

    /* Enable digital function for PF0, PF2, PF3, PF4 */
    GPIO_PORTF_DEN_R |= 0x1D;

    /* Set PF2, PF3 as outputs; PF0, PF4 as inputs */
    GPIO_PORTF_DIR_R |= 0x0C;        // outputs
    GPIO_PORTF_DIR_R &= ~0x11;       // inputs

    /* Enable pull-ups for PF0, PF4 */
    GPIO_PORTF_PUR_R |= 0x11;

    /* Configure for edge-triggered on falling edge */
    GPIO_PORTF_IS_R  &= ~0x11;       // edge-sensitive
    GPIO_PORTF_IBE_R &= ~0x11;       // not both edges
    GPIO_PORTF_IEV_R &= ~0x11;       // falling edge

    /* Clear any prior interrupts and unmask PF0, PF4 */
    GPIO_PORTF_ICR_R |= 0x11;
    GPIO_PORTF_IM_R  |= 0x11;

    /* Enable Port F interrupt in NVIC */
    NVIC_EN0_R  |= 0x40000000;       // bit 30 = Port F
    /* Set priority = 3 */
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00600000;

    while(1)
    {
        // MCU waits here; LEDs change only when interrupt occurs
    }
}


/* ------------ Interrupt Service Routine ------------ */
void GPIOF_Handler(void)
{
    unsigned long status;
    status = GPIO_PORTF_MIS_R;       // which pin caused interrupt

    if (status & 0x01)               // PF0
    {
        GPIO_PORTF_ICR_R |= 0x01;    // clear flag
        GPIO_PORTF_DATA_R = 0x04;    // Blue ON
        GPIO_PORTF_DATA_R &= ~0x08;  // Green OFF
    }

    if (status & 0x10)               // PF4
    {
        GPIO_PORTF_ICR_R |= 0x10;    // clear flag
        GPIO_PORTF_DATA_R = 0x08;    // Green ON
        GPIO_PORTF_DATA_R &= ~0x04;  // Blue OFF
    }
}

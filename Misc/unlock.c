/* --- Unlock Sequence for PF0 --- */

// 1. Enable Clock for Port F (if not already done)
SYSCTL_RCGCGPIO_R |= 0x20;
// Dummy read for delay
volatile unsigned long delay = SYSCTL_RCGCGPIO_R;

// 2. Write the "magic number" to the LOCK register to unlock
GPIO_PORTF_LOCK_R = 0x4C4F434B;

// 3. Set the bit for PF0 in the COMMIT register to allow changes
GPIO_PORTF_CR_R |= 0x01;

/* --- Standard GPIO Configuration (Now that PF0 is unlocked) --- */

// 4. Set Direction as input
GPIO_PORTF_DIR_R &= ~0x01; 

// 5. Disable Alternate Function (to use it as GPIO)
GPIO_PORTF_AFSEL_R &= ~0x01;

// 6. Enable Digital Function
GPIO_PORTF_DEN_R |= 0x01;

// 7. Enable the Internal Pull-Up Resistor (IMPORTANT for a switch)
// The on-board switch is connected to ground, so it needs a pull-up.
GPIO_PORTF_PUR_R |= 0x01;

// 8. (Optional but good practice) Re-lock the port
GPIO_PORTF_LOCK_R = 0; 

/* --- Interrupt Configuration (can now be done) --- */

// 9. Configure for a falling edge (press)
GPIO_PORTF_IS_R &= ~0x01;   // Edge-sensitive
GPIO_PORTF_IBE_R &= ~0x01;  // Not both edges
GPIO_PORTF_IEV_R &= ~0x01;  // Falling edge

// 10. Clear any prior interrupt flag
GPIO_PORTF_ICR_R = 0x01;

// 11. Unmask the interrupt for PF0
GPIO_PORTF_IM_R |= 0x01;

// 12. Enable in NVIC (Interrupt 30) and enable globally...
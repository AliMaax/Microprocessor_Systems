/**************************
* Project:      Automatic Whiteboard Duster (Top Return Logic)
* MCU:          Tiva C Series (TM4C123GH6PM)
* Logic:        Up -> Right -> Down -> Repeat
* End Logic:    Finish Loop -> Go UP -> Go LEFT (x4) -> Go DOWN -> Stop
**************************/

#include <stdint.h>
#include "TM4C123.h"

/* ============================================================================
                               SYSTEM INIT
   ============================================================================ */
void SystemInit (void)
{
#if (__FPU_USED == 1)
    // Enables Floating Point Unit (System Control Block, Page 232)
    SCB->CPACR |= ((3UL << 20) | (3UL << 22));
#endif
}

/* ============================================================================
                         SYSTEM CONTROL REGISTERS
   (Clock Gating & Peripheral Enable — Datasheet Chapter 6)
   ============================================================================ */
#define SYSCTL_RCGCGPIO_R   (*((volatile unsigned long *)0x400FE608)) // Page 340
#define SYSCTL_RCGCPWM_R    (*((volatile unsigned long *)0x400FE640)) // Page 354
#define SYSCTL_RCGCTIMER_R  (*((volatile unsigned long *)0x400FE604)) // Page 338
#define SYSCTL_PRGPIO_R     (*((volatile unsigned long *)0x400FEA08)) // Page 406

/* ============================================================================
                             GPIO PORT A
                     (Motor Direction Control Pins)
   ============================================================================ */
#define GPIO_PORTA_DATA_R   (*((volatile unsigned long *)0x400043FC)) // Page 662
#define GPIO_PORTA_DIR_R    (*((volatile unsigned long *)0x40004400)) // Page 663
#define GPIO_PORTA_DEN_R    (*((volatile unsigned long *)0x4000451C)) // Page 682

/* ============================================================================
                             GPIO PORT D
                        (PWM Output Pins PD0, PD1)
   ============================================================================ */
#define GPIO_PORTD_AFSEL_R  (*((volatile unsigned long *)0x40007420)) // Page 671
#define GPIO_PORTD_PCTL_R   (*((volatile unsigned long *)0x4000752C)) // Page 688
#define GPIO_PORTD_DEN_R    (*((volatile unsigned long *)0x4000751C)) // Page 682

/* ============================================================================
                              PWM MODULE 1
                    (Speed Control for DC Motors)
   ============================================================================ */
#define PWM1_ENABLE_R       (*((volatile unsigned long *)0x40029008)) // Page 1247
#define PWM1_0_CTL_R        (*((volatile unsigned long *)0x40029040)) // Page 1266
#define PWM1_0_LOAD_R       (*((volatile unsigned long *)0x40029050)) // Page 1278
#define PWM1_0_CMPA_R       (*((volatile unsigned long *)0x40029058)) // Page 1280
#define PWM1_0_CMPB_R       (*((volatile unsigned long *)0x4002905C)) // Page 1281
#define PWM1_0_GENA_R       (*((volatile unsigned long *)0x40029060)) // Page 1282
#define PWM1_0_GENB_R       (*((volatile unsigned long *)0x40029064)) // Page 1285

/* ============================================================================
                                TIMER 0
                        (Motion Timing & Delays)
   ============================================================================ */
#define TIMER0_CFG_R        (*((volatile unsigned long *)0x40030000)) // Page 727
#define TIMER0_TAMR_R       (*((volatile unsigned long *)0x40030004)) // Page 729
#define TIMER0_CTL_R        (*((volatile unsigned long *)0x4003000C)) // Page 737
#define TIMER0_RIS_R        (*((volatile unsigned long *)0x4003001C)) // Page 748
#define TIMER0_ICR_R        (*((volatile unsigned long *)0x40030024)) // Page 754
#define TIMER0_TAILR_R      (*((volatile unsigned long *)0x40030028)) // Page 756

/* ============================================================================
                             CONFIGURATION
   ============================================================================ */
#define TOTAL_PASSES        4           // Number of cleaning cycles
#define PWM_DUTY_3V         800         // Slow speed (horizontal)
#define PWM_DUTY_7V         1000        // Fast speed (vertical)

#define TIME_VERTICAL       16000000    // Time for UP / DOWN motion   1.75 s
#define TIME_HORIZONTAL     2500000     // Time for LEFT / RIGHT step  0.15 s
#define TIME_PAUSE          8000000     // Pause between motions

/* ============================================================================
                          MOTOR DRIVER PINS (PORT A)
   ============================================================================ */
#define PA2_IN1 (*((volatile unsigned long *)0x40004010)) // Motor A IN1
#define PA3_IN2 (*((volatile unsigned long *)0x40004020)) // Motor A IN2
#define PA4_IN3 (*((volatile unsigned long *)0x40004040)) // Motor B IN3
#define PA5_IN4 (*((volatile unsigned long *)0x40004080)) // Motor B IN4

/* ============================================================================
                          STATE MACHINE DEFINITIONS
   ============================================================================ */
typedef enum {
    STATE_MOVE_UP,
    STATE_STEP_RIGHT,
    STATE_MOVE_DOWN,
    STATE_RETURN_HOME,
    STATE_HALT
} SystemState_t;

/* ============================================================================
                          MOTOR DIRECTION DEFINITIONS
   ============================================================================ */
typedef enum {
    STOP = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction_t;

/* ============================================================================
                           FUNCTION PROTOTYPES
   ============================================================================ */
void Hardware_Init(void);
void PWM_Init(void);
void Timer0_Init(void);
void Timer_Wait(unsigned long ticks);
void Motor_Drive(Direction_t dir);

/* ============================================================================
                                  MAIN
   ============================================================================ */
int main(void)
{
    SystemState_t state = STATE_MOVE_UP;
    int pass = 0;
    int i;

    Hardware_Init();   // Enable GPIO and motor pins
    PWM_Init();        // Configure PWM for speed control
    Timer0_Init();     // Configure Timer0 for delays

    Timer_Wait(TIME_PAUSE); // Initial stabilization delay

    while(1)
    {
        switch(state)
        {
            case STATE_MOVE_UP:
                Motor_Drive(DIR_UP);
                Timer_Wait(TIME_VERTICAL);
                Motor_Drive(STOP);
                Timer_Wait(TIME_PAUSE);
                state = STATE_STEP_RIGHT;
                break;

            case STATE_STEP_RIGHT:
                Motor_Drive(DIR_RIGHT);
                Timer_Wait(TIME_HORIZONTAL);
                Motor_Drive(STOP);
                Timer_Wait(TIME_PAUSE);
                state = STATE_MOVE_DOWN;
                break;

            case STATE_MOVE_DOWN:
                Motor_Drive(DIR_DOWN);
                Timer_Wait(TIME_VERTICAL);
                Motor_Drive(STOP);
                Timer_Wait(TIME_PAUSE);
                pass++;
                state = (pass >= TOTAL_PASSES) ? STATE_RETURN_HOME : STATE_MOVE_UP;
                break;

            case STATE_RETURN_HOME:
                Motor_Drive(DIR_UP);
                Timer_Wait(TIME_VERTICAL);
                Motor_Drive(STOP);
                Timer_Wait(TIME_PAUSE);

                for(i = 0; i < 4; i++)
                {
                    Motor_Drive(DIR_LEFT);
                    Timer_Wait(TIME_HORIZONTAL);
                    Motor_Drive(STOP);
                    Timer_Wait(TIME_PAUSE);
                }

                Motor_Drive(DIR_DOWN);
                Timer_Wait(TIME_VERTICAL);
                Motor_Drive(STOP);
                state = STATE_HALT;
                break;

            case STATE_HALT:
                Motor_Drive(STOP);   // System permanently stopped
                break;
        }
    }
}

/* ============================================================================
                           MOTOR DRIVE FUNCTION
   ============================================================================ */
void Motor_Drive(Direction_t dir)
{
    PWM1_0_CMPA_R = 0;  // Clear PWM A duty (safety)
    PWM1_0_CMPB_R = 0;  // Clear PWM B duty

    PA2_IN1 = PA3_IN2 = PA4_IN3 = PA5_IN4 = 0; // All motors OFF

    switch(dir)
    {
        case DIR_UP:
            PA2_IN1 = 0xFF;
            PWM1_0_CMPA_R = PWM_DUTY_7V;
            break;

        case DIR_DOWN:
            PA3_IN2 = 0xFF;
            PWM1_0_CMPA_R = PWM_DUTY_7V;
            break;

        case DIR_LEFT:
            PA4_IN3 = 0xFF;
            PWM1_0_CMPB_R = PWM_DUTY_3V;
            break;

        case DIR_RIGHT:
            PA5_IN4 = 0xFF;
            PWM1_0_CMPB_R = PWM_DUTY_3V;
            break;

        case STOP:
        default:
            break;
    }
}

/* ============================================================================
                           GPIO INITIALIZATION
   ============================================================================ */
void Hardware_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x01;             // Enable Port A clock (Page 340)
    while((SYSCTL_PRGPIO_R & 0x01) == 0);  // Wait until ready (Page 406) if the voltage is correct

    GPIO_PORTA_DIR_R |= 0x3C;              // PA2–PA5 output (Page 663)
    GPIO_PORTA_DEN_R |= 0x3C;              // Digital enable (Page 682)
}

/* ============================================================================
                              PWM INITIALIZATION
   ============================================================================ */
void PWM_Init(void)
{
    SYSCTL_RCGCPWM_R |= 0x02;              // Enable PWM1 (Page 354)
    SYSCTL_RCGCGPIO_R |= 0x08;             // Enable Port D (Page 340)
    while((SYSCTL_PRGPIO_R & 0x08) == 0);

    GPIO_PORTD_AFSEL_R |= 0x03;            // PD0, PD1 alternate function (Page 671)
    GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & ~0xFF) | 0x55; // PWM function (Page 688)
    GPIO_PORTD_DEN_R |= 0x03;              // Digital enable (Page 682)

    PWM1_0_CTL_R = 0;                      // Disable PWM for config (Page 1266)
    PWM1_0_GENA_R = 0x8C;                  // Generator A setup (Page 1282)
    PWM1_0_GENB_R = 0x80C;                 // Generator B setup (Page 1285)
    PWM1_0_LOAD_R = 16000 - 1;             // PWM period (Page 1278)
    PWM1_0_CTL_R |= 0x01;                  // Enable PWM (Page 1266)
    PWM1_ENABLE_R |= 0x03;                 // Enable outputs (Page 1247)
}

/* ============================================================================
                           TIMER INITIALIZATION
   ============================================================================ */
void Timer0_Init(void)
{
    SYSCTL_RCGCTIMER_R |= 0x01;            // Enable Timer0 (Page 338)
    TIMER0_CTL_R = 0;                      // Disable timer (Page 737)
    TIMER0_CFG_R = 0;                      // 32-bit mode (Page 727)
    TIMER0_TAMR_R = 0x01;                  // One-shot mode (Page 729)
}

/* ============================================================================
                              TIMER DELAY
   ============================================================================ */
void Timer_Wait(unsigned long ticks)
{
    TIMER0_CTL_R = 0;                      // Stop timer
    TIMER0_TAILR_R = ticks - 1;            // Load delay value (Page 756)
    TIMER0_ICR_R = 0x01;                   // Clear timeout flag (Page 754)
    TIMER0_CTL_R |= 0x01;                  // Start timer (Page 737)
    while((TIMER0_RIS_R & 0x01) == 0);      // Wait for timeout (Page 748)
    TIMER0_ICR_R = 0x01;                   // Clear flag
}

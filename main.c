/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/*
 * NEW FUNCTIONS or STRUCTS USED IN THE LAB3 KEY:
//CS_setDCOFrequency
eUSCI_UART_Config
EUSCIA0_IRQHandler
GPIO_setAsPeripheralModuleFunctionInputPin
Interrupt_enableSleepOnIsrExit
PMAP_configurePorts
printf  (from provided printf.h, NOT from stdio.h)
Timer_A_clearCaptureCompareInterrupt
Timer_A_clearInterruptFlag
Timer_A_CompareModeConfig
Timer_A_configureUpMode
Timer_A_getCaptureCompareEnabledInterruptStatus
Timer_A_getEnabledInterruptStatus
Timer_A_initCompare
Timer_A_setCompareValue
Timer_A_startCounter
Timer_A_UpModeConfig
UART_clearInterruptFlag
UART_enableInterrupt
UART_enableModule
UART_getEnabledInterruptStatus
UART_initModule
UART_receiveData
Many functions from Lab2 are also used, of course.
 */

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "printf.h"

// TODO: Lab3: create UART, Timer_A, and CCR config structures ~~~~~~~~~~
// UART config

volatile int stopwatch = 0; //Stopwatch variable to deal with halting timer
volatile unsigned int timeCount = 0; //initializes the time that timer is currently aT
static volatile uint8_t readdata;

//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};
//![Simple UART Config]


// Timer_A for driving RGB LED via PWM


// All 3 CCR configs for the 3 pins of the RGB LED
// PROVIDED: Timer_A struct for our debounce timer, triggering its interrupt when it hits its period value in CCR0
Timer_A_UpModeConfig debounce_Config =
{
     TIMER_A_CLOCKSOURCE_SMCLK, // Usually DCO clock, which in this case we set to 12MHz in main()
     TIMER_A_CLOCKSOURCE_DIVIDER_1,
     12*10^4, //10ms Debounce Delay
     TIMER_A_TAIE_INTERRUPT_ENABLE, // Should Timer_A send interrupts to Processor *at all*
     TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Should Timer_A reaching its period value (stored in CCR0) trigger an interrupt?
     TIMER_A_DO_CLEAR
};

// END CONFIG STRUCTS  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PROVIDED: Port mapping to drive pins 2.0, 2.1, 2.0 (RGB LED) by Timer_A0's CCR1, CCR2, CCR3 respectively.
const uint8_t portMapping[] =
{
// Will be used for Port 2; In order of pin number, 0-indexed
        PMAP_TA0CCR1A,  PMAP_TA0CCR2A,  PMAP_TA0CCR3A,
        PMAP_NONE,      PMAP_NONE,      PMAP_NONE,
        PMAP_NONE,      PMAP_NONE
};

int main(void)
{
    /* HaltinWDT  */
    MAP_WDT_A_holdTimer();

    /* TODO: LAB3 Set DCO to 12MHz */
    CS_setDCOFrequency(12000000); //Set DCO to 12MHz

//    Generic Interrupt enabling:
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();

//      TODO: LAB3: UART module needs to be configured to both transmit and receive. How it reacts to specific commands is elsewhere
//        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ENTER CODE FOR LAB3 UART init HERE:
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    UART_initModule(EUSCI_A0_BASE, &uartConfig);//initializes UART, leaves it DISABLED
    UART_enableModule(EUSCI_A0_BASE);//enables UART
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);//enables interrupt for UART
    Interrupt_enableInterrupt(INT_EUSCIA0);//enables interrupt in UART


    //        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End CODE FOR LAB3 UART init

    //      TODO: LAB3: Configure the Stopwatch (TIMER32_0) and button input (Port1) according to spec.
    //           Configure Timer_A2 for debouncing, using its interrupt to prevent it from cycling forever.
    //            (Struct for Timer A2 is provided, such that when it hits its CCR0 an interrupt is triggered
    //          Start with the stopwatch paused, so it only starts actually counting in response to the relevant UART commands
    //        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ENTER CODE FOR LAB3 stopwatch init HERE:

//    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE); //Initializing timer32
//    MAP_Timer32_startTimer(TIMER32_0_BASE, 0); //Starting timer32 timer
//    Timer32_setCount(TIMER32_0_BASE, 12000000); //Setting timer32 count
//    MAP_Timer32_enableInterrupt(TIMER32_0_BASE); //Enabling timer32 timer interrupt
//    MAP_Timer32_enableInterrupt(INT_PORT1); //Enabling interrupt for port 1

//     :) You're on your way


//        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End CODE FOR LAB3 stopwatch init


//          TODO: LAB3, Part 2: Prepare RGB LED driven by PWM signals driven by TIMER_A0 with multiple CCRs. Note that because the processor
//            doesn't care about when specifically the timer A0 is cycling, no interrupts from it or its CCRs are needed.
//          We can initially drive them with a 100% duty cycle for testing; the UART commands can easily change the duty cycle on their own
//        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ENTER CODE FOR LAB3 RC_RGB init HERE:

    //Timer_A_configureUpMode(TIMER_A0_BASE, Timer_A_UpModeConfig); //Configure up mode
    //Timer_A_initCapture(TIMER_A0_BASE, Timer_A_CaptureModeConfig); //Initialize capture
    //Timer_A_initCompare(TIMER_A0_BASE, Timer_A_CompareModeConfig); //Initialize compare for timerA
    //Timer_A_setOutputForOutputModeOutBitValue( TIMER_A0_BASE, captureCompareRegister1, outputModeOutBitValue); //How to compare output mode for red
    //Timer_A_setOutputForOutputModeOutBitValue( TIMER_A0_BASE, captureCompareRegister2, outputModeOutBitValue); //How to compare output mode for green
    //Timer_A_setOutputForOutputModeOutBitValue( TIMER_A0_BASE, captureCompareRegister3, outputModeOutBitValue); //How to compare output mode for blue

    //Timer_A_setCompareValue(TIMER_A0_BASE, compareRegister, compare_TimerA_value);

    //if(TIMER_A == TIMER_A_OUTPUTMODE_OUTBITVALUE)
    /*{
        Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, captureCompareRegister1); //enables compare capture interrupt
        Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, captureCompareRegister2);
        Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, captureCompareRegister3);

    }*/

//        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End CODE FOR LAB3 RC_RGB init

    // Use the LED to indicate if we got an "L" through the terminal
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    //          PROVIDED: configure RGB pins (2.0, 2.1, 2.2) for outputting PWM function, mapped from Timer_A0's CCRs 1, 2, and 3:
                PMAP_configurePorts(portMapping, PMAP_P2MAP, 3, PMAP_DISABLE_RECONFIGURATION);
                GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2,
                        GPIO_PRIMARY_MODULE_FUNCTION);

    while(1)
    {
        PCM_gotoLPM0InterruptSafe();
    }
}


// Helper function to clarify UART command behavior
// TODO: Lab3: Resumes, Pauses, or Restarts stopwatch depending on restart flag and the stopwatch state
//This is for "s" and "!"
//static void startStopStopwatch(bool restart){

    //uint32 _t status = Timer32_getInterruptStatus(TIMER32_0_BASE); //Gets current timer32 interrupt status

//};
// END Stopwatch State Helper Function Definition ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Helper function for sampling stopwatch. This avoids code duplication between UART and button sampling
// TODO: Lab3: Sample current stopwatch value, convert to number of elapsed milliseconds, and transmit to computer
void stopwatchSample(){
    //for when "p" is pressed
    uint32_t status = Timer32_getValue(TIMER32_0_BASE);//obtain timer value
    MAP_UART_transmitData(EUSCI_A0_BASE, status);//transmits data back to UART
}
// END Sample Stopwatch code ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PROVIDED: Converts a string of numeric characters into their corresponding whole number.
uint16_t atoi(volatile char* s, uint8_t l){
    uint16_t r = 0;
    uint8_t i;
    for (i = 0; i < l;i++){
        r = 10*r + (s[i]-'0');
    }
    return r;
}


/* EUSCI A0 UART ISR - Echoes data back to PC host */
void EUSCIA0_IRQHandler(void)
{
    // TODO: Lab3: UART Command Reception logic (reacts according to the specific received character)
        uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
        MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

        uint8_t readdata; //initializes variable that reads the data in the terminal window

        if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
        {
            readdata = MAP_UART_receiveData(EUSCI_A0_BASE);//recieves data from UART

//            printf(EUSCI_A0_BASE,"%c!",readdata);

            if (readdata == 114 || readdata == 82)
            { // Checks to see if sent character is "r" or "R"
//                P2OUT ^= BIT0;
                MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
            } // Toggles P2.0

            else if (readdata == 103 || readdata == 71)
              { // Checks to see if sent character is "g" or "G"
//                  P2OUT ^= BIT1;
                  MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
              } // Toggles P2.1

            else if (readdata == 98 || readdata == 66)
              { // Checks to see if sent character is "b" or "B"
//                  P2OUT ^= BIT2;
                  MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
              } // Toggles P2.2
            else if (readdata == 83 || readdata == 115)
            {// Checks to see if sent character is "S" or "s"
                //startStopStopwatch();
                if (stopwatch == 0)
                {
                    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);//starts Timer32
                    stopwatch = 1;//sets stop watch value to 1 (the stop watch is halted)
                    MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
                }
                else if (stopwatch == 1)
                {
                    MAP_Timer32_haltTimer(TIMER32_0_BASE);//halts the timer
                    stopwatch = 0;//sets the stop watch variable so the timer continues to count down
                    MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
                }
            }
            else if (readdata == 112 || readdata == 80)
            {// Checks to see if sent character is "p" or "P"
                //stopwatchSample();
                MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
                printf(EUSCI_A0_BASE, "Current stopwatch value = %i, Status = %i\r\n", timeCount, stopwatch);
            }
            else if (readdata == 33)
            {// Checks to see if sent character is "!"
                Timer32_setCount(TIMER32_0_BASE, 12000000); //reset timer32
                Timer32_haltTimer(TIMER32_0_BASE);//halts the timer
                timeCount = 0;//sets timer counter to zero
                MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
            }
            else //every other number is returned because not a valid input
            {
                MAP_UART_transmitData(EUSCI_A0_BASE, readdata);
                MAP_UART_transmitData(EUSCI_A0_BASE, '.');//transmits a period to the UART
            }

        }
    //    also tracks parsing state (most recently seen letter, and numeric characters in current number) to understand
    //          the relevant multicharacter commands, as per spec

    //    END LAB3 UART COMMAND LOGIC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}


// TODO: Lab3: Button Interrupt, Debounce Interrupt, and Stopwatch Interrupt ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void PORT1_IRQHandler(){
    //      Button 4: Samples current stopwatch value and sends it to computer (debounced)

    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);//NEW
        GPIO_clearInterruptFlag(GPIO_PORT_P1, status);//this is the only thing you need to do within the interrupt handler. Clear the interrupt flag

        if (GPIO_PIN4 & status)//NEW
        {
//            MAP_UART_transmitData(EUSCI_A0_BASE, readdata);//NEW
            printf(EUSCI_A0_BASE, "Current stopwatch value = %i, Status = %i\r\n", timeCount, stopwatch);//NEW, hopefully does the same as if "p" is input
        }
}

extern void TA2_0_IRQHandler(){
//      Debounce Interrupt: Clears our debouncing flag so we listen to future button interrupts again, and
//          prevents TA2 from cycling again like it normally would.

    Timer_A_clearInterruptFlag(TIMER_A2_BASE);//clears the interrupt for Timer A2
}

extern void T32_INT0_IRQHandler(){
    //      Stopwatch Interrupt: Complains about stopwatch count running out in the far, far future
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);//NEW
    if (status > 1000000000000)
    {
        Timer32_haltTimer(TIMER32_0_BASE);
    }
}
// END LAB3 BUTTON AND STOPWATCH INTERRUPTS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

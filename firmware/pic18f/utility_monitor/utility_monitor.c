/*************************************************************
 * Utility meter monitor
 *
 * Monitors the inputs INT0..2 and counts pulses on them.
 * Performs debouncing before the count is incremented
 *
 * Supports onewire temperature devices connected to RA0
 * Supports an SHT humidity sensor connected to RA3 (clk)
 *  and RA4(data). Needs a pullup on RA4 of 10k.
 *
 * (c) 2009-2010, Lieven Hollevoet.
 **************************************************************
 * target device   : PIC18F2520/PIC18F2580
 * clockfreq       : 32 MHz (internal oscillator + PLL)
 * target hardware : NetNode
 * UART speed      : 38400 bps/56k7bps
 * mpasmwin.exe    : v5.34
 * mplink.exe      : v4.34
 * mcc18.exe       : v3.34
 *************************************************************/

#include <p18cxxx.h>
#include <usart.h>
#include <delays.h>
#include <timers.h>
#include <pwm.h>
#include <stdio.h>

#include "fuses.h"
#include "utility_monitor.h"
#include "oo.h"
#include "shtxx.h"
#include "xpl.h"
#include "eeprom.h"

// Global variables used for message passing between ISR and main code
// Set time_ticks to 295 so that we send a heartbeat message withing 5 seconds after reset
volatile unsigned short time_ticks = 295;
volatile unsigned char time_ticks_oo = 0;
volatile unsigned char time_ticks_sht = 0;

volatile unsigned char debounce_water;
volatile unsigned char debounce_gas;
volatile unsigned char debounce_elec;

volatile unsigned char blink_hbeat;


//////////////////////////////////////////////////////////////////
// Main loop
// Program strategy:
// * handle incoming xpl messages (respond on requests)
// * send heartbeat
// * monitor port pins for falling edges, debounce them and increment counter if required
// * poll attached sensors when required (temperature/humitidy)

void main() {


    // Test code: set an initial ID in the EEPROM so that we don't have to configure the node
    //eeprom_write(0x00, 'A');
    //eeprom_write(0x01, 'F');
    //eeprom_write(0x02, '\0');
 
    // Hardware initialisation
    init();

    //printf("Hello world");

    // Init the xPL library
    xpl_init();


    while (1) {
        // Call the xPL message handler
        xpl_handler();

    }

}

//////////////////////////////////////////////////////////////////
// Hardware initialisation routine

void init(void) {

    char blink_count = 0;

    // All digital IO's on port A
    ADCON1 = 0x0F;

    // Set port direction bits
    TRISA = PortAConfig;
    TRISB = PortBConfig;
    TRISC = PortCConfig;

#ifdef __18F45K80_H

    TRISD = PortDConfig;
    OSCTUNEbits.PLLEN = 1;
    

    // Serial interface 1 init (57k6 @ 64 MHz, BRGH = 1, => 68)
    Open1USART(USART_ASYNCH_MODE &
            USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_EIGHT_BIT &
            USART_CONT_RX &
            USART_BRGH_HIGH,
            68);
#else
    // Oscillator selection
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    OSCTUNEbits.PLLEN = 1;

    // Serial interface init (38400 @ 32 MHz, BRGH = 1 => 51)
    OpenUSART(USART_ASYNCH_MODE &
            USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_EIGHT_BIT &
            USART_CONT_RX &
            USART_BRGH_HIGH,
            51);
#endif
    // Do the status LED flicker
    while (blink_count++ < 5) {
        Delay10KTCYx(50);
        stat0 = 0;
        Delay10KTCYx(50);
        stat0 = 1;
    }

    blink_hbeat = 0;


#ifdef __18F45K80_H

    // Enable the main 1-sec timer that will interrupt every second
    OpenTimer0(TIMER_INT_ON &
            T0_16BIT &
            T0_SOURCE_INT &
            T0_PS_1_256);

    // Prepare the 20 ms debouncing timer for the inputs
    OpenTimer1(TIMER_INT_ON &
            T1_16BIT_RW &
            T1_SOURCE_FOSC_4 &
            T1_PS_1_8 &
            T1_OSC1EN_OFF, 0);
#else
    // Enable the main 1-sec timer that will interrupt every second
    OpenTimer0(TIMER_INT_ON &
            T0_16BIT &
            T0_SOURCE_INT &
            T0_PS_1_256);

    // Prepare the 20 ms debouncing timer for the inputs
    OpenTimer1(TIMER_INT_ON &
            T1_16BIT_RW &
            T1_SOURCE_INT &
            T1_PS_1_8 &
            T1_OSC1EN_OFF);
#endif

    // Load initial timer values
    WriteTimer0(TMR0_VALUE);
    WriteTimer1(TMR1_VALUE);

    // Enable interrupt on falling edge of RB0/1/2
    INTCON2bits.INTEDG0 = 0; // Interrupt on falling edge
    INTCON2bits.INTEDG1 = 0; // Interrupt on falling edge
    INTCON2bits.INTEDG2 = 0; // Interrupt on falling edge

    // Enable pullups on Portb inputs
#ifdef __18F45K80_H
    INTCON2bits.NOT_RBPU = 0;
    // Enable digital inputs on AN8..10
    ANCON1 &= 0xF8;

#else
    INTCON2bits.RBPU = 0; // (bit is active low!)
#endif

    // Reset interrupt flags and enable the interrupts
    INTCONbits.INT0IF = 0; // Reset interrupt
    INTCONbits.INT0IE = 1; // Clear mask bit
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT2IF = 0;
    INTCON3bits.INT2IE = 1;

    INTCONbits.RBIE = 1;
    INTCONbits.PEIE = 1; // Peripheral interrupt enable for USART RX interrupt
    INTCONbits.GIE = 1; // Global interrupt enable

}



//////////////////////////////////////////////////////////////////
// Interrupt service routines
// 
// This is some ugly stuff, required by the C18 compiler to ensure 
// that the both interrupt vectors contain valid goto instructions

// Generate the high-priority interrupt vector, and put a goto high_isr there
#pragma code high_vector=0x08

void high_interrupt(void) {
    _asm GOTO high_isr _endasm
}

#pragma code

#pragma interrupt high_isr

void high_isr(void) {

    /* USART RX INTERRUPT HANDLING */
    if (PIR1bits.RCIF == 1) {
#ifdef __18F45K80_H
        xpl_fifo_push_byte(Read1USART());
#else
        xpl_fifo_push_byte(ReadUSART());
#endif
        PIR1bits.RCIF = 0;
        return;
    }

    /* RB0 INTERRUPT HANDLING */
    if (INTCONbits.INT0IF == 1) {
        debounce_water = 2;
        INTCONbits.INT0IF = 0;
        stat0 = 0;
        return;
    }

    /* RB1 INTERRUPT HANDLING */
    if (INTCON3bits.INT1IF == 1) {
        debounce_gas = 2;
        INTCON3bits.INT1IF = 0;
        stat0 = 0;
        return;
    }

    /* RB2 INTERRUPT HANDLING */
    if (INTCON3bits.INT2IF == 1) {
        debounce_elec = 2;
        INTCON3bits.INT2IF = 0;
        stat0 = 0;
        return;
    }

    /* TIMER 0 INTERRUPT HANDLING, 1 second main system tick timer */
    if (INTCONbits.TMR0IF == 1) {
        WriteTimer0(TMR0_VALUE); // Reprogram timer
        stat0 = 0;
        INTCONbits.TMR0IF = 0; // Clear interrupt flag
        time_ticks++;
        time_ticks_oo++;
        time_ticks_sht++;

        show_hbeat();
        stat0 = 1;
    }

    /* TIMER 1 INTERRUPT HANDLING */
    if (PIR1bits.TMR1IF == 1) {
        WriteTimer1(TMR1_VALUE);
        PIR1bits.TMR1IF = 0;

        // Check if we need to turn on/off the heartbeat LED
        if (blink_hbeat > 0) {
            stat0 = 0;
            blink_hbeat--;
        } else {
            stat0 = 1;
        }

        // Check if we need to handle a debounce
        if (debounce_water) {
            debounce_water--;
            stat0 = 1;
            if (debounce_water == 0 && PORTBbits.RB0 == 0) {
                xpl_trig(WATER);
            }
        }

        if (debounce_gas) {
            debounce_gas--;
            stat0 = 1;
            if (debounce_gas == 0 && PORTBbits.RB1 == 0) {
                xpl_trig(GAS);
            }
        }

        if (debounce_elec) {
            debounce_elec--;
            stat0 = 1;
            if (debounce_elec == 0 && PORTBbits.RB2 == 0) {
                xpl_trig(ELEC);
            }
        }

    }

    return;
}

// Generate low-priority interrupt vector, and put a goto low_isr there
#pragma code low_vector=0x18

void low_interrupt(void) {
    _asm GOTO low_isr _endasm
}

#pragma code
#pragma interruptlow low_isr

void low_isr(void) {

}

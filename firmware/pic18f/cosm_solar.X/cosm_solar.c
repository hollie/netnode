/*************************************************************
 * Cosm gateway
 *
 * Monitors the inputs INT0..2 and counts pulses on them.
 * Performs debouncing before the count is incremented
 *
 * (c) Lieven Hollevoet, LikaTronix.be
 *
 * License: http://creativecommons.org/licenses/by-nc-sa/3.0/
 **************************************************************
 * target device   : PIC18F2520/PIC18F2580
 * clockfreq       : 32 MHz (internal oscillator + PLL)
 * target hardware : NetNode v2
 * UART speed      : 38400 bps
 * IDE             : MPLABX
 * Compiler        : C18 v3.40
 *************************************************************/

#include <p18cxxx.h>
#include <usart.h>
#include <delays.h>
#include <timers.h>
#include <pwm.h>
#include <stdio.h>

#include "fuses.h"
#include "cosm_solar.h"
#include "../likalibs.X/oo.h"
#include "../likalibs.X/eeprom.h"
#include "../likalibs.X/cosm_socket.h"


// Global variables used for message passing between ISR and main code
// Set time_ticks to 50 so that we try to connect after 10 seconds the first time
volatile unsigned short time_ticks = 350;
volatile unsigned char time_ticks_oo = 0;
volatile unsigned char time_ticks_sht = 0;

volatile unsigned char debounce_water;
volatile unsigned char debounce_gas;
volatile unsigned char debounce_elec;

volatile unsigned short elec_pulses;

enum DEVICE_CONFIGURATION {
    \\
        SHT_PRESENT = 1, \\
        ONE_WIRE_PRESENT = 2, \\
        };

char hw_config = 0; /* mapped with DEVICE_CONFIGURATION
                                            bit 0 = sht_present
                                            bit 1 = one wire present
                    */

char sensor_name[2][12] = {"generated", "dummy"};
//char cosm_api_key[64] = "OivrtCBI0vaCBGTN46ktyluuoqeSAKxzZXlZUEdzdGlRYz0g";
char cosm_api_key[64] = "HelloWorld";

unsigned int cosm_feed_id  = 67597;

//////////////////////////////////////////////////////////////////
// Main loop
// Program strategy:
// * report the sensor status every minute to Cosm.com

void main() {

    signed short sht_temp;
    unsigned char sht_humi;
    unsigned short elec_report;

    // Hardware initialisation
    init();


    // Get configuration settings from EEPROM
    eeprom_read_block(MEM_API_KEY, cosm_api_key, sizeof cosm_api_key);
    //eeprom_write_ulong(MEM_FEED_ID, 67597);
    eeprom_read_block(MEM_FEED_ID, &cosm_feed_id, sizeof cosm_feed_id);

    elec_pulses = 0;
    elec_report = 0;
    // Check presence of sensors
    // Init the helper libraries so that we know if need the
    // library specific code
    /*if (!oo_init()) {
        hw_config |= ONE_WIRE_PRESENT;
        oo_read_temperatures();
    }

    if (sht_init() == 0) {
        hw_config|= SHT_PRESENT;
    }
*/

    // Init the Cosm library
    cosm_init(cosm_api_key, cosm_feed_id);


    while (1) {

        // Check if we need to report (every 5 minutes)
        if (time_ticks > 359) {
            stat0 = 0;
            time_ticks = 0;

            /*

            if (hw_config & SHT_PRESENT) {
                sht_do_measure();
                sht_temp = sht_get_temperature();
                sht_humi = sht_get_humidity();

                cosm_report(sensor_name[0], sht_humi, 0);
                cosm_report(sensor_name[1], sht_temp, 1);

            }
            stat0 = 1;
             */

            // Calculate the average power over the last 5 minute interval
            // and report
            elec_report = elec_pulses;
            cosm_report(sensor_name[0], elec_report*12, 0);
            elec_pulses = elec_pulses - elec_report;


        }

    }

}

//////////////////////////////////////////////////////////////////
// Hardware initialisation routine

void init(void) {

    char blink_count = 0;

    // Oscillator selection
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    OSCTUNEbits.PLLEN = 1;

    // All digital IO's on port A
    ADCON1 = 0x0F;

    // Set port direction bits
    TRISA = PortAConfig;
    TRISB = PortBConfig;
    TRISC = PortCConfig;

    // Serial interface init (38400 @ 32 MHz, BRGH = 1 => 51)
    OpenUSART(USART_ASYNCH_MODE &
            USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_EIGHT_BIT &
            USART_CONT_RX &
            USART_BRGH_HIGH,
            51);

    // Do the status LED flicker
  /*  while (blink_count++ < 5) {
        Delay10KTCYx(50);
        stat0 = 0;
        Delay10KTCYx(50);
        stat0 = 1;
    }
*/
    // Enable the main 1-sec timer that will interrupt every second
    OpenTimer0(TIMER_INT_ON &
            T0_16BIT &
            T0_SOURCE_INT &
            T0_PS_1_256);

    WriteTimer0(TMR0_VALUE); // Load initial timer value


    // Prepare the 20 ms debouncing timer for the inputs
    OpenTimer1(TIMER_INT_ON &
            T1_16BIT_RW &
            T1_SOURCE_INT &
            T1_PS_1_8 &
            T1_OSC1EN_OFF);

    WriteTimer1(TMR1_VALUE);

    // Enable pullups on Portb inputs
    INTCON2bits.RBPU = 0; // (bit is active low!)

    // Enable interrupt on falling edge of RB0/1/2
    INTCON2bits.INTEDG0 = 0; // Interrupt on falling edge
    INTCON2bits.INTEDG1 = 0; // Interrupt on falling edge
    INTCON2bits.INTEDG2 = 0; // Interrupt on falling edge

    // Reset interrupt flags and enable the interrupts
    INTCONbits.INT0IF = 0; // Reset interrupt
    INTCONbits.INT0IE = 1; // Clear mask bit
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT2IF = 0;
    INTCON3bits.INT2IE = 1;

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
        cosm_process_uart(ReadUSART());
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

    /* TIMER 0 INTERRUPT HANDLING */
    if (INTCONbits.TMR0IF == 1) {
        WriteTimer0(TMR0_VALUE); // Reprogram timer
        INTCONbits.TMR0IF = 0; // Clear interrupt flag
        time_ticks++;
        time_ticks_oo++;
        time_ticks_sht++;
    }

    /* TIMER 1 INTERRUPT HANDLING */
    if (PIR1bits.TMR1IF == 1) {
        WriteTimer1(TMR1_VALUE);
        PIR1bits.TMR1IF = 0;
        // Check if we need to handle a debounce
        if (debounce_water) {
            debounce_water--;
            stat0 = 1;
            if (debounce_water == 0 && PORTBbits.RB0 == 0) {
                //xpl_trig(WATER);
            }
        }

        if (debounce_gas) {
            debounce_gas--;
            stat0 = 1;
            if (debounce_gas == 0 && PORTBbits.RB1 == 0) {
                //xpl_trig(GAS);
            }
        }

        if (debounce_elec) {
            debounce_elec--;
            stat0 = 1;
            if (debounce_elec == 0 && PORTBbits.RB2 == 0) {
                //xpl_trig(ELEC);
                elec_pulses++;
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

void display_application_menu(char entry){
    switch (entry) {
        case 0:
           printf("Cosm Solar v%i.%i settings menu:\n", VER_MAJOR, VER_MINOR);
           printf("Current settings:\nFeed ID: %i\n", cosm_feed_id);
           printf("API key: %s\n", cosm_api_key);
           printf("Menu:\n\t[1] feed id\n\t[2] API key\n?");
        default:
            printf("Oops, invalid!\n");
    }
}
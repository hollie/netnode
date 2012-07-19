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

#ifndef _COSM_GATEWAY_H_
#define _COSM_GATEWAY_H_

// Define version number
#define VER_MAJOR 0
#define VER_MINOR 1

// Define port directions, 1=input
#define PortAConfig  	0xEF
#define PortBConfig  	0x07
#define PortCConfig  	0xDE

// Define bit variables attached to pins
#define stat0    (PORTAbits.RA4)

// Where are the configuration settings stored in EEPROM
#define MEM_FEED_ID  0
#define MEM_API_KEY  0

// Timer values
#define TMR0_VALUE 0x85EE // For a second interrupt.
#define TMR1_VALUE 0xB1E0 // For 20 ms interrupt.

// Function declarations
void init(void);
void high_isr(void);
void low_isr(void);

#endif
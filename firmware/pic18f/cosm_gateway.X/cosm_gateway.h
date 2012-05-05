/*************************************************************
 * Cosm gateway
 *
 * Monitors the inputs INT0..2 and counts pulses on them.
 * Performs debouncing before the count is incremented
 *
 * Reads out onewire temperature devices connected to RA0
 * Reads out an SHT humidity sensor connected to RA3 (clk)
 *  and RA4(data). Needs a pullup on RA4 of 10k.
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

// Define port directions, 1=input
#define PortAConfig  	0xEF
#define PortBConfig  	0x07
#define PortCConfig  	0xDE

// Define bit variables attached to pins
#define stat0    (PORTAbits.RA4)

// Timer values
#define TMR0_VALUE 0x85EE // For a second interrupt.
#define TMR1_VALUE 0xB1E0 // For 20 ms interrupt.

// Function declarations
void init(void);
void high_isr(void);
void low_isr(void);

#endif
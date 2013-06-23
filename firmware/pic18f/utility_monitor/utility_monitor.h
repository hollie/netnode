/*************************************************************
* Utility meter monitor
*
* (c) 2009, Lieven Hollevoet.
**************************************************************
* compiler        : Microchip C18 v3.40
* target device   : PIC18F2520/PIC18F2580
* clockfreq       : 8 MHz (internal oscillator)
* target hardware : NetNode
* UART speed      : 38400 bps
*************************************************************/
#ifndef _UTILMONI_H_
#define _UTILMONI_H_

// Define port directions, 1=input
#define PortAConfig  	0xFF
#define PortBConfig  	0x0F
#define PortCConfig  	0xBB
#define PortDConfig     0xF0

// Define bit variables attached to pins
#ifdef __18F45K80_H

#define stat0    (PORTDbits.RD2)
#define xport_rst (PORTCbits.RC2)
#define xport_cts (PORTCbits.RC1)

// Timer values
#define TMR0_VALUE 0x0BDB // For a second interrupt runnning @ 64 MHz with 256 prescaler
#define TMR1_VALUE 0xB1E0 // For 20 ms interrupt.

/* TODO fix the timer values */

#else

#define stat0    (PORTAbits.RA4)

// Timer values
#define TMR0_VALUE 0x85EE // For a second interrupt.
#define TMR1_VALUE 0xB1E0 // For 20 ms interrupt.

#endif

#define show_hbeat() (blink_hbeat += 5);

// Function declarations
void init(void);
void high_isr(void);
void low_isr(void);


#endif //_UTILMONI_H_

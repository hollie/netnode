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

// Define the device ID and firmware version
#define DEVICE_ID	0x00
#define FIRMWARE_MAJOR  0x00
#define FIRMWARE_MINOR  0x00

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


#endif //_UTILMONI_H_

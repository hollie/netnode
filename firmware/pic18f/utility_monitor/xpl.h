/*************************************************************
* xPL library
*
* (c) 2009
* Authors: Lieven Hollevoet
*          Dirk Buekenhoudt
**************************************************************
* target device   : PIC18F2320
* clockfreq       : 8 MHz (internal oscillator)
* target hardware : NetNode
*************************************************************/
#ifndef _XPL_H_
#define _XPL_H_

// DEVICE_ID must be 8 characters or less !!
#define XPL_DEVICE_ID "utilmon"

// VERSION number
#define XPL_VERSION_MAJOR 4
#define XPL_VERSION_MINOR 4

// Uncomment the next line to enable PWM output 
// on PORTC.2
#define PWM_ENABLED

// offset of INSTANCE_ID within target=VENDOR-DEVICEID.
#define XPL_TARGET_VENDOR_DEVICEID_INSTANCE_ID_OFFSET 22

// INSTANCE_ID must be 16 characters or less !!
// The offset in EEPROM where the INSTANCE_ID is stored 
#define XPL_EEPROM_INSTANCE_ID_OFFSET 0x00
#define XPL_RX_BUFSIZE 170
#define XPL_RXFIFO_SIZE 40
#define XPL_RXFIFO_SIZE_THESHOLD (XPL_RXFIFO_SIZE - 10)

enum XPL_DEVICE_TYPE      {GAS = 1,     \\
                           WATER = 2,   \\
                           ELEC = 4,    \\
                           TEMP = 8,    \\
                           PWM = 16,    \\
                           };

enum XPL_DEVICE_CONFIGURATION     {NODE_CONFIGURED = 1,     \\
                                   ONE_WIRE_PRESENT = 2,    \\
                                   };

// public functions called outside xpl lib
void xpl_init(void);
void xpl_handler(void);
void xpl_fifo_push_byte(char);
void xpl_trig(enum XPL_DEVICE_TYPE sensor);



// For flow control of the UART
#define XON  0x11
#define XOFF 0x13

#endif // _XPL_H_


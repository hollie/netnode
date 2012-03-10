/*************************************************************
* EEPROM data memory interface for PIC18FXXX devices 
*
* Allows for read and write of the program memory. 
*
* Warning! Interrupts are disabled during write !
*
* (c) Lieven Hollevoet
*************************************************************/

#include <p18cxxx.h>
#include "eeprom.h"

////////////////////////////////////////////////////////////
// Read an EEPROM memory location. Takes the address as input 
// and returns the data at the memory location.
//
// The function is based on the Microchip document DS39582A
// page 33.
////////////////////////////////////////////////////////////
char eeprom_read(char address){

    // Load address
    EEADR = address;

	// Point to EEPROM memory
	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;

	// Read, data is available in eedata the next cycle.
	EECON1bits.RD = 1;
	
	// Return value
	return EEDATA;

}

////////////////////////////////////////////////////////////
// Writes a byte to the data EEPROM. 
// Interrupts are disabled during this routine.
//
// The function is based on the Microchip document DS39582A
// page 33.
////////////////////////////////////////////////////////////
void eeprom_write(char address, char data){
	
	char enable_gie = 0;

	// 18F devices also need the CFGS bit to be cleared
	EECON1bits.CFGS = 0;

	// Wait for write to complete if one is busy
	while (EECON1bits.WR){};

	// Load address and data
	EEADR = address;
	EEDATA = data;

	// Point to EEPROM memory
	EECON1bits.EEPGD = 0;
	// Write enable
	EECON1bits.WREN = 1;
	// Disable interrupts if enabled
	if (INTCONbits.GIE) {
		INTCONbits.GIE = 0;
		enable_gie = 1;
	}

	// Required write sequence
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;

	// Disable write
	EECON1bits.WREN = 0;

	// Wait for the write operation to complete
	while (EECON1bits.WR){};

	// Enable interrupts if they were on before we entered this function
	if (enable_gie) {
		INTCONbits.GIE = 1;
	}
}



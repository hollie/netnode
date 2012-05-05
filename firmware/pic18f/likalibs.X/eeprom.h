/*************************************************************
* EEPROM data memory interface for PIC16F and 18F devices 
*
* Allows for read and write of the EEPROM data memory. 
*
* Warning! Interrupts are disabled during write !
*
* Tested on PIC18F2320
*
* (c) Lieven Hollevoet
*************************************************************/

#ifndef _EEPROM_H_
#define _EEPROM_H_

char  eeprom_read(char address);
void  eeprom_write(char address, char data);

#endif // _EEPROM_H_

/***********************************************************************************
Project:          SHTxx demo program (V2.1)
Filename:         SHTxx_Sample_Code.c

Original version: http://www.sensirion.com

Ported to PSOC C compiler by Lieven Hollevoet.
Ported to C18 compiler by Roeland Vandebriel and Lieven Hollevoet
***********************************************************************************/
#ifndef _SHTxx_H_
#define _SHTxx_H_

// Note: the data pin needs an external 10k pullup!
#define SHT_DATA_TRIS  TRISAbits.RA3
#define SHT_DATA       PORTAbits.RA3
#define SHT_CLK_TRIS   TRISAbits.RA2
#define SHT_CLK        PORTAbits.RA2

#define DATA(value) (SHT_DATA_TRIS = (value)? 1 : 0); (SHT_DATA = value);
#define SCK(b)      (SHT_CLK = b)

void sht_init(void);
char sht_do_measure(void);

#endif // _SHTxx_H_

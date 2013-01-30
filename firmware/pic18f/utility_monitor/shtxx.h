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
#ifdef __18F45K80_H

#define SHT_DATA_TRIS  TRISAbits.TRISA3
#define SHT_DATA       PORTAbits.RA3
#define SHT_CLK_TRIS   TRISAbits.TRISA2
#define SHT_CLK        PORTAbits.RA2

#else

#define SHT_DATA_TRIS  TRISAbits.RA3
#define SHT_DATA       PORTAbits.RA3
#define SHT_CLK_TRIS   TRISAbits.RA2
#define SHT_CLK        PORTAbits.RA2

#endif

#define DATA(value) (SHT_DATA_TRIS = (value)? 1 : 0); (SHT_DATA = value);
#define SCK(b)      (SHT_CLK = b)

/*
typedef union {
    signed short temperature;
    signed short dewpoint;
    unsigned char humidity;
} sht_reading;
*/
char sht_init(void);
char sht_do_measure(void);
//sht_reading sht_get_reading(void);
signed short sht_get_temperature(void);
signed short sht_get_dewpoint(void);
unsigned char sht_get_humidity(void);

#endif // _SHTxx_H_

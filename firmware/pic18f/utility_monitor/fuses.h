//////////////////////////////////////////////////////////////////
// Device configuration section
// For PIC18F25X0 on netnode hardware v1


#ifdef __18F2580_H
// For 18F2580
#pragma config OSC = IRCIO67, IESO = OFF, FCMEN = OFF
#else
// For 18F2520
#pragma config OSC = INTIO67, IESO = OFF, FCMEN = OFF
#endif

#pragma config PWRT = ON, BOREN = OFF
#pragma config WDT = OFF, WDTPS = 32768
#pragma config MCLRE = ON, LPT1OSC = OFF, PBADEN = OFF
#pragma config DEBUG = OFF, LVP = OFF, STVREN = ON, XINST = OFF
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
#pragma config CPB = OFF, CPD = OFF
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF
#pragma config EBTRB = OFF

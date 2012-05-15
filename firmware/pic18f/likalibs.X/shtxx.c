/***********************************************************************************
Project:          SHTxx demo program (V2.1)
Filename:         SHTxx_Sample_Code.c    

Original version: http://www.sensirion.com

Ported to PSOC C compiler by Lieven Hollevoet.
Ported to C18 compiler by Roeland Vandebriel
***********************************************************************************/
#include <stdio.h>
#include "shtxx.h"
#include <delays.h>
#include <p18cxxx.h> 

enum {TEMP,HUMI};

char StatusReg;
char CheckSum;

// sht_speed = speed of the sht interface
// A higher value slows down the speed this can be helpfull for long cables
// min allowed value = 1
unsigned char sht_speed = 10;

unsigned int  CurrentValue;
unsigned int  CurrentHumi;
unsigned int  CurrentTemp;
unsigned char CurrentSensorMask;
unsigned char CurrentSensorIndex;
unsigned char NrSensorsAvailable;


sht_tdata SHTdevice_table[MaxNrSensors];

unsigned char sht_crc = 0;

rom unsigned char sht_crc_rom[256] = {
	   0,  49,  98,  83, 196, 245, 166, 151, 185, 136, 219, 234, 125,  76,  31,  46,
	  67, 114,  33,  16, 135, 182, 229, 212, 250, 203, 152, 169,  62,  15,  92, 109,
	 134, 183, 228, 213,  66, 115,  32,  17,  63,  14,  93, 108, 251, 202, 153, 168,
	 197, 244, 167, 150,   1,  48,  99,  82, 124,  77,  30,  47, 184, 137, 218, 235,
	  61,  12,  95, 110, 249, 200, 155, 170, 132, 181, 230, 215,  64, 113,  34,  19,
	 126,  79,  28,  45, 186, 139, 216, 233, 199, 246, 165, 148,   3,  50,  97,  80,
	 187, 138, 217, 232, 127,  78,  29,  44,   2,  51,  96,  81, 198, 247, 164, 149,
	 248, 201, 154, 171,  60,  13,  94, 111,  65, 112,  35,  18, 133, 180, 231, 214,
	 122,  75,  24,  41, 190, 143, 220, 237, 195, 242, 161, 144,   7,  54, 101,  84,
	  57,   8,  91, 106, 253, 204, 159, 174, 128, 177, 226, 211,  68, 117,  38,  23,
	 252, 205, 158, 175,  56,   9,  90, 107,  69, 116,  39,  22, 129, 176, 227, 210,
	 191, 142, 221, 236, 123,  74,  25,  40,   6,  55, 100,  85, 194, 243, 160, 145,
	  71, 118,  37,  20, 131, 178, 225, 208, 254, 207, 156, 173,  58,  11,  88, 105,
	   4,  53, 102,  87, 192, 241, 162, 147, 189, 140, 223, 238, 121,  72,  27,  42,
	 193, 240, 163, 146,   5,  52, 103,  86, 120,  73,  26,  43, 188, 141, 222, 239,
	 130, 179, 224, 209,  70, 119,  36,  21,  59,  10,  89, 104, 255, 206, 157, 172
};

typedef union
{ unsigned int i;
  float f;
} Values;
//----------------------------------------------------------------------------------
// modul-var
//----------------------------------------------------------------------------------

// Data and clock set macros
// DATA is on P2.1, set it to 'open drain low' drive in chip view
// DATA is on PORTC1, set it to 'open drain low' drive in chip view

#define DATA(b) {PORTCbits.RC5 = 0; TRISCbits.RC5 = b; }

#define DATA_IN PORTCbits.RC5

// CLOCK is on P4.7, set it to 'strong slow' drive in chip view
// CLOCK is on PORTC0, set it to 'strong slow' drive in chip view

//#define SCK(b)  (PORTCbits.RC4 = b)
#define SCK(b) (PORTC = ((b==1) ? (PORTC | CurrentSensorMask) : (PORTC & (!CurrentSensorMask) )) )

#define noACK 0
#define ACK   1
                            //adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0


void sht_crc_init(void){
	sht_crc = 0;
}

void sht_crc_shuffle_byte(unsigned char input){
	unsigned char index = sht_crc ^ input;
	sht_crc = sht_crc_rom[index];
}

void sht_Start()
{
    unsigned char i;

    // clear all data
    for (i=0;i<MaxNrSensors;i++) {
        SHTdevice_table[i].crc   = 0;
        SHTdevice_table[i].valid = 0;
        SHTdevice_table[i].mask  = 0;
    }

    // Set all clock pins defined in the scan mask to output ports
    if (ScanMask & 0x01) {TRISCbits.RC0 = 0;}
    if (ScanMask & 0x02) {TRISCbits.RC1 = 0;}
    if (ScanMask & 0x04) {TRISCbits.RC2 = 0;}
    if (ScanMask & 0x08) {TRISCbits.RC3 = 0;}
    if (ScanMask & 0x10) {TRISCbits.RC4 = 0;}

    // Set Data line to open drain low settings and release the port
    LATCbits.LATC5 = 0;
    TRISCbits.RC5 = 1;
}


unsigned char sht_scan(void)
{
    unsigned char i,tmp;

    NrSensorsAvailable = 0;

    for (i=0x80;i>0;i/=2) // Shift true mask
    {
        CurrentSensorMask = i & ScanMask;
        if (CurrentSensorMask) {   // Check if this bit needs to be checked?
            tmp = sht_read_statusreg();
            if(tmp == 0){ // Check for a responce of the sensor 0 means no crc errors
               NrSensorsAvailable++;  // Aha, the sensor responded
               SHTdevice_table[NrSensorsAvailable-1].mask = CurrentSensorMask; // Store the mask for later use
            }
        }
    
    }
    return NrSensorsAvailable;
}

unsigned char sht_init(void)
{
    unsigned char NrSensors;
    sht_Start();
    NrSensors = sht_scan();
    return NrSensors;

}

//----------------------------------------------------------------------------------
char sht_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge 
{ 
  unsigned char i,error=0;  
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  { if (i & value) {
      DATA(1); // TRISCbits.RC5 = 1;          //masking value with i , write to SENSI-BUS
      } else {
          DATA(0); //PORTCbits.RC5 = 0; TRISCbits.RC5 = 0;
      }
    Delay10TCYx(2*sht_speed);
    SCK(1);                          //clk for SENSI-BUS
    Delay10TCYx(2*sht_speed);
    SCK(0);
    Delay10TCYx(2*sht_speed);
  }
  DATA(1); //TRISCbits.RC5 = 1;                           //release DATA-line
  Delay10TCYx(2*sht_speed);
  SCK(1);                            //clk #9 for ack 
  Delay10TCYx(2*sht_speed);
  error=DATA_IN;                       //check ack (DATA will be pulled down by SHT11)
  SCK(0);        
  Delay10TCYx(2*sht_speed);
  return error;                     //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
char sht_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" 
{ 
  unsigned char i,val=0;
  DATA(1);//TRISCbits.RC5 = 1;                         //release DATA-line

  for (i=0x80;i>0;i/=2)              //shift bit for masking
  {
     SCK(1);                          //clk for SENSI-BUS
     Delay10TCYx(2*sht_speed);
     if (DATA_IN) val=(val | i);     //read bit
     SCK(0);
     Delay10TCYx(sht_speed);
  }
  
  if (ack){
  	DATA(0); //PORTCbits.RC5 = 0; TRISCbits.RC5 = 0;
  } else {
  	DATA(1); //TRISCbits.RC5 = 1;
  }
                        //in case of "ack==1" pull down DATA-Line
  SCK(1);               //clk #9 for ack
  Delay10TCYx(2*sht_speed);
  SCK(0);		//release DATA-line
  DATA(1);//TRISCbits.RC5 = 1;
  return val;
}

//----------------------------------------------------------------------------------
void sht_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{  
   DATA(1);//TRISCbits.RC5 = 1;
   SCK(0);                   //Initial state
   Delay10TCYx(2*sht_speed);
   SCK(1);
   Delay10TCYx(2*sht_speed);
   DATA(0);//PORTCbits.RC5 = 0; TRISCbits.RC5 = 0;
   Delay10TCYx(2*sht_speed);
   SCK(0);  
   Delay10TCYx(2*sht_speed);
   SCK(1);
   Delay10TCYx(2*sht_speed);
   DATA(1); //TRISCbits.RC5 = 1;
   Delay10TCYx(2*sht_speed);
   SCK(0);		   
}

//----------------------------------------------------------------------------------
void sht_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{  
  unsigned char i; 
  DATA(1);//TRISCbits.RC5 = 1;
  Delay10TCYx(2*sht_speed);
  SCK(0);                    //Initial state
  Delay10TCYx(2*sht_speed);
  for(i=0;i<9;i++)                  //9 SCK cycles
  { SCK(1);
    Delay10TCYx(2*sht_speed);
    SCK(0);
    Delay10TCYx(2*sht_speed);
  }
  sht_transstart();                   //transmission start
}

//----------------------------------------------------------------------------------
unsigned char sht_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset 
{ 
  unsigned char error=0;  
  sht_connectionreset();              //reset communication
  error+=sht_write_byte(RESET);       //send RESET-command to sensor
  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char sht_read_statusreg(void)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  unsigned char tmp = 0;
  unsigned char bits = 8;
  sht_crc_init();
  sht_transstart();                   //transmission start
  error=sht_write_byte(STATUS_REG_R); //send command to sensor
  sht_crc_shuffle_byte(STATUS_REG_R);
  StatusReg =sht_read_byte(ACK);      //read status register (8-bit)
  sht_crc_shuffle_byte(StatusReg);
  CheckSum  =sht_read_byte(noACK);    //read checksum (8-bit)
  while( bits--)
  {
	tmp >>=1;
	if( sht_crc & 0b10000000)
		tmp |= 0b10000000;
	sht_crc <<=1;
  }

  if (tmp != CheckSum) {
      SHTdevice_table[CurrentSensorIndex].crc++;
      error++;
  }

  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char sht_write_statusreg(unsigned char p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  sht_transstart();                   //transmission start
  error+=sht_write_byte(STATUS_REG_W);//send command to sensor
  error+=sht_write_byte(p_value);    //send value of status register
  return error;                     //error>=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char sht_measure(unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{ 
  unsigned error=0;
  unsigned int i;
  unsigned char LSB;
  unsigned char MSB;
  unsigned char tmp = 0;
  unsigned char bits = 8;

  sht_crc_init();
  sht_transstart();                   //transmission start
  switch(mode){                     //send command to sensor
    case TEMP	: error+=sht_write_byte(MEASURE_TEMP); sht_crc_shuffle_byte(MEASURE_TEMP);break;
    case HUMI	: error+=sht_write_byte(MEASURE_HUMI); sht_crc_shuffle_byte(MEASURE_HUMI);break;
    default     : break;	 
  }
  for (i=0;i<40000;i++) {
  	if(DATA_IN==0) {
            break; //wait until sensor has finished the measurement
        }
	Delay100TCYx(4);      //50 us
  }
  
  if(DATA_IN)
  {
      error+=1;                // or timeout (~2 sec.) is reached
  }
  MSB  = sht_read_byte(ACK);    //read the first byte (MSB)
  sht_crc_shuffle_byte(MSB);
  LSB  = sht_read_byte(ACK);    //read the second byte (LSB)
  sht_crc_shuffle_byte(LSB);
  CurrentValue = MSB * 256 + LSB;
  CheckSum = sht_read_byte(noACK);  //read checksum

  	while( bits--)
	{
		tmp >>=1;
		if( sht_crc & 0b10000000)
			tmp |= 0b10000000;
		sht_crc <<=1;
	}

  if (tmp != CheckSum) {
      SHTdevice_table[CurrentSensorIndex].crc++;
      return 1;
  }
  return error;
  
}


//----------------------------------------------------------------------------------------
void calc_sth11(float *p_humidity ,float *p_temperature)
//----------------------------------------------------------------------------------------
// calculates temperature [캜] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [째C]
{ const float C1=  -4.0;              // for 12 Bit
  const float C2=  +0.0405;           // for 12 Bit
  const float C3=  -0.0000028;        // for 12 Bit
  const float T1=  +0.01;             // for 14 Bit @ 5V
  const float T2=  +0.00008;          // for 14 Bit @ 5V
//  const float d1= -40.1;              // for 5V [캜]
  const float d1= -39.66;              // for 3,3V [캜]
  const float d2=   0.01;             // for 14 bit [캜]

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit 
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [캜]

  t_C= d1 + t * d2;                  //calc. temperature from ticks to [캜]
  rh_lin=C3*rh*rh + C2*rh + C1;      //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;   //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100;       //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;       //the physical possible range

  *p_temperature=t_C;               //return temperature [캜]
  *p_humidity=rh_true;              //return humidity[%RH]
}

//--------------------------------------------------------------------
float calc_dewpoint(float h,float t)
//--------------------------------------------------------------------
// calculates dew point
// input:   humidity [%RH], temperature [째C]
// output:  dew point [째C]
{ float logEx,dew_point;
  logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2);
  dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
  return dew_point;
}

//----------------------------------------------------------------------------------
char sht_do_measure(unsigned char Sensor){
//----------------------------------------------------------------------------------
// sample program that shows how to use SHT11 functions
// 1. connection reset 
// 2. measure humidity [ticks](12 bit) and temperature [ticks](14 bit)
// 3. calculate humidity [%RH] and temperature [째C]
// 4. calculate dew point [째C]
// 5. print temperature, humidity, dew point  

  unsigned int humi_val_i;
  unsigned int temp_val_i;

  Values humi_val;
  Values temp_val;

  unsigned char error,checksum;
  unsigned int i;
  float humi_val_f;
  float temp_val_f;
  
  error=sht_SetSensor(Sensor);
  
  sht_connectionreset();
  
  SHTdevice_table[CurrentSensorIndex].valid = 0;
  error+=sht_measure(HUMI);  //measure humidity
  humi_val_i = CurrentValue;
  error+=sht_measure(TEMP);  //measure temperature
  temp_val_i = CurrentValue;
  
  if(error!=0) {
      sht_connectionreset();
  }                //in case of an error: connection reset
  else
  { humi_val_f = (float)humi_val_i;                   //converts integer to float
    temp_val_f = (float)temp_val_i;                   //converts integer to float
    calc_sth11(&humi_val_f,&temp_val_f);
    SHTdevice_table[CurrentSensorIndex].humidity    = (unsigned char)humi_val_f;
    SHTdevice_table[CurrentSensorIndex].temperature = (int)(temp_val_f*100);
    SHTdevice_table[CurrentSensorIndex].valid       = 1;
  }
  return error;
} 


int sht_getCurrentTemp(unsigned char Sensor){
    if(SHTdevice_table[Sensor-1].valid){
        return SHTdevice_table[Sensor-1].temperature;
    }
    return 66; // put a rediculus value
}

unsigned char sht_getCurrentHumi(unsigned char Sensor){
    if(SHTdevice_table[Sensor-1].valid){
        return SHTdevice_table[Sensor-1].humidity;
    }
    return 0; // put a rediculus value

}

char sht_getCurrentCRC(unsigned char Sensor){

    return SHTdevice_table[Sensor-1].crc;

}

unsigned char sht_getCurrentMask(unsigned char Sensor){

    return SHTdevice_table[Sensor-1].mask;

}

char sht_ClearCRC(unsigned char Sensor){
    if (Sensor > NrSensorsAvailable){
        return 1;
    }
    SHTdevice_table[Sensor-1].crc = 0;
    return 0;
}


unsigned char sht_SetSensor(unsigned char Sensor){
    if (Sensor > NrSensorsAvailable){
        return 1;
    }
    CurrentSensorIndex = Sensor-1;
    CurrentSensorMask  = SHTdevice_table[CurrentSensorIndex].mask;
    return 0;
}

char sht_GetDeviceCount(void){
    return NrSensorsAvailable;
}
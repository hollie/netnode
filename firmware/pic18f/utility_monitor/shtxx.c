/***********************************************************************************
Project:          SHTxx demo program (V2.1)
Filename:         SHTxx_Sample_Code.c    

Original version: http://www.sensirion.com

Ported to PSOC C compiler by Lieven Hollevoet.
Ported to C18 compiler by Roeland Vandebriel
***********************************************************************************/
//RV: #include "PSoCAPI.h"
#include <stdio.h>
#include "shtxx.h"
#include <delays.h> // added by RV
#include <p18cxxx.h> // added by RV
//#include <timers.h>  // added by RV

enum {TEMP,HUMI};

char StatusReg;
char CheckSum;
unsigned int CurrentValue;
unsigned int CurrentHumi;
unsigned int CurrentTemp;

int  Temp[7];
char Humi[7];

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

#define DATA(Id,Value) (TRISCbits.RC5 = Value)

#define DATA_IN PORTCbits.RC5

// CLOCK is on P4.7, set it to 'strong slow' drive in chip view
// CLOCK is on PORTC0, set it to 'strong slow' drive in chip view

#define SCK(b)  (PORTCbits.RC4 = b)


#define noACK 0
#define ACK   1
                            //adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0


void s_Start()
{
    unsigned char tmp;

    // Release the IO line
    TRISCbits.RC5 = 1;

    tmp = TRISC;
    TRISCbits.RC4 = 0;
    tmp = TRISC;
    LATCbits.LATC5 = 0;
    tmp = PORTC;
    PORTCbits.RC5 = 0; TRISCbits.RC5 = 0;
    TRISCbits.RC5 = 1;
    SCK(0);
    SCK(1);
    SCK(0);
    TRISCbits.RC5 = 1;

    
        
}

//----------------------------------------------------------------------------------
char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge 
{ 
  unsigned char i,error=0;  
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  { if (i & value) {
      TRISCbits.RC5 = 1;          //masking value with i , write to SENSI-BUS
      } else {
          PORTCbits.RC5 = 0; TRISCbits.RC5 = 0;
      }
    SCK(1);                          //clk for SENSI-BUS
    Nop(); Nop();Nop(); Nop();                          // wait for 1 us
    SCK(0);
  }
  TRISCbits.RC5 = 1;                           //release DATA-line
  SCK(1);                            //clk #9 for ack 
  Nop();Nop();Nop(); Nop();                    // wait for 1 us
  error=DATA_IN;                       //check ack (DATA will be pulled down by SHT11)
  SCK(0);        
  return error;                     //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" 
{ 
  unsigned char i,val=0;
  TRISCbits.RC5 = 1;                         //release DATA-line

  for (i=0x80;i>0;i/=2)              //shift bit for masking
  {
     SCK(1);                          //clk for SENSI-BUS
     if (DATA_IN) val=(val | i);     //read bit
     SCK(0);
     Nop();Nop();
  }
  
  if (ack){
  	PORTCbits.RC5 = 0; TRISCbits.RC5 = 0;
  } else {
  	TRISCbits.RC5 = 1;
  }
                        //in case of "ack==1" pull down DATA-Line
  SCK(1);               //clk #9 for ack
  Nop(); Nop(); Nop(); Nop();         // wait for 0,5 us
  SCK(0);		//release DATA-line						    
  TRISCbits.RC5 = 1;
          
  return val;
}

//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{  
   TRISCbits.RC5 = 1; SCK(0);                   //Initial state
   Nop(); Nop();Nop(); Nop();
   SCK(1);
   Nop(); Nop();Nop(); Nop();
   PORTCbits.RC5 = 0; TRISCbits.RC5 = 0;
   Nop(); Nop();Nop(); Nop();
   SCK(0);  
   Nop(); Nop();Nop(); Nop();
   SCK(1);
   Nop(); Nop();Nop(); Nop();
   TRISCbits.RC5 = 1;
   Nop(); Nop();Nop(); Nop();
   SCK(0);		   
}

//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{  
  unsigned char i; 
  TRISCbits.RC5 = 1;
  Nop(); Nop();Nop(); Nop();
  SCK(0);                    //Initial state
  Nop(); Nop();Nop(); Nop();
  for(i=0;i<9;i++)                  //9 SCK cycles
  { SCK(1);
    Nop(); Nop();Nop(); Nop();
    SCK(0);
    Nop(); Nop();Nop(); Nop();
  }
  s_transstart();                   //transmission start
}

//----------------------------------------------------------------------------------
char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset 
{ 
  unsigned char error=0;  
  s_connectionreset();              //reset communication
  error+=s_write_byte(RESET);       //send RESET-command to sensor
  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_read_statusreg(void)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  s_transstart();                   //transmission start
  error=s_write_byte(STATUS_REG_R); //send command to sensor
  StatusReg =s_read_byte(ACK);      //read status register (8-bit)
  CheckSum  =s_read_byte(noACK);    //read checksum (8-bit)
  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_write_statusreg(unsigned char p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  s_transstart();                   //transmission start
  error+=s_write_byte(STATUS_REG_W);//send command to sensor
  error+=s_write_byte(p_value);    //send value of status register
  return error;                     //error>=1 in case of no response form the sensor
}
 							   
//----------------------------------------------------------------------------------
char s_measure(unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{ 
  unsigned error=0;
  unsigned int i;
  unsigned char LSB;
  unsigned char MSB;

  s_transstart();                   //transmission start
  switch(mode){                     //send command to sensor
    case TEMP	: error+=s_write_byte(MEASURE_TEMP); break;
    case HUMI	: error+=s_write_byte(MEASURE_HUMI); break;
    default     : break;	 
  }
  for (i=0;i<40000;i++) {
  	if(DATA_IN==0) break; //wait until sensor has finished the measurement
	Delay100TCYx(4);      //50 us
  }
  
  if(DATA_IN) error+=1;                // or timeout (~2 sec.) is reached
  MSB  = s_read_byte(ACK);    //read the first byte (MSB)
  LSB  = s_read_byte(ACK);    //read the second byte (LSB)
  CurrentValue = MSB * 256 + LSB;
  CheckSum = s_read_byte(noACK);  //read checksum
  
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
char s_do_measure(unsigned char index){
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
  
  
  s_connectionreset();
  
  //while(1)
  //{ 
  error=0;
  error+=s_measure(HUMI);  //measure humidity
  humi_val_i = CurrentValue;
  error+=s_measure(TEMP);  //measure temperature
  temp_val_i = CurrentValue;
  
  if(error!=0) {
      s_connectionreset();
 //     LTRX_CPutString("SHTxx error: ");
 //     LTRX_PutSHexByte(error);
  }                //in case of an error: connection reset
  else
  { humi_val_f = (float)humi_val_i;                   //converts integer to float
    temp_val_f = (float)temp_val_i;                   //converts integer to float
    calc_sth11(&humi_val_f,&temp_val_f);
    Humi[index] = (unsigned char)humi_val_f;
    Temp[index] = (int)(temp_val_f*100);

    //calculate humidity, temperature
//    *dew_point=calc_dewpoint(humi_val.f,temp_val.f); //calculate dew point
	  
//	printf("SHTxx T:%5.1f C RH:%5.1f%\% DewPt:%5.1f C\r\n",temp_val.f,humi_val.f,dew_point);
  }
  return error;
} 

int s_getCurrentTemp(unsigned char index){

    return Temp[index];

}

unsigned char s_getCurrentHumi(unsigned char index){

    return Humi[index];
    
}
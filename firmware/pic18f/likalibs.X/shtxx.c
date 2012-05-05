/***********************************************************************************
Project:          SHTxx demo program (V2.1)
Filename:         SHTxx_Sample_Code.c    

Original version: http://www.sensirion.com

Ported to PSOC C compiler by Lieven Hollevoet.
Ported to C18 compiler by Roeland Vandebriel and Lieven Hollevoet
 ***********************************************************************************/
#include <p18cxxx.h>
#include <stdio.h>
#include <delays.h> 
#include <math.h>

#include "shtxx.h"

enum {
    TEMP, HUMI
};

//sht_reading value_read;
unsigned char sht_humidity;
signed short  sht_temperature;
signed short  sht_dewpoint;

char StatusReg;
char CheckSum;
unsigned int CurrentValue;

#define noACK 0
#define ACK   1
//                            adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0

// Hardware initialization, tries to detect if a sensor is present
// Returns 0 if is a sensor connected.
char sht_init() {

    // Clock line needs to be output
    //TRISAbits.RA4 = 0;
    SHT_CLK_TRIS = 0;

    // Release the IO line
    DATA(1);
    SHT_DATA = 0;

    return sht_do_measure();
}

//----------------------------------------------------------------------------------

char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge 
{
    unsigned char i, error = 0;
    for (i = 0x80; i > 0; i /= 2) //shift bit for masking
    {
        if (i & value) {
            DATA(1); //masking value with i , write to SENSI-BUS
        } else {
            DATA(0);
        }
        SCK(1); //clk for SENSI-BUS
        Nop();
        Nop();
        Nop();
        Nop(); // wait for 1 us
        SCK(0);
    }
    DATA(1); //release DATA-line
    SCK(1);  //clk #9 for ack
    Nop();
    Nop();
    Nop();
    Nop(); // wait for 1 us
    error = SHT_DATA; //check ack (DATA will be pulled down by SHT11)
    SCK(0);
    return error; //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------

char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" 
{
    unsigned char i, val = 0;
    DATA(1); //release DATA-line

    for (i = 0x80; i > 0; i /= 2) //shift bit for masking
    {
        SCK(1); //clk for SENSI-BUS
        if (SHT_DATA) val = (val | i); //read bit
        SCK(0);
        Nop();
        Nop();
    }

    if (ack) {
        DATA(0);
    } else {
        DATA(1);
    }
    //in case of "ack==1" pull down DATA-Line
    SCK(1); //clk #9 for ack
    Nop();
    Nop();
    Nop();
    Nop(); // wait for 0,5 us
    SCK(0); //release DATA-line
    DATA(1);

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
    DATA(1);
    SCK(0); //Initial state
    Nop();
    Nop();
    Nop();
    Nop();
    SCK(1);
    Nop();
    Nop();
    Nop();
    Nop();
    DATA(0);
    Nop();
    Nop();
    Nop();
    Nop();
    SCK(0);
    Nop();
    Nop();
    Nop();
    Nop();
    SCK(1);
    Nop();
    Nop();
    Nop();
    Nop();
    DATA(1);
    Nop();
    Nop();
    Nop();
    Nop();
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
    DATA(1);
    Nop();
    Nop();
    Nop();
    Nop();
    SCK(0); //Initial state
    Nop();
    Nop();
    Nop();
    Nop();
    for (i = 0; i < 9; i++) //9 SCK cycles
    {
        SCK(1);
        Nop();
        Nop();
        Nop();
        Nop();
        SCK(0);
        Nop();
        Nop();
        Nop();
        Nop();
    }
    s_transstart(); //transmission start
}

//----------------------------------------------------------------------------------

char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset 
{
    unsigned char error = 0;
    s_connectionreset(); //reset communication
    error += s_write_byte(RESET); //send RESET-command to sensor
    return error; //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------

char s_read_statusreg(void)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{
    unsigned char error = 0;
    s_transstart(); //transmission start
    error = s_write_byte(STATUS_REG_R); //send command to sensor
    StatusReg = s_read_byte(ACK); //read status register (8-bit)
    CheckSum = s_read_byte(noACK); //read checksum (8-bit)
    return error; //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------

char s_write_statusreg(unsigned char p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{
    unsigned char error = 0;
    s_transstart(); //transmission start
    error += s_write_byte(STATUS_REG_W); //send command to sensor
    error += s_write_byte(p_value); //send value of status register
    return error; //error>=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------

char s_measure(unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{
    unsigned char error = 0;
    unsigned int i;
    unsigned char LSB;
    unsigned char MSB;

    s_transstart(); //transmission start
    switch (mode) { //send command to sensor
        case TEMP: error += s_write_byte(MEASURE_TEMP);
            break;
        case HUMI: error += s_write_byte(MEASURE_HUMI);
            break;
        default: break;
    }
    for (i = 0; i < 40000; i++) {
        if (SHT_DATA == 0) break; //wait until sensor has finished the measurement
        Delay100TCYx(4); //50 us
    }

    if (SHT_DATA) error += 1; // or timeout (~2 sec.) is reached
    MSB = s_read_byte(ACK); //read the first byte (MSB)
    LSB = s_read_byte(ACK); //read the second byte (LSB)
    CurrentValue = MSB * 256 + LSB;
    CheckSum = s_read_byte(noACK); //read checksum

    //printf("Read for mode %d resulted in %02X%02X", mode, MSB, LSB);
    return error;

}


//----------------------------------------------------------------------------------------

void calc_sth11(float *p_humidity, float *p_temperature)
//----------------------------------------------------------------------------------------
// calculates temperature [°C] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [°C]
{
    const float C1 = -4.0; // for 12 Bit
    const float C2 = +0.0405; // for 12 Bit
    const float C3 = -0.0000028; // for 12 Bit
    const float T1 = +0.01; // for 14 Bit @ 5V
    const float T2 = +0.00008; // for 14 Bit @ 5V
    //  const float d1= -40.1;              // for 5V [°C]
    const float d1 = -39.66; // for 3,3V [°C]
    const float d2 = 0.01; // for 14 bit [°C]

    float rh = *p_humidity; // rh:      Humidity [Ticks] 12 Bit
    float t = *p_temperature; // t:       Temperature [Ticks] 14 Bit
    float rh_lin; // rh_lin:  Humidity linear
    float rh_true; // rh_true: Temperature compensated humidity
    float t_C; // t_C   :  Temperature [°C]

    t_C = d1 + t * d2; //calc. temperature from ticks to [°C]
    rh_lin = C3 * rh * rh + C2 * rh + C1; //calc. humidity from ticks to [%RH]
    rh_true = (t_C - 25)*(T1 + T2 * rh) + rh_lin; //calc. temperature compensated humidity [%RH]
    if (rh_true > 100)rh_true = 100; //cut if the value is outside of
    if (rh_true < 0.1)rh_true = 0.1; //the physical possible range

    *p_temperature = t_C; //return temperature [°C]
    *p_humidity = rh_true; //return humidity[%RH]
}

//--------------------------------------------------------------------

float calc_dewpoint(float h, float t)
//--------------------------------------------------------------------
// calculates dew point
// input:   humidity [%RH], temperature [Â°C]
// output:  dew point [Â°C]
{
    float logEx, dew_point;
    logEx = 0.66077 + 7.5 * t / (237.3 + t)+(log10(h) - 2);
    dew_point = (logEx - 0.66077)*237.3 / (0.66077 + 7.5 - logEx);
    return dew_point;
}

// Perform a mreasurement and store the result in the 'value_read'
// variable for reporting through the sht_get_reading function.
// This function only returns a char that is either '0' when the 
// measurement went fine, or a non-zero value on error.
// This function is called in de init function to detect if a sensor
// is connected to the processor.
char sht_do_measure() {
    //----------------------------------------------------------------------------------
    // sample program that shows how to use SHT11 functions
    // 1. connection reset
    // 2. measure humidity [ticks](12 bit) and temperature [ticks](14 bit)
    // 3. calculate humidity [%RH] and temperature [°C]
    // 4. calculate dew point [°C]
    // 5. print temperature, humidity, dew point

    unsigned int humi_val_i;
    unsigned int temp_val_i;

    unsigned char error, checksum;
    unsigned int i;

    float humi_val_f;
    float temp_val_f;
    float dewpoint_f;

    s_connectionreset();

    error = 0;
    error += s_measure(HUMI); //measure humidity
    humi_val_i = CurrentValue;
    error += s_measure(TEMP); //measure temperature
    temp_val_i = CurrentValue;

    if (error != 0) {
        s_connectionreset();
        //printf("SHTxx error: %X\n", error);
    } else {
        humi_val_f = (float) humi_val_i; //converts integer to float
        temp_val_f = (float) temp_val_i; //converts integer to float
        calc_sth11(&humi_val_f, &temp_val_f);

        dewpoint_f = calc_dewpoint(humi_val_f, temp_val_f); //calculate dew point

        //value_read.humidity    = (unsigned char) humi_val_f;
        //value_read.temperature = (signed short)temp_val_f * 100;
        //value_read.dewpoint    = (signed short)dewpoint_f * 100;
        sht_humidity    = (unsigned char) (humi_val_f + 0.5);
        sht_temperature = (signed short) (temp_val_f * 100);
        sht_dewpoint    = (signed short) (dewpoint_f * 100);

        // printf does not support float, so calc integer and fractional part of the temperatures
        // Convert float to two chars, as the printf does not support printing floats
        //temp_whole = (signed char) temp_val_f;
        //temp_part  = (unsigned char) (temp_val_f * 100 - (float) temp_whole * 100);
        //dew_whole  = (signed char) dew_point_f;
        //dew_part   = (unsigned char) (dew_point_f * 100 - (float) dew_whole * 100);


        //printf("SHTxx T:%i.%i C RH:%d DewPt: %i.%i\r\n", temp_whole, temp_part, (unsigned char)humi_val_f, dew_whole, dew_part);
    }
    return error;
}

// Only call this function after it was verified sht_do_measure does not return in error
// The temperatures are returned as type unsigned short multiplied by 100
// because the C18 compiler is not too supportive of floats, both in passing values
// through the union and in printing them
//sht_reading sht_get_reading(){
//    //sht_do_measure();
//    return value_read;
//}

signed short sht_get_temperature(){
    return sht_temperature;
}

signed short sht_get_dewpoint(){
    return sht_dewpoint;
}

unsigned char sht_get_humidity(){
    return sht_humidity;
}
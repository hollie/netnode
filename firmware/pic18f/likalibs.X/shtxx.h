#include <math.h>

#define MaxNrSensors 5 // decrease this value if you want to reduce the mem footprint
#define ScanMask 0x1F  // The bit into this mask set to 1 will be used as clock lines on portC
                       // These It does not mean that a sensor should be presented
                       // These pins will be scanned to find out if a sensor is connected.
                       // RC5 is used as data line
                       // RC6 & 7 are no used. So the max nr of sensors connected = 5.

typedef struct sht_data {
    unsigned char  mask;
    int            temperature;
    char           humidity;
    unsigned char  crc;
    unsigned char  valid;
} sht_tdata;


void           sht_Start(void);			// Funky use of capitals to be compliant with other PSOC libs
char           sht_do_measure(unsigned char Sensor);
int            sht_getCurrentTemp(unsigned char Sensor);
unsigned char  sht_getCurrentHumi(unsigned char Sensor);
char           sht_getCurrentCRC(unsigned char Sensor);
unsigned char  sht_getCurrentMask(unsigned char Sensor);
unsigned char  sht_init(void);
unsigned char  sht_SetSensor(unsigned char Sensor);
void           sht_crc_init(void);
unsigned char  sht_softreset(void);
char           sht_read_statusreg(void);
char           sht_GetDeviceCount(void);

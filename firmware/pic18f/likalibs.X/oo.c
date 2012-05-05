/*************************************************************
* One wire interface lib for the Dallas DS1820 and alike
*
* (c) 2005-2010, Lieven Hollevoet, mcc18 compiler.
*************************************************************/

#include <p18cxxx.h>
#include <stdio.h>
#include <delays.h>
#include "oo.h"

unsigned char oo_scratchpad[9];
// Scratchpad contents
// 0	Temperature LSB
// 1	Temperature MSB
// 2	Hi alarm temperature
// 3	Lo alarm temperature
// 4	Reserved, 0xFF
// 5	Reserved, 0xFF
// 6	Remainder register
// 7	Nr of counts per degree
// 8	CRC of pad contents


// CRC working variable
unsigned char crc = 0;

// CRC lookup table
rom unsigned char crc_rom[256] = {0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65, 157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220, 35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98, 190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255, 70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7, 219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154, 101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36, 248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185, 140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205, 17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80, 175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238, 50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115, 202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139, 87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22, 233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168, 116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53};


char oo_rx_bit();
char oo_rx_byte();
void oo_tx_bit(char bit);
void oo_tx_byte(char byte);

// Internal variables
unsigned char id[8];
signed char conflict;
char        new_conflict;

char        nr_of_devices = 0;
oo_tdata    device_table[OO_SUPPORTED_DEVICE_COUNT];

#warning "Assuming clock frequency of 32 MHz, adjust the delays accordingly when using another frequency"	


////////////////////////////////////////////////////////////
// oo_busreset()
// 
// Issues a 'ping' on the bus. If there is at least one
// sensor on the bus, it will generate a presence pulse.
// Returns: 0 if a presence pulse was detected
//          1 if no device was detected
////////////////////////////////////////////////////////////
char oo_busreset(){
	
	OO_BUS = 0;			// Send the reset pulse
	oo_writemode;		// therefore, make port low
	
	Delay100TCYx(48);	// Wait for 600 µs, this is the reset pulse
	
	oo_readmode;		// Release bus
	
	Delay100TCYx(8);	// Wait 60 us (release) + 40 us (let device drive the bus)
	
	// Now sample, if there is a sensor on the bus, the line should be low
	if (OO_BUS){
		return 1; // <<-error
	}
	
	Delay100TCYx(32);	// Wait another 400 us to complete the reset sequence
	
	return 0;
	
}

////////////////////////////////////////////////////////////
// oo_tx_bit(bit value)
// 
// Transmits a single bit over the bus
////////////////////////////////////////////////////////////
void oo_tx_bit(char value){

	// Pull bus low
	OO_BUS = 0;
	oo_writemode;
	
	// Start the write slot (should take ~1us)
	Delay10TCYx(1);
	
	if (value == 0){
		// Write a '0' (hold the bus for another 60 us)
		Delay10TCYx(48);
	}
	
	// Release the bus
	oo_readmode;
	
	// Final wait for the bus to stabilise (10 us)
	Delay10TCYx(8);
	
	// Make sure a write 0 and a write 1 take the same time to complete
	if (value == 1){
		Delay10TCYx(48);
	}
	
	return;
}

////////////////////////////////////////////////////////////
// char oo_rx_bit()
// 
// Receives a single bit from the bus
////////////////////////////////////////////////////////////
char oo_rx_bit(){

	char value = 0;
	
	// Bus low
	OO_BUS = 0;
	oo_writemode;
		
	// Wait 1 us
	Delay10TCYx(1);

	// Release bus
	oo_readmode;
		
	// Wait 10 us before sampling
	Delay10TCYx(8);

	value = OO_BUS;
	
	// Wait for the end of the read time slot (50 us)
	Delay10TCYx(40);
	
	return value;
}

////////////////////////////////////////////////////////////
// oo_tx_byte(char data)
// 
// Transmits a single byte from the bus
////////////////////////////////////////////////////////////
void oo_tx_byte(char data){
	char counter = 0;
	while (counter < 8){
		if (data & 0x01){
			oo_tx_bit(1);
		} else {
			oo_tx_bit(0);
		}
		
		data = data >> 1;
		counter++;
	}
}

////////////////////////////////////////////////////////////
// char oo_rx_byte()
// 
// Receives a single byte from the bus
////////////////////////////////////////////////////////////
char oo_rx_byte(){

	char counter = 0;
	char data    = 0;
	char  oo_bit  = 0;
	
	while (counter < 8){
	
		// Rx bit
		oo_bit = oo_rx_bit();
		
		// Shift and add one depending on the value of the bus
		data = data >> 1;
		
		if (oo_bit){
			data |= 0x80;
		}
		
		counter++;
		
	}

	oo_crc_shuffle_byte(data);

	return data;
}

////////////////////////////////////////////////////////////
// oo_crc_init()
//
// Initialise the CRC working register to be able to 
// start a new calculation
////////////////////////////////////////////////////////////
void oo_crc_init(){
	crc = 0;
}

////////////////////////////////////////////////////////////
// oo_crc_shuffle_byte()
//
// Shuffle the next byte into the CRC
////////////////////////////////////////////////////////////
void oo_crc_shuffle_byte(unsigned char input){
	unsigned char index = crc ^ input;
	crc = crc_rom[index];
}

////////////////////////////////////////////////////////////
// char oo_get_devicecount()
//
// Returns the number of detected devices
////////////////////////////////////////////////////////////
char oo_get_devicecount(){
	return nr_of_devices;
}

////////////////////////////////////////////////////////////
// void oo_set_devicecount()
//
// Sets the number of detected devices, private function
////////////////////////////////////////////////////////////
void oo_set_devicecount(unsigned char count){
	nr_of_devices = count;
	return;
}

////////////////////////////////////////////////////////////
// char oo_read_scratchpad()
// 
// Reads the scratchpad of the currently selected device
////////////////////////////////////////////////////////////
char oo_read_scratchpad(){

	char counter = 0;
	
	// Read the scratchpad
	oo_tx_byte(OO_READPAD);
	
	// Reset the CRC register, CRC is updated in the oo_rx_byte() function.
	oo_crc_init();
	
	while (counter < 9){
		oo_scratchpad[counter] = oo_rx_byte();
		counter++;
	}

	// Verify the CRC
	return crc;

	
	return 0;
}


////////////////////////////////////////////////////////////
// oo_conversion_busy()
// 
// DS1820 specific
// Returns 1 if a sensor is still processing
//         0 if all sensors are done
////////////////////////////////////////////////////////////
char oo_conversion_busy(){
	if (oo_rx_byte() == 0x00){
		return 1;
	} else {
		return 0;
	}
}

////////////////////////////////////////////////////////////
// oo_start_conversion()
// 
// DS1820 specific
// Commands all sensors to start a temperature conversion
////////////////////////////////////////////////////////////
void oo_start_conversion(){
	// Reset the bus so everyone is listening
	oo_busreset();

	// Command all temp sensors on the bus to start a conversion
	oo_tx_byte(OO_SKIPROM);
	
	Delay1KTCYx(10);

	// Convert temperature
	oo_tx_byte(OO_CONVERTT);
	
	return;
}

////////////////////////////////////////////////////////////
// char oo_wait_for_completion()
// 
// DS1820 specific
// Wait for the completion of the temperature conversion
// returns 0 is success, returns 1 when a timeout occurred.
////////////////////////////////////////////////////////////
char oo_wait_for_completion(){

	unsigned char counter = 0;
	
	while (oo_conversion_busy()){
		// Security: if the conversion is not completed
		// after > 1 sec -> break.
		Delay1KTCYx(31);
		counter++;
		if (counter == 0xFF){
			return 1;
		}	
	}
	
	return 0;
}


////////////////////////////////////////////////////////////
// oo_get_next_id()
//
// Helper function for the scanbus option. Gets the next 
// id on the bus.
////////////////////////////////////////////////////////////
char oo_get_next_id(){

	char val0;
	char val1;
	char counter    = 0;
	char bit_index  = 0;
	char byte_index = 0;
	char current_conflict = -1;

	new_conflict = 0;
	
	// Reset the bus
	if (oo_busreset()){
		return 1; // -> error
	}
		
	// Issue the search ROM command
	oo_tx_byte(OO_SEARCHROM);
	
	// Start the search for the sensor	
	for (byte_index = 0; byte_index < 8; byte_index++){
		
		char data_in_byte = 0;
		
		for (bit_index = 0; bit_index < 8; bit_index++){
	
			// Read 2 consecutive bits from the One Wire bus
			val0 = oo_rx_bit();
			val1 = oo_rx_bit();

			// Evaluate the result
			if ((val0 == 0) && (val1 == 0)){
				// meepmeep, conflict
				//serial_printf("C");
				current_conflict = counter;
			
				// if conflict occurs earlier than the previous conflict, then use the previous value
				if (current_conflict < conflict) {
					// use previous value
					if (id[byte_index] && (1 << bit_index)){
						val0 = 1;
					} else {
						val0 = 0;
					}
				} else if (current_conflict == conflict) {
					// use 1
					val0 = 1; 
				} else {
					// use zero on new conflicts
					val0 = 0;
					conflict = current_conflict;
					new_conflict = 1;
				}
				
			} else if (val0 && val1){
				//serial_printf("H");
				return 1;
			}
			
			// Store the bit in the ID array
			data_in_byte = data_in_byte >> 1;
			if (val0){
				data_in_byte |= 0x80;
			}
			oo_tx_bit(val0);
			//serial_print_dec(val0);
		
			counter++;
			
		}

		id[byte_index] = data_in_byte;

	}
	
	//serial_print_lf();
	
	// Verify ID CRC
	oo_crc_init();
	
	//printf("Found ID: ");
	for (byte_index = 0; byte_index < 8; byte_index++){
	//	printf("%02X", id[byte_index]);
		oo_crc_shuffle_byte(id[byte_index]); 
	}
	
	//serial_print_lf();
	
	// Bail on CRC errors
	if (crc) {
		//printf("CRC error\n");
		return crc;
	}
	
	return 0;
}

////////////////////////////////////////////////////////////
// oo_scanbus()
//
// Scan the bus to detect all devices
// This function stores the following information in the 
// memory:
//  => the number of IDs detected
//  => IDs of the devices, one ID takes 8 bytes
////////////////////////////////////////////////////////////
char oo_scanbus(void){

	char retval;
	char device_count = 0;
	char loper;
	
	oo_set_devicecount(0);

	conflict = -1;
	new_conflict = 1;
	
	// Reset the bus, wake up devices.
	if (oo_busreset()){
		return 1; // -> error
	}
	
	// new_conflict gets updated in the oo_get_next_id() function.
	while (new_conflict){
		retval = oo_get_next_id(); 

		// Bail in case of errors
		if (retval) return retval;
		

		// Add the device id to the device table
		for (loper=0; loper < 8; loper++){
			device_table[device_count].id[loper] = id[loper];
		}
		
		device_count++;
		
	}
	
	// Update the number of devices
    oo_set_devicecount(device_count);
	
	return 0;
	
}



////////////////////////////////////////////////////////////
// char oo_read_device()
//
// Reads the temperature of the device 'count' and update 
// the device table
//
//  Returns 1 in case the devicecount is too high
//  Returns 2 in case of a CRC error, in this case the
//     valid bit of the device is also put to '0' in 
//     the device hash
////////////////////////////////////////////////////////////
unsigned char oo_read_device(unsigned char count){

	unsigned char loper;
	unsigned char id_byte;
	short         temperature;

	if (count > oo_get_devicecount()) {
		return 1;
	}

	device_table[count].valid = 0;
		
	oo_busreset();
	oo_tx_byte(OO_MATCHROM);
	//serial_printf("Going for matchrom\n");
	
	for (loper=0; loper < 8; loper++){
		id_byte = device_table[count].id[loper];
		oo_tx_byte(id_byte);
	}
	
	// Read the scratchpad of the selected device
	// The scratchpad is defined as a global variable to reduce the amount of data pushed on the heap
	if (oo_read_scratchpad()){
		return 2;
	}
	
	/*
	// Debug: print the scratchpad contents
	printf("Scratchpad contents:\r\n");
	for (loper=0; loper<9; loper++){
		printf("%i: %02X\r\n", loper, oo_scratchpad[loper]);
	}
	*/

	// And extract the temperature information
	temperature = (short)oo_scratchpad[1];
	temperature = temperature << 8;
	temperature += (short)oo_scratchpad[0];	
	device_table[count].temperature = temperature;
	device_table[count].valid = 1;
	return 0;
	
}


// Get a system report from the OneWire bus
// This means: 
//  * start conversion on all devices
//  * wait for completion
//  * read the scratchpad of the first device, extract temperature data and update the device hash
//  * continue until the last sensor is read
void oo_read_temperatures(void){
	
	oo_tdata data;
	
	unsigned char id[8];
	char loper=0;
	unsigned char index;

	
	// Start conversion
	oo_start_conversion();
	
	// Wait for completion
	if (oo_wait_for_completion()){
		//printf("Timed out while waiting for conversion!\n");
		return;
	}
	
	// Find the first device on the bus
	while (loper < oo_get_devicecount()){
		oo_read_device(loper);
		loper++;
	}
		
}

// Init routine for the onewire bus.
// This function should be called at startup of the controller
// It will initialize the bus and scan for devices
// Returns zero when no errors were detected
// Returns 1 when there a timeout during temperature conversion
unsigned char oo_init(void){

	// Set the bus and check if a device is present
	// If there is one, oo_scanbus will return a '0'. 
	// On error it reports a non-zero value
	return oo_scanbus();
}

// Get the info of a device so that user code can extract the data for 
// a specific sensor
oo_tdata oo_get_device_info(unsigned char index){

	if (index>oo_get_devicecount()){
		index = 0;
	}

	return device_table[index];
}

signed short oo_get_device_temp(unsigned char index){

	if (index>oo_get_devicecount()){
		index = 0;
	}

	return device_table[index].temperature;

}

// Print a device status line
// Format IDIDIDID - TTTT - C
//  ID = 64-bit ID
//  TT = 16-bit temperature, signed, quantisation depends on device type
//  C  = CRC valid (1 or 0)
void oo_print_device_info(unsigned char index){
	char loper;
	for (loper=0; loper<8; loper++){
		printf("%02X", device_table[index].id[loper]);
	}
	printf(" - %04X - %X\r\n", device_table[index].temperature, device_table[index].valid);
}
/*
 * Connect and send messages to the Cosm.com socket service
 * through a Lantronix XPort device
 *
 * Author: Lieven Hollevoet, LikaTronix.be
 *
 * License: http://creativecommons.org/licenses/by-nc-sa/3.0/
 */

#include <p18cxxx.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>
#include "cosm_socket.h"



enum UART_STATE_TYPE { IDLE = 0, WAIT_CONNECT, CONNECTED, INCOMING, WAITING_INFO, STORE_STRING, STRING_RECEIVED, WAIT_FOR_DISCONNECT};
enum UART_STATE_TYPE uart_state;
short uart_length;

char* api_key;
unsigned int feed_id;


//////////////////////////////////////////////////////////////////
// Init the library
//////////////////////////////////////////////////////////////////
void cosm_init(char* auth_key, unsigned int feed) {
    uart_length=0;
    uart_state = IDLE;
    api_key = auth_key;
    feed_id = feed;
    return;
}


//////////////////////////////////////////////////////////////////
// Open a TCP connection through the XPORT
//////////////////////////////////////////////////////////////////

void cosm_connect(void) {

    char counter = 0;

    if (uart_state == IDLE) {
        uart_state = WAIT_CONNECT;
        printf("C216.52.233.122/8081\n"); // DNS is not working reliably, so we hardcode the server IP address here
        while (uart_state == WAIT_CONNECT) {
            counter++;
            // Sleep a while
            Delay10KTCYx(80);

            // Timeout, just in case we get stuck here.
            if (counter > 10 ) {
                uart_state = IDLE;
                return;
            }
        }
    }

    return;
}


//////////////////////////////////////////////////////////////////
// Send the current sensor information
// If we want to report a floating point value, we pass the
// value multiplied by 100 and we scale in this function
// by setting the scale value to '1'
//////////////////////////////////////////////////////////////////

void cosm_report(char *name, signed short value, char scale) {

    signed short value_abs;
    signed short value_whole;
    signed short value_part;

    cosm_connect();
    if (uart_state != CONNECTED) {
        return;
    }

    if (scale) {
        value_abs = value;
        if (value < 0) {
            value_abs = -value;
        }
        value_whole =  (value_abs / 100);
        value_part  =  (value_abs - value_whole * 100);
    }

    // Report the sensor value
    printf("{\n\"method\":\"put\",\n");
    printf("\"resource\":\"/feeds/%u\",\n",feed_id);
    printf("\"params\":{},\n");
    printf("\"headers\":{\"X-PachubeApiKey\":\"%s\"},\n", api_key);
    printf("\"body\":{\n \"version\":\"1.0.0\",\n \"datastreams\":[\n {\n  ");
    printf("\"id\":\"%s\",\n  ", name);
    printf("\"current_value\":\"");
    if (scale) {
        if (value < 0) {
            printf("-");
        }
        printf("%i.%02i", value_whole, value_part);
    } else {
        printf("%i", value);
    }
    printf("\"\n }\n ]\n},\n\"token\":\"testing*\"\n}\n");

    return;
}





///////////////////////////////////////////////////////////////////////
// State machine to process that UART input
// This function takes care of the interface towards the XPORT
// that has been put in 'manual connection' mode.
///////////////////////////////////////////////////////////////////////

void cosm_process_uart(char data) {

    uart_length++;

    switch (data) {
        case 'C': // When a connection is made, the XPORT sends a 'C'
            if (uart_state == WAIT_CONNECT || uart_state == IDLE) {
                uart_state = CONNECTED;
            }

            return;
        case 'I': // When the next character is an 'I', it is an active incoming connection. Handle it...
            if (uart_state == CONNECTED && uart_length == 2) {
                uart_state = INCOMING;
            }
            return;
        //case '*': // End of the string that needs to be stored
            //if (uart_state == STORE_STRING){
        //    uart_state = WAIT_FOR_DISCONNECT;
            //}
        //    return;
        case 'D':
            //if (uart_state == WAIT_FOR_DISCONNECT){
                uart_length=0;
                uart_state = IDLE;
            //    return;
            //}
        default:
            switch (uart_state) {
                case WAIT_CONNECT:
                    if (data == 'N') { // If the connection failed, reset the state machine
                        uart_state = IDLE;
                        uart_length = 0;
                        return;
                    }
                default:
                    
                    return;

            }

    }

}

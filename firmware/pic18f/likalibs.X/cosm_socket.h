/*
 * Connect and send messages to the Cosm.com socket service
 * through a Lantronix XPort device
 *
 * Author: Lieven Hollevoet, LikaTronix.be, 2012
 *
 * License: http://creativecommons.org/licenses/by-nc-sa/3.0/
 *
 */

#ifndef _COSM_SOCKET_H_
#define _COSM_SOCKET_H_

void cosm_init(char* auth_key, unsigned int feed);
void cosm_process_uart(char data);
void cosm_report(char* name, signed short value, char scale);

#endif // _COSM_SOCKET_H_

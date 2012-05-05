/*************************************************************
* 24h clock routines
*
* Keeps track of the current time 
*
* (c) 2006, Lieven Hollevoet
*     http://electronics.lika.be
*
* Toolsuite: Microchip C18
*************************************************************/

#ifndef _CLOCK_H_
#define _CLOCK_H_

void clock_increment(void);
void clock_set(char day, char hours, char mins, char secs);
void clock_clear(void);
void clock_print(void);
char clock_get_minutes(void);
char clock_get_seconds(void);
char clock_get_hours(void);
char clock_get_day(void);

#endif // _CLOCK_H_

/*************************************************************
* 24h clock routines
*
* Keeps track of the current time 
*
* (c) 2009, Lieven Hollevoet
*     http://electronics.lika.be
*
* Toolsuite: Microchip C18
*************************************************************/

#include <stdio.h>

#include "clock.h"

char clock_day;
char clock_hours;
char clock_mins;
char clock_secs;

// Called every second
void clock_increment(){
	clock_secs++;
	if (clock_secs >= 60) {
		clock_mins++;
		clock_secs = 0;
	}
	if (clock_mins >= 60) {
		clock_hours++;
		clock_mins = 0;
	}
	if (clock_hours >= 24) {
		clock_hours = 0;
		clock_day++;
		if (clock_day > 6) {
			clock_day = 0;
		}
	}
		
	return;
}

void clock_set(char day, char hours, char mins, char secs){
	clock_day   = day;
	clock_hours = hours;
	clock_mins  = mins;
	clock_secs  = secs;
	
	return;
}

void clock_clear(){
	clock_day   = 0;
	clock_secs  = 45;
	clock_mins  = 0;
	clock_hours = 0;
	
	return;
}

void clock_print(){
	printf("%d-%02d:%02d.%02d\n\r", clock_day, clock_hours, clock_mins, clock_secs);
	return;	
}

char clock_get_minutes(){
	return clock_mins;
}

char clock_get_seconds(){
	return clock_secs;
}

char clock_get_hours(){
	return clock_hours;	
}

char clock_get_day(){
	return clock_day;
}
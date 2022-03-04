/*
 * event_log.h
 *
 *  Created on: 2022年3月3日
 *      Author: Administrator
 */

#ifndef EVENT_LOG_H_

#include "stdint.h"


typedef enum event_s{
	BUTTON_PREES	= 0,
	BUTTON_RELEASE	= 1
}event_t;

typedef struct eventLog_s{
	uint32_t time;
	event_t event;
}eventLog_t;


void log_write(eventLog_t event);
void log_print();




#define EVENT_LOG_H_



#endif /* EVENT_LOG_H_ */

/*
 * event_log.c
 *
 *  Created on: 2022年3月3日
 *      Author: Administrator
 */
#include "event_log.h"
#include "cJSON.h"
#include "stdio.h"
#include "stdlib.h"


#define LOG_NUM 10

static eventLog_t eventLog[LOG_NUM] = {0};
static uint8_t logPtr = 0;
static uint8_t isFull = 0;

void log_write(eventLog_t event){
	eventLog[logPtr] = event;
	logPtr += 1;
	if (logPtr == LOG_NUM){
		logPtr = 0;
		isFull = 1;
	}
}

void log_print(){

	uint8_t i = 0;
	if(isFull){
		i = logPtr;
	}else if(logPtr == 0){
		return;//None log
	}
	cJSON *root = cJSON_CreateObject();
	cJSON *arr	= cJSON_CreateArray();
	cJSON *child[LOG_NUM];
	char *buffer = NULL;
	do{
		child[i] = cJSON_CreateObject();
		switch(eventLog[i].event){
			case BUTTON_PREES:
				cJSON_AddNumberToObject(child[i], "time", eventLog[i].time);
				cJSON_AddStringToObject(child[i], "event", "BUTTON_PREES");
				break;
			case BUTTON_RELEASE:
				cJSON_AddNumberToObject(child[i], "time", eventLog[i].time);
				cJSON_AddStringToObject(child[i], "event", "BUTTON_RELEASE");
				break;
			default:
				break;
		}
		cJSON_AddItemToArray(arr, child[i]);
		i++;
		if (i == LOG_NUM){
			i = 0;
		}
	}while(i != logPtr);
	cJSON_AddItemToObject(root,"log",arr);
	buffer = cJSON_Print(root);
	cJSON_Delete(root);
	printf("jsonLog: %s\r\n", buffer);
	free(buffer);

}

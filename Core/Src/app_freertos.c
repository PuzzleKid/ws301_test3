/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "app_lorawan.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lora_app.h"
#include "usart.h"
#include <stdio.h>
#include "time.h"
#include "cJSON.h"
#include <stdlib.h>
#include "event_log.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define	RECEIVE_MAX 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static uint8_t rxData[RECEIVE_MAX] = {0};
static uint16_t rxLen = 0;
eventLog_t evelog;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for nfcTask */
osThreadId_t nfcTaskHandle;
const osThreadAttr_t nfcTask_attributes = {
  .name = "nfcTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 1024 * 4
};
/* Definitions for systemTask */
osThreadId_t systemTaskHandle;
const osThreadAttr_t systemTask_attributes = {
  .name = "systemTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 768 * 4
};
/* Definitions for processQueue */
osMessageQueueId_t processQueueHandle;
const osMessageQueueAttr_t processQueue_attributes = {
  .name = "processQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void ws301Process(void);
void serialProcess(void);
void memcpy8to16(uint16_t *dst, const uint8_t *src, uint8_t size);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartNFCTask(void *argument);
void StartSystemTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of processQueue */
  processQueueHandle = osMessageQueueNew (4, sizeof(processRequest_t), &processQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of nfcTask */
  nfcTaskHandle = osThreadNew(StartNFCTask, NULL, &nfcTask_attributes);

  /* creation of systemTask */
  systemTaskHandle = osThreadNew(StartSystemTask, NULL, &systemTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LoRaWAN */
  MX_LoRaWAN_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	ws301Process();
	MX_LoRaWAN_Process();
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartNFCTask */
/**
* @brief Function implementing the nfcTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartNFCTask */

void StartNFCTask(void *argument)
{
  /* USER CODE BEGIN StartNFCTask */

  /* Infinite loop */
  for(;;)
  {

	  if(HAL_GPIO_ReadPin(SWITCH2_GPIO_Port, SWITCH2_Pin) == SWITCH2_PRESS){
		  osDelay(10);
		  if(HAL_GPIO_ReadPin(SWITCH2_GPIO_Port, SWITCH2_Pin) == SWITCH2_PRESS){
			  LED_GREEN_TOGGLE();
			  evelog.time = SysTimeGet().Seconds;
			  evelog.event = BUTTON_PREES;
			  log_write(evelog);
			  while(HAL_GPIO_ReadPin(SWITCH2_GPIO_Port, SWITCH2_Pin) == SWITCH2_PRESS){
				  osDelay(100);
			  }
			  evelog.time = SysTimeGet().Seconds;
			  evelog.event = BUTTON_RELEASE;
			  log_write(evelog);
		  }
	  }
	  osDelay(10);
  }
  /* USER CODE END StartNFCTask */
}

/* USER CODE BEGIN Header_StartSystemTask */
/**
* @brief Function implementing the systemTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSystemTask */
void StartSystemTask(void *argument)
{
  /* USER CODE BEGIN StartSystemTask */
	HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, rxData, RECEIVE_MAX);
  /* Infinite loop */
  for(;;)
  {
	  serialProcess();
      osDelay(10);
  }
  /* USER CODE END StartSystemTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    rxLen = Size;
    if (rxLen >= RECEIVE_MAX){
    	log_err("rx overflow!\r\n");
    }else{
    	rxData[rxLen] = 0;
    }

    HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, rxData, RECEIVE_MAX);
}

void ws301Process(void){
	static osStatus_t result;
	static processRequest_t msg_ptr;
	static uint8_t Buffer[256] = {0};
	static LmHandlerAppData_t appData = {0,0,Buffer};
	static TimerTime_t nextTxIn = 0;

	LmHandlerErrorStatus_t errStatus;

	if (osMessageQueueGetCount (processQueueHandle) != 0){

		result = osMessageQueueGet (processQueueHandle, &msg_ptr, NULL, 0);
		if(result != osOK){
			log_err("get msg fail:%d\r\n",result);
			return;
		}
		switch(msg_ptr.RequestType){
			case MSG_JOIN:
				log("rec join:%d\r\n",msg_ptr.param.ActivationType);
				LmHandlerJoin(msg_ptr.param.ActivationType);
				break;
			case MSG_SEND:
				log("rec send\r\n");

				appData.Port = msg_ptr.param.SendPacket.Port;
				appData.BufferSize = msg_ptr.param.SendPacket.BufferSize;
//				printf("appData.BufferSize:%d\r\n",appData.BufferSize);

				memcpy1(appData.Buffer, msg_ptr.param.SendPacket.Buffer, appData.BufferSize );
//				printf("appData.Buffer:%d\r\n",appData.Buffer[0]);
				LmHandlerSend(&appData, msg_ptr.param.SendPacket.MsgType,&nextTxIn, \
						msg_ptr.param.SendPacket.allowDelayedTx );
				break;
			case MSG_READ_CONFIG:
				errStatus = LmHandlerGetDevAddr(&msg_ptr.param.devAddr);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetDevAdd fail:%d\r\n",errStatus);
				}else{
					log("devAddr:%08X\r\n",msg_ptr.param.devAddr);
				}
				errStatus = LmHandlerGetDevEUI(msg_ptr.param.devEUI);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetDevEUI fail:%d\r\n",errStatus);
				}else{
					log("###### DevEui:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX8(msg_ptr.param.devEUI));
				}
				errStatus = LmHandlerGetAppEUI(msg_ptr.param.appEUI);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetAppEUI fail:%d\r\n",errStatus);
				}else{
					log("###### AppEui:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX8(msg_ptr.param.appEUI));
				}
				errStatus = LmHandlerGetNwkKey(msg_ptr.param.appKey);//AppKey->NwkKey
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetAppKey fail:%d\r\n",errStatus);
				}else{
					log("###### AppKey:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX16(msg_ptr.param.appKey));
				}
				errStatus = LmHandlerGetAppSKey(msg_ptr.param.appSKey);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetAppSKey fail:%d\r\n",errStatus);
				}else{
					log("###### AppSKey:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX16(msg_ptr.param.appSKey));
				}
				errStatus = LmHandlerGetNwkSKey(msg_ptr.param.nwkSKey);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetNwkSKey fail:%d\r\n",errStatus);
				}else{
					log("###### NwkSKey:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX16(msg_ptr.param.nwkSKey));
				}
				errStatus = LmGetDefaultChannelMask(msg_ptr.param.defChannel);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetDefChannel fail:%d\r\n",errStatus);
				}else{
					log("###### DefChannel:  %04X:%04X:%04X:%04X:%04X:%04X\r\n",HEX8(msg_ptr.param.defChannel));
				}

				errStatus = LmGetChannelMask(msg_ptr.param.channel);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("GetChannel fail:%d\r\n",errStatus);
				}else{
					log("###### Channel:  %04X:%04X:%04X:%04X:%04X:%04X\r\n",HEX8(msg_ptr.param.channel));
				}
				break;
			case MSG_DEV_ADDR:
				errStatus = LmHandlerSetDevAddr(msg_ptr.param.devAddr);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetDevAddr fail:%d\r\n",errStatus);
				}else{
					log("SetDevAddr:%08X\r\n",msg_ptr.param.devAddr);
				}
				break;
			case MSG_DEV_EUI:
				errStatus = LmHandlerSetDevEUI(msg_ptr.param.devEUI);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetDevEUI fail:%d\r\n",errStatus);
				}else{
					log("SetDevEUI:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX8(msg_ptr.param.devEUI));
				}
				break;
			case MSG_APP_EUI:
				errStatus = LmHandlerSetAppEUI(msg_ptr.param.appEUI);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetAppEUI fail:%d\r\n",errStatus);
				}else{
					log("SetAppEUI:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX8(msg_ptr.param.appEUI));
				}
				break;
			case MSG_APP_KEY:
				errStatus = LmHandlerSetNwkKey(msg_ptr.param.appKey);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetAppKey fail:%d\r\n",errStatus);
				}else{
					log("SetAppKey:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX16(msg_ptr.param.appKey));
				}
				break;
			case MSG_APP_SKEY:
				errStatus = LmHandlerSetAppSKey(msg_ptr.param.appSKey);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetAppSKey fail:%d\r\n",errStatus);
				}else{
					log("SetAppSKey:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX16(msg_ptr.param.appSKey));
				}
				break;
			case MSG_NWK_SKEY:
				errStatus = LmHandlerSetNwkSKey(msg_ptr.param.nwkSKey);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetNwkSKey fail:%d\r\n",errStatus);
				}else{
					log("SetNwkSKey:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",HEX16(msg_ptr.param.nwkSKey));
				}
				break;
			case MSG_DEFAULT_CHANNEL:
				errStatus = LmSetDefaultChannelMask(msg_ptr.param.defChannel);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetDefChannel fail:%d\r\n",errStatus);
				}else{
					log("SetDefChannel:%04X:%04X:%04X:%04X:%04X:%04X\r\n",HEX6(msg_ptr.param.defChannel));
				}
				break;
			case MSG_CHANNEL:
				errStatus = LmSetChannelMask(msg_ptr.param.channel);
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("SetChannel fail:%d\r\n",errStatus);
				}else{
					log("SetChannel:%04X:%04X:%04X:%04X:%04X:%04X\r\n",HEX6(msg_ptr.param.channel));
				}
				break;
			case MSG_STOP:
				errStatus = LmHandlerStop();
				if (errStatus != LORAMAC_HANDLER_SUCCESS){
					log_err("LmHandlerStop fail:%d\r\n",errStatus);
				}else{
					log("LmHandlerStop\r\n");
				}
				break;
//			case MSG_WRITE_JSON:
//				if (LmHandlerDeviceTimeReq() != LORAMAC_HANDLER_SUCCESS){
//				  MW_LOG(TS,VL,"DeviceTimeReq ERROR\r\n");
//				}
//				break;
			case MSG_RESERVED:
				break;
			default:
				break;
		}

	}
}
void serialProcess(void){
	static osStatus_t result;
    if (rxLen)
    {
//		  printf("READ[%s][%d]\r\n", rxData, rxLen);

  	  printf_arr(rxData,rxData,rxLen);

      if (rxLen > 3){
      	  if (rxData[0]==0x7e && rxData[2]==(rxLen-4) && rxData[rxLen-1]==0x7e){
      		  processRequest_t msg = {0};
      		  SysTime_t sysTime = {0};
      		  SysTime_t mcuTime = {0};
      		  switch(rxData[1]){

				  case CMD_JOIN_ABP:
					  log("ABP\r\n");
					  msg.RequestType = MSG_JOIN;
					  msg.param.ActivationType = ACTIVATION_TYPE_ABP;
					  break;
				  case CMD_JOIN_OTAA:
					  log("OTAA\r\n");
					  msg.RequestType = MSG_JOIN;
					  msg.param.ActivationType = ACTIVATION_TYPE_OTAA;
					  break;
				  case CMD_SEND_UNCONF:
					  log("UNCONF\r\n");
					  msg.RequestType = MSG_SEND;
					  msg.param.SendPacket.MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG;
					  msg.param.SendPacket.Port = 2;
					  msg.param.SendPacket.BufferSize = rxData[2];
					  memcpy1( msg.param.SendPacket.Buffer, &rxData[3], rxData[2] );

					  break;
				  case CMD_SEND_CONF:
					  log("CONF\r\n");
					  msg.RequestType = MSG_SEND;
					  msg.param.SendPacket.MsgType = LORAMAC_HANDLER_CONFIRMED_MSG;
					  msg.param.SendPacket.Port = 2;
					  msg.param.SendPacket.BufferSize = rxData[2];
					  memcpy1( msg.param.SendPacket.Buffer, &rxData[3], rxData[2] );

					  break;
				  case CMD_READ_CONFIG:
					  log("CMD_READ_CONFIG\r\n");
					  msg.RequestType = MSG_READ_CONFIG;
					  break;

				  case CMD_DEV_ADDR:
					  log("CMD_DEV_ADDR\r\n");
					  if(rxData[2] == 4){
						  msg.RequestType = MSG_DEV_ADDR;
						  msg.param.devAddr = (rxData[3]<<24)+(rxData[4]<<16)+(rxData[5]<<8)+rxData[6];
						  log("devAddr:%lu \r\n",msg.param.devAddr);
					  }else{
						  log_err("cmd err\r\n");
					  }

					  break;
				  case CMD_DEV_EUI:
					  log("CMD_DEV_EUI\r\n");
					  if(rxData[2] == 8){
						  msg.RequestType = MSG_DEV_EUI;
						  memcpy1( msg.param.devEUI, &rxData[3], rxData[2] );
						  printf_arr("msg.param.devEUI",msg.param.devEUI,8);
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_APP_EUI:
					  log("CMD_APP_EUI\r\n");
					  if(rxData[2] == 8){
						  msg.RequestType = MSG_APP_EUI;
						  memcpy1( msg.param.appEUI, &rxData[3], rxData[2] );
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_APP_KEY:
					  log("CMD_APP_KEY\r\n");
					  if(rxData[2] == 16){
						  msg.RequestType = MSG_APP_KEY;
						  memcpy1( msg.param.appKey, &rxData[3], rxData[2] );
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_APP_SKEY:
					  if(rxData[2] == 16){
						  msg.RequestType = MSG_APP_SKEY;
						  memcpy1( msg.param.appSKey, &rxData[3], rxData[2] );
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_NWK_SKEY:
					  if(rxData[2] == 16){
						  msg.RequestType = MSG_NWK_SKEY;
						  memcpy1( msg.param.nwkSKey, &rxData[3], rxData[2] );
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_DEFAULT_CHANNEL:
					  if(rxData[2] == 12){
						  msg.RequestType = MSG_DEFAULT_CHANNEL;
						  memcpy8to16( msg.param.defChannel, &rxData[3], 6 );
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_CHANNEL:
					  if(rxData[2] == 12){
						  msg.RequestType = MSG_CHANNEL;
						  memcpy8to16( msg.param.channel, &rxData[3], 6 );
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_ADR:
					  if(rxData[3] == 0){
						  msg.RequestType = MSG_CHANNEL;
						  msg.param.adrEnable = 0;
					  }else if(rxData[3] == 1){
						  msg.RequestType = MSG_CHANNEL;
						  msg.param.adrEnable = 1;
					  }else{
						  log_err("cmd err\r\n");
					  }
					  break;
				  case CMD_STOP:
					  log("CMD_STOP\r\n");
					  msg.RequestType = MSG_STOP;
					  break;
				  case CMD_DEV_TIME:
					  if (LmHandlerDeviceTimeReq() == LORAMAC_HANDLER_SUCCESS){
					  	  log("DeviceTimeReq SUCCESS\r\n");
					  }else{
						  log_err("DeviceTimeReq ERROR\r\n");
					  }
					break;
				  case CMD_GET_TIME:
					  sysTime = SysTimeGet();
					  log("sysTime:%lu\r\n",sysTime.Seconds);
					  break;
				  case CMD_GET_MCUTIME:
					  mcuTime = SysTimeGetMcuTime();
					  log("mcuTime:%lu\r\n",mcuTime.Seconds);
					  break;
				  case CMD_READ_JSON:
					  log("READ_JSON\r\n");
					  log_print();
					  break;
				  default :
					  log_err("cmd err\r\n");
					  break;


      		  }
      		  if (msg.RequestType != MSG_NONE){
      			  result = osMessageQueuePut (processQueueHandle, &msg, 0, 0);
				  if (result != osOK){
						log_err("put msg fail:%d\r\n",result);
				  }
      		  }

      	  }

      }


      rxLen = 0;
    }
}

void memcpy8to16(uint16_t *dst, const uint8_t *src, uint8_t size){

	 while( size-- )
	 {
	        *dst++ = ((*src++)<<8)+(*src++);
	 }

}
/* USER CODE END Application */

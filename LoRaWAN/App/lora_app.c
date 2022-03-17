/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_app.c
  * @author  MCD Application Team
  * @brief   Application of the LRWAN Middleware
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
#include "platform.h"
#include "Region.h" /* Needed for LORAWAN_DEFAULT_DATA_RATE */
#include "sys_app.h"
#include "lora_app.h"
#include "cmsis_os.h"
#include "LmHandler.h"

/* USER CODE BEGIN Includes */
#include "lora_info.h"
#include "mw_log_conf.h"

#include "main.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  join event callback function
  * @param  joinParams status of join
  */
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);

/**
  * @brief  tx event callback function
  * @param  params status of last Tx
  */
static void OnTxData(LmHandlerTxParams_t *params);

/**
  * @brief callback when LoRa application has received a frame
  * @param appData data received in the last Rx
  * @param params status of last Rx
  */
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

/*!
 * Will be called each time a Radio IRQ is handled by the MAC layer
 *
 */
static void OnMacProcessNotify(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief LoRaWAN handler Callbacks
  */
static LmHandlerCallbacks_t LmHandlerCallbacks =
{
  .GetBatteryLevel =           GetBatteryLevel,
  .GetTemperature =            GetTemperatureLevel,
  .GetUniqueId =               GetUniqueId,
  .GetDevAddr =                GetDevAddr,
  .OnMacProcess =              OnMacProcessNotify,
  .OnJoinRequest =             OnJoinRequest,
  .OnTxData =                  OnTxData,
  .OnRxData =                  OnRxData
};

/* USER CODE BEGIN PV */
static LmHandlerParams_t LmHandlerParams =
{
  .ActiveRegion =             ACTIVE_REGION,
  .DefaultClass =             LORAWAN_DEFAULT_CLASS,
  .AdrEnable =                LORAWAN_ADR_STATE,
  .TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
  .PingPeriodicity =          LORAWAN_DEFAULT_PING_SLOT_PERIODICITY
};
uint8_t devEUI[]={ 0x00, 0x80, 0xE1, 0x01, 0x01, 0x01, 0x01, 0x01 };
uint8_t devEUI_r[]={ 0 };
//static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;
static uint16_t defChannelsMask[] = {0xff00,0x0000,0x0000,0x0000,0x0000,0x0000};
static uint16_t channelsMask[] = {0xff00,0x0000,0x0000,0x0000,0x0000,0x0000};
static uint16_t Channel[6]={ 0 };



/* USER CODE END PV */

/* Exported functions ---------------------------------------------------------*/
/* USER CODE BEGIN EF */
//void SendTxData(void)
//{
//  static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
//  static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };
//  UTIL_TIMER_Time_t nextTxIn = 0;
//  static LmHandlerErrorStatus_t log;
//  AppData.Port = 2;
//  AppData.BufferSize = 8;
//  AppData.Buffer[0] = 0;
//  AppData.Buffer[1] = 1;
//  AppData.Buffer[2] = 0;
//  AppData.Buffer[3] = 1;
//
//  log = LmHandlerSend(&AppData, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, true);
//  if (log != LORAMAC_HANDLER_SUCCESS){
//	  MW_LOG(TS,VL, "SEND REQUEST:%d\r\n",log);
//  }
//
//  if (nextTxIn > 0)
//  {
//    //APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n", (nextTxIn / 1000));
//  }
//}
LmHandlerErrorStatus_t LmGetDefaultChannelMask(uint16_t *ChannelsMask);
LmHandlerErrorStatus_t LmSetDefaultChannelMask(uint16_t *ChannelsMask);
LmHandlerErrorStatus_t LmGetChannelMask(uint16_t *ChannelsMask);
LmHandlerErrorStatus_t LmSetChannelMask(uint16_t *ChannelsMask);
/* USER CODE END EF */

void LoRaWAN_Init(void)
{
  /* USER CODE BEGIN LoRaWAN_Init_1 */
  LoraInfo_Init();
  /* USER CODE END LoRaWAN_Init_1 */

  /* Init the Lora Stack*/
  LmHandlerInit(&LmHandlerCallbacks);

  /* USER CODE BEGIN LoRaWAN_Init_Last */


	if (LmHandlerConfigure(&LmHandlerParams) != LORAMAC_HANDLER_SUCCESS){
		MW_LOG(TS,VL,"LmHandlerConfigure ERROR\r\n");
	}

	if (LmHandlerSetDevEUI(devEUI) != LORAMAC_HANDLER_SUCCESS){
		MW_LOG(TS,VL,"SetDevEUI ERROR\r\n");
	}

	LmHandlerGetDevEUI(devEUI_r);
	MW_LOG(TS_OFF, VLEVEL_M, "###### DevEui:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
		 HEX8(devEUI_r));

	if (LmSetDefaultChannelMask(defChannelsMask) != LORAMAC_HANDLER_SUCCESS){
	  MW_LOG(TS,VL,"SetChannel ERROR\r\n");
	}
	if (LmGetDefaultChannelMask(Channel) != LORAMAC_HANDLER_SUCCESS){
		log_err("GetDefChannel fail:\r\n");
	}else{
		log("###### DefaultChannelMask:  %04X:%04X:%04X:%04X:%04X:%04X\r\n",HEX6(Channel));
	}

	if (LmSetChannelMask(channelsMask) != LORAMAC_HANDLER_SUCCESS){
	  MW_LOG(TS,VL,"SetChannel ERROR\r\n");
	}
	if (LmGetChannelMask(Channel) != LORAMAC_HANDLER_SUCCESS){
		log_err("GetChannel fail:\r\n");
	}else{
		log("###### DeChannelMask:  %04X:%04X:%04X:%04X:%04X:%04X\r\n",HEX6(Channel));
	}

	LmHandlerJoin(ACTIVATION_TYPE_OTAA);


//  LmHandlerConfigure(&LmHandlerParams);
//  LmSetChannel(ChannelsMask);
//  LmHandlerSetDevEUI(devEUI);


  /* USER CODE END LoRaWAN_Init_Last */
}

/* USER CODE BEGIN PB_Callbacks */

/* USER CODE END PB_Callbacks */

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PrFD */
//void SetChannel(void){
//	MibRequestConfirm_t mibReq;
//	mibReq.Type = MIB_CHANNELS_MASK;
//
//	mibReq.Param.ChannelsMask = UserChannelsMask;
//	LoRaMacMibSetRequestConfirm(&mibReq);
//
//	mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
//	mibReq.Param.ChannelsDefaultMask = UserChannelsMask;
//	LoRaMacMibSetRequestConfirm(&mibReq);
//}
LmHandlerErrorStatus_t LmSetDefaultChannelMask(uint16_t *ChannelsMask)
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
        mibReq.Param.ChannelsDefaultMask = ChannelsMask;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}
LmHandlerErrorStatus_t LmSetChannelMask(uint16_t *ChannelsMask)
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        mibReq.Type = MIB_CHANNELS_MASK;
        mibReq.Param.ChannelsMask = ChannelsMask;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}
LmHandlerErrorStatus_t LmGetDefaultChannelMask(uint16_t *ChannelsMask)
{
    MibRequestConfirm_t mibReq;

    if (ChannelsMask == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type =  MIB_CHANNELS_DEFAULT_MASK;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8(ChannelsMask, mibReq.Param.ChannelsDefaultMask, 12);
    return LORAMAC_HANDLER_SUCCESS;
}
LmHandlerErrorStatus_t LmGetChannelMask(uint16_t *ChannelsMask)
{
    MibRequestConfirm_t mibReq;

    if (ChannelsMask == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type =  MIB_CHANNELS_MASK;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8(ChannelsMask, mibReq.Param.ChannelsMask, 12);
    return LORAMAC_HANDLER_SUCCESS;
}
/* USER CODE END PrFD */

static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN OnRxData_1 */
  log("%s\r\n",__FUNCTION__);
	if (appData == NULL){
		log("OnRxData:appData == NULL \r\n");
	}else{
    printf("RxDoneParams.Size:%d\r\n",appData->BufferSize);
    printf("RxDoneParams.Payload:");
    for(uint8_t i = 0; i < appData->BufferSize; i++) {
    	printf("%02X ",appData->Buffer[i]);
    }
    printf("\r\n");
  }
  /* USER CODE END OnRxData_1 */
}

/* USER CODE BEGIN PrFD_LedEvents */

/* USER CODE END PrFD_LedEvents */

static void OnTxData(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN OnTxData_1 */
	log("%s\r\n",__FUNCTION__);
	if (params == NULL){
		log("OnTxData:params == NULL \r\n");
	}else{
		log("Params->Datarate:%d\r\n",params->Datarate);
		log("Params->Status:%d\r\n",params->Status);
		log("Params->Channel:%d\r\n",params->Channel);
	    printf("Params->AppData:");
	    for(uint8_t i = 0; i < params->AppData.BufferSize; i++) {
	    	printf("%02X ",params->AppData.Buffer[i]);
	    }
	    printf("\r\n");
	}

  /* USER CODE END OnTxData_1 */
}

static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
  /* USER CODE BEGIN OnJoinRequest_1 */
	log("%s\r\n",__FUNCTION__);
	if (joinParams == NULL){
		log("OnJoinRequest:params == NULL \r\n");
	}else{
		log("joinParams->Datarate:%d\r\n",joinParams->Datarate);
		log("joinParams->Status:%d\r\n",joinParams->Status);
		log("joinParams->Mode:%d\r\n",joinParams->Mode);
		if(joinParams->Status == 0){
			if (LmHandlerDeviceTimeReq() == LORAMAC_HANDLER_SUCCESS){
				log("DeviceTimeReq SUCCESS\r\n");
				processRequest_t msg = {0};
				msg.RequestType = MSG_SEND;
				msg.param.SendPacket.MsgType = LORAMAC_HANDLER_CONFIRMED_MSG;
				msg.param.SendPacket.Port = 2;
				msg.param.SendPacket.BufferSize = 1;
				msg.param.SendPacket.Buffer[0] = 0x40;
				extern osMessageQueueId_t processQueueHandle;
				if (osMessageQueuePut (processQueueHandle, &msg, 0, 0) != osOK){
					log_err("OnJoinRequest put msg fail\r\n");
				}
		//			uint8_t Buffer[1] = {0x33};
		//			LmHandlerAppData_t appData = {2,1,Buffer};
		//			TimerTime_t nextTxIn;
		//			LmHandlerErrorStatus_t log = LmHandlerSend(&appData, LORAMAC_HANDLER_CONFIRMED_MSG, &nextTxIn, false);
		//			if (log != LORAMAC_HANDLER_SUCCESS){
		//				MW_LOG(TS,VL, "SEND REQUEST:%d\r\n",log);
		//			}
			}else{
				log_err("DeviceTimeReq ERROR\r\n");
			}
		}
	}
  /* USER CODE END OnJoinRequest_1 */
}

static void OnMacProcessNotify(void)
{
  /* USER CODE BEGIN OnMacProcessNotify_1 */
	log("%s\r\n",__FUNCTION__);
  /* USER CODE END OnMacProcessNotify_1 */
}

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
#include "LmHandlerTypes.h"
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
void SetChannel(void);
LmHandlerErrorStatus_t LmSetChannel(uint16_t *ChannelsMask);
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
static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;
static uint16_t ChannelsMask[] = {0xff00,0x00,0x00,0x00,0x00,0x00};
/* USER CODE END PV */

/* Exported functions ---------------------------------------------------------*/
/* USER CODE BEGIN EF */
void SendTxData(void)
{
  static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
  static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };
  UTIL_TIMER_Time_t nextTxIn = 0;
  static LmHandlerErrorStatus_t log;
  AppData.Port = 2;
  AppData.BufferSize = 8;
  AppData.Buffer[0] = 0;
  AppData.Buffer[1] = 1;
  AppData.Buffer[2] = 0;
  AppData.Buffer[3] = 1;

  log = LmHandlerSend(&AppData, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, true);
  if (log != LORAMAC_HANDLER_SUCCESS){
	  MW_LOG(TS,VL, "SEND REQUEST:%d\r\n",log);
  }

  if (nextTxIn > 0)
  {
    //APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n", (nextTxIn / 1000));
  }
}
/* USER CODE END EF */

void LoRaWAN_Init(void)
{
  /* USER CODE BEGIN LoRaWAN_Init_1 */
  LoraInfo_Init();
  /* USER CODE END LoRaWAN_Init_1 */

  /* Init the Lora Stack*/
  LmHandlerInit(&LmHandlerCallbacks);

  /* USER CODE BEGIN LoRaWAN_Init_Last */

  LmHandlerConfigure(&LmHandlerParams);
  LmSetChannel(ChannelsMask);
  LmHandlerSetDevEUI(devEUI);
  LmHandlerJoin(ActivationType);
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
LmHandlerErrorStatus_t LmSetChannel(uint16_t *ChannelsMask)
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
        mibReq.Param.ChannelsMask = ChannelsMask;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}
/* USER CODE END PrFD */

static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN OnRxData_1 */
    printf("RxDoneParams.Size:%d\r\n",appData->BufferSize);
    printf("RxDoneParams.Payload:");
    for(uint8_t i = 0; i < appData->BufferSize; i++) {
    	printf("%02X ",appData->Buffer[i]);
    }
    printf("\r\n");
  /* USER CODE END OnRxData_1 */
}

/* USER CODE BEGIN PrFD_LedEvents */

/* USER CODE END PrFD_LedEvents */

static void OnTxData(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN OnTxData_1 */
  /* USER CODE END OnTxData_1 */
}

static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
  /* USER CODE BEGIN OnJoinRequest_1 */
  /* USER CODE END OnJoinRequest_1 */
}

static void OnMacProcessNotify(void)
{
  /* USER CODE BEGIN OnMacProcessNotify_1 */

  /* USER CODE END OnMacProcessNotify_1 */
}

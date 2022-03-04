/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_lorawan.h
  * @author  MCD Application Team
  * @brief   Header of application of the LRWAN Middleware
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_LORAWAN_H__
#define __APP_LORAWAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LmHandler.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//typedef enum {
//	join                    = 0,
//	send                   	= 1,
//
//	StatusReserved          = 255
//} processStatus_t;
//typedef union processParam
//{
//	ActivationType_t ActivationType,
//	LmHandlerAppData_t SendData
//
//}processParam_t;
//typedef struct SendPacket_s{
//	bool allowDelayedTx;
//	LmHandlerMsgTypes_t MsgType;
//	LmHandlerAppData_t *SendData;
//}SendPacket_t;

typedef enum {
	CMD_JOIN_ABP 	= 1,
	CMD_JOIN_OTAA 	= 2,
	CMD_SEND_UNCONF = 3,
	CMD_SEND_CONF 	= 4,
	CMD_READ_CONFIG = 5,
	CMD_DEV_ADDR 	= 6,
	CMD_DEV_EUI 	= 7,
	CMD_APP_EUI 	= 8,
	CMD_APP_KEY		= 9,
	CMD_APP_SKEY	= 10,
	CMD_NWK_SKEY 	= 11,
	CMD_DEFAULT_CHANNEL	= 12,
	CMD_CHANNEL 	= 13,
	CMD_ADR			= 14,
	CMD_STOP		= 16,
	CMD_DEV_TIME	= 20,
	CMD_GET_TIME	= 21,
	CMD_GET_MCUTIME	= 22,
	CMD_READ_JSON	= 64
}cmdType_t;

typedef struct processRequest
{
	enum
    {
		MSG_NONE		= 0,
		MSG_JOIN        = 1,
		MSG_SEND        = 2,


		MSG_READ_CONFIG = 5,
		MSG_DEV_ADDR 	= 6,
		MSG_DEV_EUI 	= 7,
		MSG_APP_EUI 	= 8,
		MSG_APP_KEY		= 9,
		MSG_APP_SKEY	= 10,
		MSG_NWK_SKEY 	= 11,
		MSG_DEFAULT_CHANNEL	= 12,
		MSG_CHANNEL 	= 13,
		MSG_ADR			= 14,
		MSG_STOP		= 16,
		MSG_DEV_TIME	= 20,
		MSG_RESERVED          		= 255
    }RequestType;

    union
    {
    	ActivationType_t ActivationType;
    	struct
		{
    		bool allowDelayedTx;
    		LmHandlerMsgTypes_t MsgType;
            uint8_t Port;
            uint8_t BufferSize;
            uint8_t Buffer[255];
    	} SendPacket;
    	uint32_t devAddr;
    	uint8_t devEUI[8];
    	uint8_t appEUI[8];
    	uint8_t appKey[16];
    	uint8_t appSKey[16];
    	uint8_t nwkSKey[16];
    	uint16_t defChannel[6];
    	uint16_t channel[6];
    	uint8_t adrEnable;
    } param;
} processRequest_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported Functions Prototypes------------------------------------------------------- */
/**
  * @brief  Init Lora Application
  */
void MX_LoRaWAN_Init(void);

/* USER CODE BEGIN EFP */
void MX_LoRaWAN_Process(void);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__APP_LORAWAN_H__*/

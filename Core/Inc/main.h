/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_system.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "common.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

extern 	void Cyclic_Init(void);



#define YP_SERIAL_BUFF_MAX  (20)

typedef struct
{
    int len;
    U1  buff[YP_SERIAL_BUFF_MAX];
}YpSerialBuff_t;


#define CR_SERIAL_BUFF_MAX  (127)

typedef struct
{
    int len;
    U1  buff[CR_SERIAL_BUFF_MAX];
}CrSerialBuff_t;

typedef struct
{
    int status;
    int len;
    U1  buff[CR_SERIAL_BUFF_MAX];
}MainControlBuff_t;


#define RX_YPSERIAL  (0)
#define RX_CRSERIAL  (1)
#define RX_CRSERIALRESPONSE  (2)





/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

//extern osMailQId ConsoleTxMailHandle;
//extern osMailQId YpSerialTxMailHandle;
//extern osMailQId YpSerialRxMailHandle;
//extern osMailQId GpsStatusMailHandle;
//extern osMailQId BattStatusMailHandle;




/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SIGNAL_CYCLIC 1
#define CYCLIC_TIMER_MS 5
#define YPRX_QUEUES_TIMEOUT 200
#define CRRX_QUEUES_TIMEOUT 200
#define SIGNAL_YPSERIAL_TXCOMP 1
#define SIGNAL_CR_TXCOMP 1
#define SIGNAL_1 1
#define SIGNAL_2 1
#define LED1_Pin GPIO_PIN_15
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */


#define LED1_Pin 			GPIO_PIN_15
#define LED1_Port 			GPIOA

#define LED2_Pin 			GPIO_PIN_3
#define LED2_Port 			GPIOB

#define LED3_Pin 			GPIO_PIN_4
#define LED3_Port 			GPIOB

#define LED4_Pin 			GPIO_PIN_5
#define LED4_Port 			GPIOB

#define LED_OFF 			GPIO_PIN_SET
#define LED_ON 				GPIO_PIN_RESET



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/








/* USER CODE BEGIN Header */
/******************************************************************************
 *
 *	開発機種			：免許証リーダ制御用アダプタ
 *	Micro-processingUnit：STM32G071GBU
 *-----------------------------------------------------------------------------
 *	機能名称	：RTOS周期タイマ処理　
 *	ファイル名	：cyclicl.c
 *	ファイル説明：RTOSによる周期起動タスク処理
 *				：  5ms周期
 *
 *	作成日		：2021年 X月 X日
 *	作成者		：Hayashi
 *
 *			Copyright(C)	2021 YUPITERU Ltd. All rights reserved.
 *=============================================================================
 *
 *-----------------------------------------------------------------------------
 *【 revision history 】
 *
 *	Version	：
 *	Update	：
 *	summary ：
 *	author	：
 *
 ******************************************************************************/


#include "main.h"
#include "ypserial.h"
#include "cyclic.h"
#include "cmsis_os.h"


extern	void YP_SerialRxCheak(void);
extern	void CR_SerialRxCheak(void);
extern	void CR_SerialResponseCheak(void);
extern	void YP_Init(void);
extern	void CR_Init(void);
extern	void MAIN_Init(void);

extern	osTimerId CyclicTimerHandle;
extern	osThreadId CyclicHandle;

//#define debag




/* USER CODE BEGIN 0 */
 /*****************************************************************************
  *
  *	関数名称：void initProcess()
  *	処理概要：各制御処理の初期化
  *	引数	：なし
  *	戻り値	：なし
  *
  *****************************************************************************/
void Cyclic_Init(void)
{
	YP_Init();

	CR_Init();

	MAIN_Init();

}




//--------------------------------------------------------------------------------
//
// 					5ms周期処理
//
//--------------------------------------------------------------------------------
static void cyclic_routine(void)
{

	/* YPシリアル受信判定制御 */
	YP_SerialRxCheak();

	/* カードリーダシリアル受信判定制御 */
	CR_SerialRxCheak();

	/* カードリーダシリアルレスポンス判定制御 */
	CR_SerialResponseCheak();

}

//--------------------------------------------------------------------------------
//
// 					5ms周期タスク処理
//
//--------------------------------------------------------------------------------
void CyclicTaskEntry(void const * argument)
{
    osTimerStart(CyclicTimerHandle, CYCLIC_TIMER_MS);

    for(;;)
    {
       osEvent ret = osSignalWait(SIGNAL_CYCLIC, osWaitForever);

        if (ret.status == osEventSignal && ret.value.signals == SIGNAL_CYCLIC)
        {
			// 定期起動処理
            cyclic_routine();

// TEST LED CONTROL
#ifdef debag
//	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_15);
  	HAL_GPIO_TogglePin(LED1_Port,LED1_Pin);
		GPIO_PinState pin_state = HAL_GPIO_ReadPin(LED1_Port,LED1_Pin);
    if(pin_state == LED_ON){

//			HAL_GPIO_WritePin(LED1_Port, LED1_Pin, LED_ON);
			HAL_GPIO_WritePin(LED3_Port, LED3_Pin, LED_ON);

			HAL_GPIO_WritePin(LED2_Port, LED2_Pin, LED_OFF);
			HAL_GPIO_WritePin(LED4_Port, LED4_Pin, LED_OFF);
	}else{
//			HAL_GPIO_WritePin(LED1_Port, LED1_Pin, LED_OFF);
			HAL_GPIO_WritePin(LED3_Port, LED3_Pin, LED_OFF);

			HAL_GPIO_WritePin(LED2_Port, LED2_Pin, LED_ON);
			HAL_GPIO_WritePin(LED4_Port, LED4_Pin, LED_ON);
 	}
#endif
// TEST LED CONTROL

		}
	    osDelay(1);

    }
}

//--------------------------------------------------------------------------------
//
// 					OSタイマ5ms周期割り込み処理
//
//--------------------------------------------------------------------------------
void CallbackCyclicTimer(void const * argument)
{
    osSignalSet(CyclicHandle, SIGNAL_CYCLIC);

}



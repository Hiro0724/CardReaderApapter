/******************************************************************************
 *
 *	開発機種			：免許証リーダ制御用アダプタ
 *	Micro-processingUnit：STM32G071GBU
 *-----------------------------------------------------------------------------
 *	機能名称	：UART1 通信処理　
 *	ファイル名	：uartCardReader.c
 *	ファイル説明：免許証（カード）リーダとの通信処理
 *				：38400bps, 8bit, start 1bit, stop 1bit, non parity
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


/********************************************************************/
/*  インクルードファイル                                            */
/********************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include <string.h>
#include "stdlib.h"
#include "cardreader.h"
#include "cmsis_os.h"


/********************************************************************/
/*  外部参照宣言	                                                */
/********************************************************************/
//extern void xxxx(void);
//extern	uint8_t	;
extern	void MX_USART1_UART_Init(void);
extern osMailQId CrSerialTxMailHandle;
extern osMailQId CrSerialRxMailHandle;
extern osMailQId MainControlMailHandle;
extern osThreadId CrSerialTxHandle;

extern void init_sq_up( void );

/********************************************************************/
/*  プロトタイプ宣言	                                            */
/********************************************************************/
//void	(void);
//static void(void);
//static void (void);
//static void (void);
//static void (void);


/********************************************************************/
/*	内部定数定義													*/
/********************************************************************/

/* ｼﾘｱﾙ送信状態定義 */
enum{
	TX_STOP=0,								/* ｼﾘｱﾙ送信停止			*/
	TX_DATA,								/* ﾃﾞｰﾀ部送信中			*/
	TX_CMPWAIT								/* 送信完了待ち			*/
};


/* ｼﾘｱﾙ通信制御状態定義 */
enum{
	NOCONN_STATUS = 0,						/* 通信なし				*/
	CONNECTED_STATUS						/* 通信接続中状態		*/
};

/*------------------------------------------*/
/*	送信制御状態定義						*/
/*------------------------------------------*/
enum{
	TXST_WAIT=0,							/* 送信要求待ち状態		*/
	TXST_SENDING							/* 送信中状態			*/

};

#define		CR_RXBUFMAX	(CRD_RCV_BUF_SIZE - CRS_HAEDER )	/* 受信信ﾊﾞｯﾌｧMax値		*/

#define	LEN_CR_SERIAL	5
#define GET_ADR	 (0x00)	// Fix
#define GET_DSTADR(adr) ((adr) & 0x0F)

#define ADR_ANY             (0x0)
#define ADR_SELF            (0xC)
#define CR_SERIAL_LEN_MAX   (CR_SERIAL_BUFF_MAX - 5)
#define NO_SCR      (0x7A)
#define IDX_STX     (0)
#define IDX_ADR     (1)
#define IDX_CMD     (2)
#define IDX_LEN     (3)
#define IDX_DAT     (4)

#define LABL_DAT_STX            (0x02)
#define LABL_DAT_ETX            (0x03)
#define LABL_DAT_CR             (0x0D)



enum {
	ADD_CR_SERIAL_STX = 0,	
	ADD_CR_SERIAL_ADD,	
	ADD_CR_SERIAL_COMMAND,	
	ADD_CR_SERIAL_LEN,	
	ADD_CR_SERIAL_DATA,	
	ADD_CR_SERIAL_ETX,	
	ADD_CR_SERIAL_SUM,	
	ADD_CR_SERIAL_CR,
};

/* ｼﾘｱﾙ受信状態定義 */
typedef enum
{
	RX_STOP=0,								/* ｼﾘｱﾙ受信停止			*/
	RX_ACTVE,								/* ｼﾘｱﾙ受信待ち中		*/
	RX_RCVHEAD,								/* ﾍｯﾀﾞｰ部受信中		*/
	RX_RCVDATA,								/* ﾃﾞｰﾀ部受信中			*/
	RX_RCVERR,								/* 受信ｴﾗｰ発生			*/
}CrSerialRxStatus_e;

typedef struct
{
    U1 rxtmr;
    U1 byte_buff;
    CrSerialRxStatus_e  stat;
    int cnt;
    int len;
    U1  buff[CR_SERIAL_BUFF_MAX];
}CrSerialRx_t;


typedef enum
{
    CMD_CARDREADERINVENTORY 	= 0x71,
    CMD_CARDREADERCHGCOMMOD  	= 0x70,
    CMD_CARDREADERSETMODE 		= 0x4E,
    CMD_CARDREADERREQMODE		= 0x4F,
    CMD_CARDREADERMULTIPOLLING 	= 0x81,
}CRSerialCmd_e;



/********************************************************************/
/*	マクロ定義														*/
/********************************************************************/



/********************************************************************/
/*  内部変数定義                                                    */
/********************************************************************/
static CRS_SERIAL cr_serial;				/* ｼﾘｱﾙ通信周期処理制御情報		*/

static CrSerialRx_t cr_rx;					/* ｼﾘｱﾙ通信時の受信制御情報		*/

static CrSerialBuff_t	cr_tx;				/* ｼﾘｱﾙ通信時の送信制御情報		*/

static  uint8_t buffer;


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;



/********************************************************************/
/*  プロトタイプ定義                                                */
/********************************************************************/

void	cr_SerialInit( void );

U1	CR_SerialOpen(	U1 );
U1	cr_SerialStop( void );

static Bool parse_cr_serial( const U1 *p_buff, int len );
static void cr_ResetRxError( void );


/*****************************************************************************
 *
 *	関数名称：void CR_Init( void )
 *	処理概要：CRｼﾘｱﾙ初期化
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
void CR_Init( void )
{

	/* ｼﾘｱﾙ通信周期処理制御情報ｸﾘｱ */
	c_memset( (U1 *)&cr_serial, 0x00, sizeof( CRS_SERIAL ) );

	/* ｼﾘｱﾙ通信時の受信制御情報ｸﾘｱ */
	c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );

	/* ｼﾘｱﾙ通信時の送信制御情報ｸﾘｱ */
	c_memset( (U1 *)&cr_tx, 0x00, sizeof( CrSerialBuff_t ) );


	CR_SerialOpen( YPS_MODE_NORMAL );

}



/*****************************************************************************
 *
 *	関数名称：U1 CR_SerialOpen( U1 )
 *	処理概要：CRｼﾘｱﾙをｵｰﾌﾟﾝする
 *	引数	：なし
 *	戻り値	：OK--.正常
 *			  NG--.異常
 *
 *****************************************************************************/
U1 CR_SerialOpen(U1	mode)
{
	U1	ret = OK;		/* 関数の戻り値格納用 */


	/* 既に起動済みか？ */
	if ( cr_serial.stat != NOCONN_STATUS ) {
		return( NG );
	}

	/* ｼﾘｱﾙ通信ﾓｰﾄﾞ指定 */
	switch ( mode ) {

	case YPS_MODE_NORMAL:				/* 周期送信ﾓｰﾄﾞ		*/
		cr_serial.stat = CONNECTED_STATUS;
		/* CR通信初期化実施 */
		cr_SerialInit();

	default:							/* その他			*/
		ret = NG;
		break;
	}

	return( ret );
}


/*****************************************************************************
 *
 *	関数名称：void cr_SerialInit( void )
 *	処理概要：CRｼﾘｱﾙをｵｰﾌﾟﾝする
 *	引数	：なし
 *	戻り値	：
 *
 *****************************************************************************/
void cr_SerialInit( void )
{

//	MX_USART1_UART_Init();

	/* ｼﾘｱﾙ受信動作開始 */
	cr_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart1, &buffer, 1);		// 受信開始UART1(TO カードリーダ）

}


/*****************************************************************************
 *
 *	関数名称：U1 cr_SerialStop( void )
 *	処理概要：CRｼﾘｱﾙをｸﾛｰｽﾞする
 *	引数	：なし
 *	戻り値	：
 *
 *****************************************************************************/
U1 cr_SerialStop( void )
{
	U1	ret = OK;

	cr_serial.stat = NOCONN_STATUS;

	return( ret );
}



/*****************************************************************************
 *
 *	関数名称：void	UART1_RxCpltCallback(void)
 *	処理概要：UART1受信割り込み処理
 *	引数	：なし
 *	戻り値	：
 *
 *****************************************************************************/
void	UART1_RxCpltCallback(void)
{

	uint8_t	rxdat;


	rxdat = buffer;										// 受信データ抽出

	// データ受信状態により処理
	switch ( cr_rx.stat ) {
	case RX_ACTVE:										//受信まち
		// 初回受信？
		if ( cr_rx.cnt == 0 ) {
			if (rxdat == LABL_DAT_STX)					// (0x02) 
			{
				cr_rx.stat = RX_RCVHEAD;				// 受信開始
				cr_rx.buff[cr_rx.cnt] = rxdat;
				cr_rx.cnt++;
				cr_rx.rxtmr = 0;
				cr_rx.len = 0;
			}
		} else {
			cr_rx.cnt = 0;
			cr_rx.len = 0;
		}
		
		break;

	case RX_RCVHEAD:									/* データ長受信待ち		*/
		cr_rx.buff[cr_rx.cnt] = rxdat;
		if (cr_rx.cnt == ADD_CR_SERIAL_LEN){
			cr_rx.len = rxdat;

			if ( cr_rx.len > CR_RXBUFMAX ) {
				/* 受信エラー */
				cr_rx.stat = RX_ACTVE;
				cr_rx.len = 0;
				cr_rx.cnt = 0;
			} else {
				cr_rx.len = cr_rx.len + ( CRS_HAEDER );	// 全バイト数(データ＋その他）
				cr_rx.stat = RX_RCVDATA;
				cr_rx.cnt++;
				cr_rx.rxtmr = 0;
			}
		}else{
			cr_rx.cnt++;
		}
		break;

	case RX_RCVDATA:									/* データ部受信			*/
		cr_rx.buff[cr_rx.cnt] = rxdat;
		cr_rx.cnt++;
		cr_rx.rxtmr = 0;
		/* 受信データ長 */
		if ( cr_rx.cnt >= cr_rx.len ) {

			/* 受信確定データバッファ取り出し */

            CrSerialBuff_t *p_mail = osMailAlloc(CrSerialRxMailHandle, osWaitForever);
            if( p_mail )
            {
                p_mail->len = cr_rx.len;
                memcpy(p_mail->buff, cr_rx.buff, p_mail->len);
                osMailPut(CrSerialRxMailHandle, p_mail);
            }

			//受信バッファクリア
			c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );
			cr_rx.stat = RX_ACTVE;
			cr_rx.len = 0;
			cr_rx.cnt = 0;

		}
		break;

	case RX_RCVERR:
	default:
		cr_rx.stat = RX_RCVERR;
		break;
	}

	HAL_UART_Receive_IT(&huart1, &buffer, 1);			//	受信＆割り込み許可

}



/*------------------------------------------------------------------*/
/*																	*/
/*		UART1受信エラー処理											*/
/*																	*/
/*------------------------------------------------------------------*/
void	UART1_ErrorCallback(void)
{

	cr_ResetRxError( );

}


/*****************************************************************************
 *
 *	関数名称：U1 cr_CalcCheckSum( U1 len ,U1 *buf)
 *	処理概要：ﾁｪｯｸｻﾑｺｰﾄﾞを算出する
 *	引数	：データ長　データ先頭アドレス
 *	戻り値	：
 *
 *****************************************************************************/
static U1 cr_serial_checksum( const U1 *p_frame )
{
    U1 sum = 0;
    U1 len = p_frame[ADD_CR_SERIAL_LEN] + 5;

    for( int i=0 ; i<len ; i++ )
    {
        sum += p_frame[i];
    }

    return sum;
}

//--------------------------------------------------------------------------------
static int idx_sum( const U1 *p_frame )
{
    return 5 + p_frame[IDX_LEN];
}

//--------------------------------------------------------------------------------
static int idx_etx( const U1 *p_frame )
{
    return 4 + p_frame[IDX_LEN];
}

//--------------------------------------------------------------------------------
static int idx_cr( const U1 *p_frame )
{
    return 6 + p_frame[IDX_LEN];
}

//--------------------------------------------------------------------------------
static int idx_cal( const U1 *p_frame , U1 adj)
{
    return adj + p_frame[IDX_LEN];
}





/*****************************************************************************
 *
 *	関数名称：void cr_ResetRxError( void )
 *	処理概要：受信エラーが発生時に、受信機能をリセットする
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
static void cr_ResetRxError( void )
{

	// 受信バッファクリア
	c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );

	cr_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart1, &buffer, 1);		// 受信開始UART2(TO 外部接続機器（YPフォーマット）

	return;
}

/*****************************************************************************
 *
 *	関数名称：void cr_ClearRxError( void )
 *	処理概要：受信データ間隔でタイムアウトが発生した時の受信クリア
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
static void cr_ClearRxError( void )
{

	/* 受信動作停止 */
    __disable_irq();

	// 受信バッファクリア
	c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );

	cr_rx.stat = RX_ACTVE;

	/* 受信動作開始 */
    __enable_irq();

	HAL_UART_Receive_IT(&huart1, &buffer, 1);		// 受信開始UART1(TO カードリーダー）

}

/********************************************************************/
/*																	*/
/*					 シリアル状態制御関数 							*/
/*																	*/
/********************************************************************/

/*****************************************************************************
 *
 *	関数名称：void CR_SerialRxCheak( void )
 *	処理概要：CRｼﾘｱﾙRX判定処理　（5ms周期処理）
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/

void CR_SerialRxCheak( void )
{

	/* 受信ｲﾍﾞﾝﾄがあるか？ */
	switch ( cr_rx.stat ) {
	case RX_STOP:			/* 受信動作停止			*/
	case RX_ACTVE:			/* ﾃﾞｰﾀ受信待ち			*/
		break;

	case RX_RCVERR:			/* 受信ｴﾗｰ検知			*/

		/* 受信ﾘｾｯﾄ */
		cr_ClearRxError();
		break;

	default:					/* その他				*/
		cr_rx.rxtmr++;
		if ( cr_rx.rxtmr > YPS_RXWTHTM ) {

			/* 受信ﾘｾｯﾄ */
			cr_ClearRxError();

		}
		break;
	}
}


/*****************************************************************************
 *
 *	処理概要：CRｼﾘｱﾙ受信タスク処理
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
/* USER CODE Header_CrSerialRxTaskEntry */

void CrSerialRxTaskEntry(void const * argument)
{

  /* USER CODE BEGIN YpSerialRxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
		osEvent event = osMailGet(CrSerialRxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			CrSerialBuff_t *p_Rxmail = (CrSerialBuff_t *)event.value.p;
			if (p_Rxmail)
			{
				if( p_Rxmail->len )
				{

					//受信データ解析
                    parse_cr_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len );

               	}
               	osMailFree(CrSerialRxMailHandle, p_Rxmail);
           	}
		}
	    osDelay(1);
	}
}

/*****************************************************************************
 *
 *	処理概要：CRｼﾘｱﾙ送信タスク処理
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
/* USER CODE Header_CRSerialTxTaskEntry */

void CrSerialTxTaskEntry(void const * argument)
{
  for(;;)
  {
		osEvent event = osMailGet(CrSerialTxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			CrSerialBuff_t *p_mail = (CrSerialBuff_t *)event.value.p;
			if (p_mail)
			{
				if( p_mail->len )
				{
                	cr_tx.len = p_mail->len;
                	memcpy(cr_tx.buff, p_mail->buff, cr_tx.len);

               		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)cr_tx.buff, cr_tx.len);
               	}
               	osMailFree(CrSerialTxMailHandle, p_mail);
           	}
		}
		//waiting for tx complete signal (or timeout)
		osSignalWait(SIGNAL_CR_TXCOMP, cr_tx.len*100);

		osDelay(1);
  }
}

/*****************************************************************************
 *
 *	処理概要：CRｼﾘｱﾙ送信完了
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
void UART1_TxCpltCallback( void )
{
    osSignalSet(CrSerialTxHandle, SIGNAL_CR_TXCOMP);
}



/*****************************************************************************
 *
 *	処理概要：CRｼﾘｱﾙ送信処理
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
//--------------------------------------------------------------------------------
static void tx_cr_serial( U1 dst_adr, CRSerialCmd_e cmd, const U1 *p_data, int data_len )
{
    CrSerialBuff_t *p_yps = osMailAlloc( CrSerialTxMailHandle, osWaitForever );
    
    if( p_yps )
    {
        memset( p_yps, 0, sizeof(*p_yps) );

        p_yps->buff[IDX_STX] = LABL_DAT_STX;					
        p_yps->buff[IDX_ADR] = dst_adr;
        p_yps->buff[IDX_LEN] = (U1)data_len;
        p_yps->buff[IDX_CMD] = (U1)cmd;
        if( p_data )
        {
            memcpy(&p_yps->buff[IDX_DAT], p_data, data_len );
        }

		/* 送信するﾁｪｯｸｻﾑを計算して送信ﾃﾞｰﾀとしてｾｯﾄする*/
		/* ﾁｪｯｸｻﾑ算出 */
 
      	p_yps->buff[4 + (p_yps->buff[IDX_LEN])] = LABL_DAT_ETX;	// (0x03)
 	    p_yps->buff[5 + (p_yps->buff[IDX_LEN])] = cr_serial_checksum( p_yps->buff );
      	p_yps->buff[6 + (p_yps->buff[IDX_LEN])] = LABL_DAT_CR;	// (0x0D)

        p_yps->len = 7 + p_yps->buff[IDX_LEN];					//送信バイト数セット

        osMailPut(CrSerialTxMailHandle, p_yps);

    }
}


//--------------------------------------------------------------------------------
//
//		受信データ解析
//
//--------------------------------------------------------------------------------
static Bool parse_cr_serial( const U1 *p_buff, int len )
{

	// CHECKSUM判定
    if( cr_serial_checksum(p_buff) != p_buff[idx_sum(p_buff)] )
    {
        return false;
    }

	// アドレス０判定
    if( p_buff[IDX_ADR] != 0x00 )
    {
        return false;
    }

	// メイン制御へデータ送信   
   	MainControlBuff_t *p_mail = osMailAlloc(MainControlMailHandle, osWaitForever);
   	if( p_mail )
   	{
       	p_mail->status = RX_CRSERIAL;
       	p_mail->len = len;
       	memcpy(p_mail->buff, p_buff, len);
       	osMailPut(MainControlMailHandle, p_mail);
   	}

    return true;
}


//---------------------------------------
//
//			送信要求
//
//---------------------------------------
void	req_tx_cr_serial( U1 src_adr, U1 command, const U1 *p_data, U1 len )
{

	tx_cr_serial( GET_ADR, command, &p_data[0], len );

}



/* USER CODE END 0 */
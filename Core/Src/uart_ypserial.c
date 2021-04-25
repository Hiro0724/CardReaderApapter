/******************************************************************************
 *
 *	開発機種			：免許証リーダ制御用アダプタ
 *	Micro-processingUnit：STM32G071GBU
 *-----------------------------------------------------------------------------
 *	機能名称	：UART2 通信処理　
 *	ファイル名	：uart_ypserial.c
 *	ファイル説明：外部接続機器とのYYPフォーマット通信処理
 *				：オプションアダプタ経由予定
 *				：  9600bps, 8bit, start 1bit, stop 2bit, non parity
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
extern	void MX_USART2_UART_Init(void);
extern osMailQId YpSerialTxMailHandle;
extern osMailQId YpSerialRxMailHandle;
extern osMailQId MainControlMailHandle;
extern osThreadId YpSerialTxHandle;

extern void req_status( U1 *p_status, U1 *p_kind, U1 *p_err );

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

#define		YP_RXBUFMAX	(YPS_RCV_BUF_SIZE - YPS_HAEDER )	/* 受信信ﾊﾞｯﾌｧMax値		*/

#define	LEN_YP_SERIAL	5
#define GET_SRCADR(adr) (((adr) & 0xF0)>>4)
#define GET_DSTADR(adr) ((adr) & 0x0F)

#define ADR_ANY             (0x0)
#define ADR_SELF            (0xC)
#define YP_SERIAL_LEN_MAX   (YP_SERIAL_BUFF_MAX - 5)
#define NO_SCR      (0x7A)
#define IDX_ADR     (0)
#define IDX_LEN     (1)
#define IDX_SCR     (2)
#define IDX_CMD     (3)
#define IDX_DAT     (4)

enum {
	ADD_YP_SERIAL_ADD = 0,	
	ADD_YP_SERIAL_LEN,	
	ADD_YP_SERIAL_SCRAMBLE,	
	ADD_YP_SERIAL_COMMAND,	
	ADD_YP_SERIAL_DATA,	
	ADD_YP_SERIAL_CHKSUM,
};

/* ｼﾘｱﾙ受信状態定義 */
typedef enum
{
	RX_STOP=0,								/* ｼﾘｱﾙ受信停止			*/
	RX_ACTVE,								/* ｼﾘｱﾙ受信待ち中		*/
	RX_RCVHEAD,								/* ﾍｯﾀﾞｰ部受信中		*/
	RX_RCVDATA,								/* ﾃﾞｰﾀ部受信中			*/
	RX_RCVERR,								/* 受信ｴﾗｰ発生			*/
}YpSerialRxStatus_e;

typedef struct
{
    U1 rxtmr;
    U1 byte_buff;
    YpSerialRxStatus_e  stat;
    int cnt;
    int len;
    U1  buff[YP_SERIAL_BUFF_MAX];
}YpSerialRx_t;


typedef enum
{
    ReqCmd_YpSerialStatus = 0xA0,
    ReqCmd_YpSerialReset  = 0xA1,
    ReqCmd_YpSerialReqKind = 0xA2,
    ReqCmd_YpSerialReserve1 = 0xA3,
    ReqCmd_YpSerialReserve2 = 0xA4,
}YpSerialReqCmd_e;


typedef enum
{
    Cmd_YpSerialStatus = 0xA0,
    Cmd_YpSerialReset  = 0xA1,
    Cmd_YpSerialReqKind = 0xA2,
    Cmd_YpSerialReserve1 = 0xA3,
    Cmd_YpSerialReserve2 = 0xA4,
}YpSerialCmd_e;








/********************************************************************/
/*	マクロ定義														*/
/********************************************************************/



/********************************************************************/
/*  内部変数定義                                                    */
/********************************************************************/
static YPS_SERIAL yp_serial;				/* ｼﾘｱﾙ通信周期処理制御情報		*/

static YpSerialRx_t yp_rx;					/* ｼﾘｱﾙ通信時の受信制御情報		*/

static YpSerialBuff_t	yp_tx;				/* ｼﾘｱﾙ通信時の送信制御情報		*/

static  uint8_t buffer;


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
IWDG_HandleTypeDef hiwdg;



/********************************************************************/
/*  プロトタイプ定義                                                */
/********************************************************************/

void	yp_SerialInit( void );

U1	YP_SerialOpen(	U1 );
U1	yp_SerialStop( void );

static Bool parse_yp_serial( const U1 *p_buff, int len );
static void yp_ResetRxError( void );

extern void	 req_chgkind( U1 *p);

/*****************************************************************************
 *
 *	関数名称：void YP_SerialInit( void )
 *	処理概要：YPｼﾘｱﾙ初期化
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
void YP_Init( void )
{

	/* ｼﾘｱﾙ通信周期処理制御情報ｸﾘｱ */
	c_memset( (U1 *)&yp_serial, 0x00, sizeof( YPS_SERIAL ) );

	/* ｼﾘｱﾙ通信時の受信制御情報ｸﾘｱ */
	c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );

	/* ｼﾘｱﾙ通信時の送信制御情報ｸﾘｱ */
	c_memset( (U1 *)&yp_tx, 0x00, sizeof( YpSerialBuff_t ) );


	YP_SerialOpen( YPS_MODE_NORMAL );

}



/*****************************************************************************
 *
 *	関数名称：U1 YP_SerialOpen( U1 )
 *	処理概要：YPｼﾘｱﾙをｵｰﾌﾟﾝする
 *	引数	：なし
 *	戻り値	：OK--.正常
 *			  NG--.異常
 *
 *****************************************************************************/
U1 YP_SerialOpen(U1	mode)
{
	U1	ret = OK;		/* 関数の戻り値格納用 */


	/* 既に起動済みか？ */
	if ( yp_serial.stat != NOCONN_STATUS ) {
		return( NG );
	}

	/* ｼﾘｱﾙ通信ﾓｰﾄﾞ指定 */
	switch ( mode ) {

	case YPS_MODE_NORMAL:				/* 周期送信ﾓｰﾄﾞ		*/
		yp_serial.stat = CONNECTED_STATUS;
		/* YP通信初期化実施 */
		yp_SerialInit();

	default:							/* その他			*/
		ret = NG;
		break;
	}

	return( ret );
}


/*****************************************************************************
 *
 *	関数名称：void yp_SerialInit( void )
 *	処理概要：YPｼﾘｱﾙをｵｰﾌﾟﾝする
 *	引数	：なし
 *	戻り値	：
 *
 *****************************************************************************/
void yp_SerialInit( void )
{

//	MX_USART2_UART_Init();

	/* ｼﾘｱﾙ受信動作開始 */
	yp_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart2, &buffer, 1);		// 受信開始UART2(TO 外部接続機器（YPフォーマット）

}


/*****************************************************************************
 *
 *	関数名称：U1 yp_SerialStop( void )
 *	処理概要：YPｼﾘｱﾙをｸﾛｰｽﾞする
 *	引数	：なし
 *	戻り値	：
 *
 *****************************************************************************/
U1 yp_SerialStop( void )
{
	U1	ret = OK;

	yp_serial.stat = NOCONN_STATUS;

	return( ret );
}



/*****************************************************************************
 *
 *	関数名称：void	UART2_RxCpltCallback(void)
 *	処理概要：UART2受信割り込み処理
 *	引数	：なし
 *	戻り値	：
 *
 *****************************************************************************/
void	UART2_RxCpltCallback(void)
{

	uint8_t	rxdat;
	uint8_t	data;

	rxdat = buffer;										// 受信データ抽出

	// データ受信状態により処理
	switch ( yp_rx.stat ) {
	case RX_ACTVE:										//受信まち
		// 初回受信？
		if ( yp_rx.cnt == 0 ) {
			data = rxdat & 0xF0;
			data = data >> 4;
			if ( ( data != 0 ) && ( data <= 7 ) ) {
				yp_rx.stat = RX_RCVHEAD;				//送信元判定
				yp_rx.buff[yp_rx.cnt] = rxdat;
				yp_rx.cnt++;
				yp_rx.rxtmr = 0;
			}
		} else {
			yp_rx.cnt = 0;
		}
		
		break;

	case RX_RCVHEAD:									/* ヘッダ受信		*/
		yp_rx.buff[yp_rx.cnt] = rxdat;
		yp_rx.len = rxdat;
		if ( yp_rx.len > YP_RXBUFMAX ) {
			/* 受信エラー */
			yp_rx.stat = RX_ACTVE;
			yp_rx.len = 0;
			yp_rx.cnt = 0;
		} else {
			yp_rx.len = yp_rx.len + ( YPS_HAEDER );
			yp_rx.stat = RX_RCVDATA;
			yp_rx.cnt++;
			yp_rx.rxtmr = 0;
		}
		break;

	case RX_RCVDATA:									/* データ部受信			*/
		yp_rx.buff[yp_rx.cnt] = rxdat;
		yp_rx.cnt++;
		yp_rx.rxtmr = 0;
		/* 受信データ長 */
		if ( yp_rx.cnt >= yp_rx.len ) {

			/* 受信確定データバッファ取り出し */

            YpSerialBuff_t *p_mail = osMailAlloc(YpSerialRxMailHandle, osWaitForever);
            if( p_mail )
            {
                p_mail->len = yp_rx.len;
                memcpy(p_mail->buff, yp_rx.buff, p_mail->len);
                osMailPut(YpSerialRxMailHandle, p_mail);
            }

			//受信バッファクリア
			c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );
			yp_rx.stat = RX_ACTVE;
			yp_rx.len = 0;
			yp_rx.cnt = 0;

		}
		break;

	case RX_RCVERR:
	default:
		yp_rx.stat = RX_RCVERR;
		break;
	}

	HAL_UART_Receive_IT(&huart2, &buffer, 1);			//	受信＆割り込み許可

}



/*------------------------------------------------------------------*/
/*																	*/
/*		UART2受信エラー処理											*/
/*																	*/
/*------------------------------------------------------------------*/
void	UART2_ErrorCallback(void)
{

	yp_ResetRxError( );

}


/*****************************************************************************
 *
 *	関数名称：U1 yp_CalcCheckSum( U1 len ,U1 *buf)
 *	処理概要：ﾁｪｯｸｻﾑｺｰﾄﾞを算出する
 *	引数	：データ長　データ先頭アドレス
 *	戻り値	：
 *
 *****************************************************************************/
static U1 yp_CalcCheckSum(	
	U1		len,			/* ﾃﾞｰﾀ長格納用			*/
	U1		*buf			/* ﾃﾞｰﾀ列の先頭ｱﾄﾞﾚｽ	*/
)
{
	U1		sum = 0;
	U1		cnt;


	/* ﾁｪｯｸｻﾑ算出 */
	for ( cnt = 0 ; cnt < len ; cnt++ ) {
		sum = sum + buf[cnt];
	}

	return( sum );
}

//--------------------------------------------------------------------------------
static U1 yp_serial_checksum( const U1 *p_frame )
{
    U1 sum = 0;
    U1 len = p_frame[1] + 4;

    for( int i=0 ; i<len ; i++ )
    {
        sum += p_frame[i];
    }

    return sum;
}

//--------------------------------------------------------------------------------
static int idx_sum( const U1 *p_frame )
{
    return 4 + p_frame[IDX_LEN];
}



/*****************************************************************************
 *
 *	関数名称：void yp_ResetRxError( void )
 *	処理概要：受信エラーが発生時に、受信機能をリセットする
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
static void yp_ResetRxError( void )
{

	// 受信バッファクリア
	c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );

	yp_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart2, &buffer, 1);		// 受信開始UART2(TO 外部接続機器（YPフォーマット）

	return;
}

/*****************************************************************************
 *
 *	関数名称：void yp_ClearRxError( void )
 *	処理概要：受信データ間隔でタイムアウトが発生した時の受信クリア
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
static void yp_ClearRxError( void )
{

	/* 受信動作停止 */
    __disable_irq();

	// 受信バッファクリア
	c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );

	yp_rx.stat = RX_ACTVE;

	/* 受信動作開始 */
    __enable_irq();

	HAL_UART_Receive_IT(&huart2, &buffer, 1);		// 受信開始UART2(TO 外部接続機器（YPフォーマット）

}

/********************************************************************/
/*																	*/
/*					 シリアル状態制御関数 							*/
/*																	*/
/********************************************************************/

/*****************************************************************************
 *
 *	関数名称：void YP_SerialRxCheak( void )
 *	処理概要：YPｼﾘｱﾙRX判定処理　（5ms周期処理）
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/

void YP_SerialRxCheak( void )
{

	/* 受信ｲﾍﾞﾝﾄがあるか？ */
	switch ( yp_rx.stat ) {
	case RX_STOP:			/* 受信動作停止			*/
	case RX_ACTVE:			/* ﾃﾞｰﾀ受信待ち			*/
		break;

	case RX_RCVERR:			/* 受信ｴﾗｰ検知			*/

		/* 受信ﾘｾｯﾄ */
		yp_ClearRxError();
		break;

	default:					/* その他				*/
		yp_rx.rxtmr++;
		if ( yp_rx.rxtmr > YPS_RXWTHTM ) {

			/* 受信ﾘｾｯﾄ */
			yp_ClearRxError();

		}
		break;
	}
}


/*****************************************************************************
 *
 *	処理概要：YPｼﾘｱﾙ受信タスク処理
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
/* USER CODE Header_YpSerialRxTaskEntry */

void YpSerialRxTaskEntry(void const * argument)
{

  /* USER CODE BEGIN YpSerialRxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
		osEvent event = osMailGet(YpSerialRxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			YpSerialBuff_t *p_Rxmail = (YpSerialBuff_t *)event.value.p;
			if (p_Rxmail)
			{
				if( p_Rxmail->len )
				{

					//受信データ解析
                    parse_yp_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len );

               	}
               	osMailFree(YpSerialRxMailHandle, p_Rxmail);
           	}
		}
	    osDelay(1);
	}
}

/*****************************************************************************
 *
 *	処理概要：YPｼﾘｱﾙ送信タスク処理
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
/* USER CODE Header_YpSerialTxTaskEntry */

void YpSerialTxTaskEntry(void const * argument)
{
  for(;;)
  {
		osEvent event = osMailGet(YpSerialTxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			YpSerialBuff_t *p_mail = (YpSerialBuff_t *)event.value.p;
			if (p_mail)
			{
				if( p_mail->len )
				{
                	yp_tx.len = p_mail->len;
                	memcpy(yp_tx.buff, p_mail->buff, yp_tx.len);

               		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)yp_tx.buff, yp_tx.len);
               	}
               	osMailFree(YpSerialTxMailHandle, p_mail);
           	}
		}
		//waiting for tx complete signal (or timeout)
		osSignalWait(SIGNAL_YPSERIAL_TXCOMP, yp_tx.len*100);

		osDelay(1);
  }
}

/*****************************************************************************
 *
 *	処理概要：YPｼﾘｱﾙ送信完了
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
void UART2_TxCpltCallback( void )
{
    osSignalSet(YpSerialTxHandle, SIGNAL_YPSERIAL_TXCOMP);
}



/*****************************************************************************
 *
 *	処理概要：YPｼﾘｱﾙ送信処理
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
//--------------------------------------------------------------------------------
static void tx_yp_serial( U1 dst_adr, YpSerialCmd_e cmd, const U1 *p_data, int data_len )
{
    YpSerialBuff_t *p_yps = osMailAlloc( YpSerialTxMailHandle, osWaitForever );
    
    if( p_yps )
    {
        memset( p_yps, 0, sizeof(*p_yps) );

        p_yps->buff[IDX_ADR] = (U1)((ADR_SELF << 4) | (dst_adr & 0x0f));
        p_yps->buff[IDX_LEN] = (U1)data_len;
        p_yps->buff[IDX_SCR] = NO_SCR;
        p_yps->buff[IDX_CMD] = (U1)cmd;
        if( p_data )
        {
            memcpy(&p_yps->buff[IDX_DAT], p_data, data_len );
        }

		/* 送信するﾁｪｯｸｻﾑを計算して送信ﾃﾞｰﾀとしてｾｯﾄする*/
		/* ﾁｪｯｸｻﾑ算出 */
        p_yps->buff[idx_sum(p_yps->buff)] = yp_serial_checksum( p_yps->buff );

        p_yps->len = 5 + p_yps->buff[IDX_LEN];

        osMailPut(YpSerialTxMailHandle, p_yps);
    }
}


//--------------------------------------------------------------------------------
//
//		受信データ解析
//
//--------------------------------------------------------------------------------
static Bool parse_yp_serial( const U1 *p_buff, int len )
{

	// CHECKSUM判定
    if( yp_serial_checksum(p_buff) != p_buff[idx_sum(p_buff)] )
    {
        return false;
    }

	// スクランブル７A判定
    if( p_buff[IDX_SCR] != 0x7A )
    {
        return false;
    }

	// アドレス設定
    U1 src_adr = GET_SRCADR(p_buff[IDX_ADR]);

    switch( p_buff[IDX_CMD] )
    {

	case	ReqCmd_YpSerialStatus:	// 0xA0
			{
				//ステータス応答
				U1 ret_code[3];
			  	U1 ret_status;
			  	U1 ret_cardkind;
			  	U1 ret_err;
				
				// ステータス要求
				req_status(&ret_status, &ret_cardkind,	&ret_err );

				//ステータスセット 
				ret_code[0] = ret_status;
				//カード種別セット 
				ret_code[1] = ret_cardkind;
				//エラーコードセット 
				ret_code[2] = ret_err;

				tx_yp_serial( src_adr, Cmd_YpSerialStatus, &ret_code[0], sizeof(ret_code) );
							//アドレス    　　コマンド　　　　データ　　　　　　データ長

			}
	        break;

	case	ReqCmd_YpSerialReset:	// 0xA1
			{
				// リセット応答
		  		U1 ret_code[1] = {0x00};// 0x00 fix
				tx_yp_serial( src_adr, Cmd_YpSerialReset, &ret_code[0], sizeof(ret_code) );





			}        
		
			break;

	case	ReqCmd_YpSerialReqKind:	// 0xA2
			{
				// カード種別要求
		  		U1 ret_code[1];
				U1 p;

				// 変更データ設定
				p = p_buff[IDX_DAT];
				req_chgkind(&p);

		  		
				// カード読取種別応答
		  		ret_code[0] = p_buff[IDX_DAT];
		  		
				tx_yp_serial( src_adr, Cmd_YpSerialReqKind, &ret_code[0], sizeof(ret_code) );


			}        
        	break;

// TEST

	case	ReqCmd_YpSerialReserve1:	// 0xA3
			{
				// メイン制御へデータ送信   
		    	MainControlBuff_t *p_mail = osMailAlloc(MainControlMailHandle, osWaitForever);
            	if( p_mail )
            	{
                	p_mail->status = RX_YPSERIAL;
                	p_mail->len = len;
                	memcpy(p_mail->buff, p_buff, len);
                	osMailPut(MainControlMailHandle, p_mail);
            	}
	        }	
			break;

	case	ReqCmd_YpSerialReserve2: // 0xA4
			//TEST
			{
			  	U1 ret_code[15];
				tx_yp_serial( src_adr, Cmd_YpSerialReserve2, &ret_code[0], sizeof(ret_code) );

		    	MainControlBuff_t *p_mail = osMailAlloc(MainControlMailHandle, osWaitForever);
            	if( p_mail )
            	{
                	p_mail->status = RX_YPSERIAL;
                	p_mail->len = len;
                	memcpy(p_mail->buff, p_buff, len);
                	osMailPut(MainControlMailHandle, p_mail);
            	}


			}        
//TEST
			
        	break;


    default:
	    	return false;

    }
    return true;
}


//---------------------------------------
//
//			送信要求
//
//---------------------------------------
void	req_tx_yp_serial( U1 src_adr, U1 command, const U1 *p_data, U1 len )
{

	tx_yp_serial( src_adr, command, &p_data[0], len );

}



/* USER CODE END 0 */
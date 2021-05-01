
//test
//test add
//test add2
//tets add2
//tesr add3
/******************************************************************************
 *
 *	開発機種			：免許証リーダ制御用アダプタ
 *	Micro-processingUnit：STM32G071GBU
 *-----------------------------------------------------------------------------
 *	機能名称	：MAIN制御処理　
 *	ファイル名	：main_control.c
 *	ファイル説明：メイン制御処理
 *
 *	作成日		：2021年 X月 X日
 *	作成者		：Hayashi
 *
 *			Copyright(C)	2021 YUPITERU Ltd. All rights reserved.
 *=============================================================================
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
#include "intevt.h"



/********************************************************************/
/*  プロトタイプ定義                                                */
/********************************************************************/

extern osMailQId MainControlMailHandle;

extern void	req_tx_cr_serial( U1 src_adr, U1 command, const U1 *p_data, U1 len );
extern void	req_tx_yp_serial( U1 src_adr, U1 command, const U1 *p_data, U1 len );


static Bool parse_yprx_serial( const U1 *p_buff, int len );
static Bool parse_crrx_serial( const U1 *p_buff, int len , int evnt);
static void init_cr_tx_dataset(void);
static void nor_cr_tx_dataset(void);



typedef enum
{
    YPCMD_CARDREADERSTATUS = 0xA0,
    YPCMD_CARDREADERRESET  = 0xA1,
    YPCMD_CARDREADERREQKIND = 0xA2,
    YPCMD_CARDREADERRESERVE1 = 0xA3,
    YPCMD_CARDREADERRESERVE2 = 0xA4,
}YpSerialCmd_e;


typedef enum
{
    MAINSTATUS_INI = 0x00,
    MAINSTATUS_NOR = 0x01,
    MAINSTATUS_ERR = 0x02,
    MAINSTATUS_TST = 0x03,
}MainStatus_e;


#define YP_IDX_ADR     (0)
#define YP_IDX_LEN     (1)
#define YP_IDX_SCR     (2)
#define YP_IDX_CMD     (3)
#define YP_IDX_DAT     (4)

#define CR_IDX_STX     (0)
#define CR_IDX_ADR     (1)
#define CR_IDX_CMD     (2)
#define CR_IDX_LEN     (3)
#define CR_IDX_DAT     (4)

#define CR_KIND_INI		(0x04) //TYPE B


/*==============================================================*/
/* [ 定義宣言 ]                                         		*/
/* 																*/
/*==============================================================*/
struct	jobtb	{
	const 	uint8_t	evnt;				/* ｲﾍﾞﾝﾄ		*/
	const	uint8_t	next;				/* 次状態番号	*/  // ここに入るものは無条件(No Condition)時
	void    (* const tsk)(void);		/* ﾀｽｸｱﾄﾞﾚｽ		*/
};


// イニシャルシーケンス番号
typedef enum
{
    INIT_SEQ_CR_START = 0,              // 初期動作開始受信
 
    INIT_SEQ_CR_TX_SETINVENTORY,        // INVENTORY送信
    INIT_SEQ_CR_RX_SETINVENTORY,        // INVENTORY受信
    INIT_SEQ_CR_TX_SETCOMMOD1,          // 通信モード設定送信
    INIT_SEQ_CR_RX_SETCOMMOD1,          // 通信モード設定受信
    INIT_SEQ_CR_TX_SETMODE,             // 動作モード設定送信
    INIT_SEQ_CR_RX_SETMODE,        		// 動作モード設定受信
    INIT_SEQ_CR_TX_SETCOMMOD2,          // 通信モード設定送信
    INIT_SEQ_CR_RX_SETCOMMOD2,       	// 通信モード設定受信
    INIT_SEQ_CR_TX_REQMODE1,    		// ＲＯＭバージョン取得送信
    INIT_SEQ_CR_RX_REQMODE1,            // ＲＯＭバージョン取得受信
    INIT_SEQ_CR_TX_REQMODE2,    		// モジュールタイプ取得送信
    INIT_SEQ_CR_RX_REQMODE2,            // モジュールタイプ取得受信
    INIT_SEQ_CR_TX_REQMODE3,    		// ＲＯＭバージョン取得送信
    INIT_SEQ_CR_RX_REQMODE3,            // ＲＯＭバージョン取得受信

    INIT_SEQ_CR_COMPLETE,               // 完了

}e_INIT_SEQ;

typedef enum
{
    STATUS_INIT = 0, 		             // 初期動作中
    STATUS_NOR1,  			             // 通常1動作中
    STATUS_NOR2,  			             // 通常2動作中
    STATUS_TST,  			             // テスト動作中
}e_MAIN_STATUS;


typedef enum
{
    ERR_NON = 0, 			             // エラー無し
    ERR_CR, 			 	    	     // カードリーダエラー（応答無し)
}e_MAIN_ERR;


typedef enum
{
    YP_TX_PRA_STATUS = 0, 	             // ステータス応答
    YP_TX_PRA_RESET, 		             // リセット応答
    YP_TX_PRA_KIND, 		             // 読み込みカード指定応答
    YP_TX_PRA_REV0, 		    	     // 
    YP_TX_PRA_REV1, 		    	     // 
    YP_TX_PRA_CARDDATA 		             // リセット応答
}e_YP_TX_PRA;


static const uint8_t    tburtevt[] = {
    QAPEV_INIT_CMDA,                     // CMD A受信
    QAPEV_INIT_CMDB,                     // CMD B受信
    QAPEV_INIT_CMDC,                     // CMD C受信
    QAPEV_INIT_CMDD,                     // CMD D受信
    QAPEV_INIT_CMDE,                     // CMD E受信
    QAPEV_INIT_CMDF,                     // CMD F受信
    QAPEV_INIT_CMDG,                     // CMD G受信
    QAPEV_INIT_CMDH,                     // CMD H受信
};

/*==============================================================*/
/* [ 変数定義]                                         			*/
/* 																*/
/*==============================================================*/
static e_MAIN_STATUS	mainstatus;					/* 状態番号		*/
static e_MAIN_STATUS	next_mainstatus;			/* 次状態番号	*/

static e_MAIN_ERR		err_status;					/* エラー状態	*/

F1 	read_cardkind;									// 読み込みカード種別

static U1 init_seq;
static U1 tx_cmd;


static	int cr_rev_data[30];		


/********************************************************************/
/*																	*/
/*				メイン動作制御関数 									*/
/*																	*/
/********************************************************************/
/*****************************************************************************
 *
 *	関数名称：void MAIN_Init( void )
 *	処理概要：メイン動作初期化
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/
void MAIN_Init( void )
{

	mainstatus = STATUS_INIT; 			             // 初期動作中
	next_mainstatus = STATUS_INIT; 			         // 初期動作中
	err_status = ERR_NON;

	init_seq = INIT_SEQ_CR_TX_SETINVENTORY;

 	read_cardkind.byte = CR_KIND_INI;				// 読み込みカード種別初期値
	

	// カードリーダ初期設定開始
	init_cr_tx_dataset();

}


/*****************************************************************************
 *
 *	処理概要：カードリーダ初期設定データ送信処理
 *	引数	：コマンド
 *	戻り値	：なし
 *
 *****************************************************************************/
static void init_cr_tx_dataset(void)
{
	const ST_CRDATA *ans;
//	U2 *pp;

	U1 p_data[30];
	U1 i,j,len,cmd;
		
	if (init_seq >= INIT_SEQ_CR_COMPLETE)
	{
		return;
	}

	//　データ部抽出
	i = init_seq / 2;

	cmd = CR_TX_INI_DATA_TBL[i].cmd;

	ans = CR_TX_INI_DATA_TBL;

	for(j = 0; j < i; j++)
	{
		++ans;
	}

	len = *ans->str;
//	pp = ans->str;

	for(j = 1; j <= len; j++)
	{
//		p_data[(j-1)] = *(pp+j);
		p_data[(j-1)] = *((ans->str)+j);
	}

	// コマンドセット
	req_tx_cr_serial( 0x00, cmd ,&p_data[0], len );
}


/*****************************************************************************
 *
 *	処理概要：カードリーダデータ送信処理
 *	引数	：コマンド
 *	戻り値	：なし
 *
 *****************************************************************************/
static void nor_cr_tx_dataset(void)
{
	const ST_CRDATA *ans;
//	U2 *pp;

	U1 p_data[30];
	U1 i,j,len,cmd;
		
	//　データ部抽出
	i = init_seq / 2;

	cmd = CR_TX_NOR_DATA_TBL[i].cmd;

	ans = CR_TX_NOR_DATA_TBL;

	for(j = 0; j < i; j++)
	{
		++ans;
	}


	len = *ans->str;
//	pp = ans->str;

	for(j = 1; j <= len; j++)
	{
//		p_data[(j-1)] = *(pp+j);
		p_data[(j-1)] = *((ans->str)+j);
	}

	// コマンドセット
	req_tx_cr_serial( 0x00, cmd ,&p_data[0], len );
}


/*****************************************************************************
 *
 *	処理概要：YPシリアルデータ送信処理
 *	引数	：コマンド
 *	戻り値	：なし
 *
 *****************************************************************************/


static void nor_yp_tx_dataset(U1 pra)
{
	const ST_CRDATA *ans;

	U1 p_data[30];
	U1 i,j,len,cmd;
		
	//　データ部抽出
	i = pra;
	cmd = YP_TX_NOR_DATA_TBL[i].cmd;

	ans = YP_TX_NOR_DATA_TBL;
	for(j = 0; j < i; j++)
	{
		++ans;
	}

	len = *ans->str;

	for(j = 1; j <= len; j++)
	{
		p_data[(j-1)] = *((ans->str)+j);
	}


	// コマンドセット
	//ステータスセット 
	p_data[0] = mainstatus;
	//カード種別セット 
	p_data[1] = read_cardkind.byte;
	//エラーコードセット 
	p_data[2] = err_status;
	
	req_tx_yp_serial( 0x00, cmd ,&p_data[0], len );
}







/*****************************************************************************
 *
 *	処理概要：メイン制御処理
 *	引数	：なし
 *	戻り値	：なし
 *
 *****************************************************************************/

void MainControlTaskEntry(void const * argument)
{
	int evnt,i;


  /* USER CODE BEGIN MainControlTaskEntry */
  /* Infinite loop */
  for(;;)
  {
		osEvent event = osMailGet(MainControlMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
				MainControlBuff_t *p_Rxmail = (MainControlBuff_t *)event.value.p;
				if (p_Rxmail)
				{
					//受信データ解析
					switch(p_Rxmail->status)
					{
						case RX_YPSERIAL:		// from yp serial
				   			parse_yprx_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len );
							break;

						case RX_CRSERIAL:		// from card reader


							// 受信データ抽出
						    for (i=0 ; i < p_Rxmail->len; i++){
								cr_rev_data[i] = p_Rxmail->buff[i];		
							}
    
							// イベントセット
							switch (mainstatus)
							{
								case STATUS_INIT:		             // 初期動作中
									evnt = tburtevt[(init_seq/2)];
									break;
							
								case STATUS_NOR1: 		             // 通常1動作中
									evnt = tburtevt[tx_cmd];
									break;
							
								case STATUS_NOR2:		             // 通常2動作中
									break;

								case STATUS_TST: 		             // テスト動作中							
									break;
							}

				   			parse_crrx_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len , evnt);
							break;

    					default:
							break;
					}
				}
       	osMailFree(MainControlMailHandle, p_Rxmail);
		}
	    osDelay(1);
	}
}



//--------------------------------------------------------------------------------
//
//						受信データ解析
//
//--------------------------------------------------------------------------------
//***************************************************
//		YPシリアルからのデータ
//***************************************************

static Bool parse_yprx_serial( const U1 *p_buff, int len )
{

	switch( p_buff[YP_IDX_CMD] )
	{
		case YPCMD_CARDREADERRESERVE1:

			{
			  	U1 ret_code[15];
				ret_code[0] = 1;
				ret_code[1] = 2;
			}
			break;

		case YPCMD_CARDREADERRESERVE2:
			{
			  	U1 ret_code[2];
				ret_code[0] = 1;
				ret_code[1] = 2;
			}

			break;

    default:
	    	return false;

    }
    return true;
}



//******************************************
//		カードリーダ　イニシャル設定
//		送信後イニシャルシーケンスUP
//
//******************************************
void init_sq_up( void )
{
	if (mainstatus == STATUS_INIT) 			             // 初期動作中
	{
		++init_seq;
	}
}


//******************************************
//		ステータス問合せ要求応答
//
//******************************************
void req_status( U1 *p_status, U1 *p_kind, U1 *p_err )
{
	U1 a;


	a = mainstatus;
	*p_status = a;

	*p_kind = read_cardkind.byte;	 

	a = err_status;
	*p_err = a;

}

//******************************************
//		読取カード種別変更
//
//******************************************
void req_chgkind( U1 *p_kind)
{

	read_cardkind.byte = *p_kind;	 

}

//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      コマンド送信0x71 の　ACK　受信 
//******************************************
void	tsk000(void)
{


	init_seq = INIT_SEQ_CR_TX_SETCOMMOD1;
	init_cr_tx_dataset();
}

//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      コマンド送信0x70 の　ACK　受信 
//******************************************
void	tsk001(void)
{


	init_seq = INIT_SEQ_CR_TX_SETMODE;
	init_cr_tx_dataset();
}

//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      コマンド送信0x4E の　ACK　受信 
//******************************************
void	tsk002(void)
{


	init_seq = INIT_SEQ_CR_TX_SETCOMMOD2;
	init_cr_tx_dataset();
}

//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      コマンド送信0x70 の　ACK　受信 
//******************************************
void	tsk003(void)
{


	init_seq = INIT_SEQ_CR_TX_REQMODE1;
	init_cr_tx_dataset();
}

//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      コマンド送信0x4F の　ACK　受信 
//******************************************
void	tsk004(void)
{


	init_seq = INIT_SEQ_CR_TX_REQMODE2;
	init_cr_tx_dataset();
}

//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      コマンド送信0x4F の　ACK　受信 
//******************************************
void	tsk005(void)
{


	init_seq = INIT_SEQ_CR_TX_REQMODE3;
	init_cr_tx_dataset();
}



//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      コマンド送信0x4F の　ACK　受信 
//******************************************
void	tsk006(void)
{
	mainstatus = STATUS_NOR1; 			 // 通常動作中
	next_mainstatus = STATUS_NOR1; 		 // 通常動作中
	init_seq = INIT_SEQ_CR_COMPLETE;

	//YP SERIALに初期設定完了通知
	nor_yp_tx_dataset(YP_TX_PRA_STATUS);
}


//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      ACK　受信 (設定コマンドに対する応答はACKでのみ判断)
//******************************************
void	tsk007(void)
{

	switch (init_seq)
	{

		case INIT_SEQ_CR_TX_SETINVENTORY:        // INVENTORY受信

			//コマンド解除の応答の為、ACKのみ判定

			init_seq = INIT_SEQ_CR_TX_SETCOMMOD1;
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD1:          // 通信モード設定受信

			//現在ACKのみ判定

			init_seq = INIT_SEQ_CR_TX_SETMODE;
			break;   

		case INIT_SEQ_CR_TX_SETMODE:        	// 動作モード設定受信

			//現在ACKのみ判定

			init_seq = INIT_SEQ_CR_TX_SETCOMMOD2;
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD2:       	// 通信モード設定受信

			//現在ACKのみ判定

			init_seq = INIT_SEQ_CR_TX_REQMODE1;
			break;   

		case INIT_SEQ_CR_TX_REQMODE1:            // ＲＯＭバージョン取得受信

			//現在ACKのみ判定

			init_seq = INIT_SEQ_CR_TX_REQMODE2;
			break;   

		case INIT_SEQ_CR_TX_REQMODE2:            // モジュールタイプ取得受信

			//現在ACKのみ判定

			init_seq = INIT_SEQ_CR_TX_REQMODE3;
			break;   

		case INIT_SEQ_CR_TX_REQMODE3:            // ＲＯＭバージョン取得受信

			//現在ACKのみ判定


			mainstatus = STATUS_NOR1; 			 // 通常動作中
			next_mainstatus = STATUS_NOR1; 		 // 通常動作中
			init_seq = INIT_SEQ_CR_COMPLETE;

			//YP SERIALに初期設定完了通知
			nor_yp_tx_dataset(YP_TX_PRA_STATUS);

			break;   
	}
	

	// カードリーダ初期設定送信
	if (init_seq < INIT_SEQ_CR_COMPLETE)
	{
		init_cr_tx_dataset();
	}
}


//******************************************
//		カードリーダ　イニシャル設定
//		イニシャル開始 
//      NACK　受信
//******************************************
void	tsk008(void)
{
	switch (init_seq)
	{

		case INIT_SEQ_CR_TX_SETINVENTORY:        // INVENTORY受信

			// 再送信			
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD1:          // 通信モード設定受信

			// 再送信			
			break;   

		case INIT_SEQ_CR_TX_SETMODE:        	// 動作モード設定受信

			// 再送信			
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD2:       	// 通信モード設定受信

			// 再送信			
			break;   

		case INIT_SEQ_CR_TX_REQMODE1:            // ＲＯＭバージョン取得受信

			// 再送信			
			break;   

		case INIT_SEQ_CR_TX_REQMODE2:            // モジュールタイプ取得受信

			// 再送信			
			break;   

		case INIT_SEQ_CR_TX_REQMODE3:            // ＲＯＭバージョン取得受信

			// 再送信			
			break;   
	}


}

/*------------------------------------------*/
/*	ＩＮＩＴ()	初期化処理					*/
/*------------------------------------------*/
const	struct	jobtb tb_INIT[] =
{
    {QAPEV_INIT_CMDA, STATUS_INIT,tsk000},		// 受信
    {QAPEV_INIT_CMDB, STATUS_INIT,tsk001},		// 受信
    {QAPEV_INIT_CMDC, STATUS_INIT,tsk002},		// 受信
    {QAPEV_INIT_CMDD, STATUS_INIT,tsk003},		// 受信
    {QAPEV_INIT_CMDE, STATUS_INIT,tsk004},		// 受信
    {QAPEV_INIT_CMDF, STATUS_INIT,tsk005},		// 受信
    {QAPEV_INIT_CMDG, STATUS_INIT,tsk006},		// 受信


    {CR_COM_ACK, STATUS_INIT,tsk007},			// ACK受信
    {CR_COM_NACK,STATUS_INIT,tsk008},       	// NACK受信

    {  		   0,    	   0,     0},           // 終端
};



//******************************************
//		カードリーダ　通常時
//		応答受信
//
//******************************************
void	tsk010(void)
{

		// テスト　test
		
init_seq = 1;
		nor_yp_tx_dataset(YP_TX_PRA_STATUS); // YP Serialへ送信
		next_mainstatus = STATUS_NOR2; 		 // 通常動作中
}

void	tsk011(void)
{
}

void	tsk012(void)
{
		// テスト test
init_seq = 3;
		nor_yp_tx_dataset(YP_TX_PRA_STATUS);				// YP Serialへ送信
		next_mainstatus = STATUS_NOR1; 		 // 通常動作中
}


/*------------------------------------------*/
/*		通常動作　NOR1						*/
/*------------------------------------------*/
const	struct	jobtb tb_NOR1[] =
{
    {CR_COM_ACK, STATUS_INIT,tsk010},			// ACK受信

    {CR_COM_NACK,STATUS_INIT,tsk011},       	// NACK受信

    {  		   0,    	   0,     0},           // 終端
};


/*------------------------------------------*/
/*		通常動作　NOR2						*/
/*------------------------------------------*/
const	struct	jobtb tb_NOR2[] =
{
    {CR_COM_ACK, STATUS_INIT,tsk012},			// ACK受信

    {CR_COM_NACK,STATUS_INIT,tsk001},       	// NACK受信

    {  		   0,    	   0,     0},           // 終端
};


/*---------------------------------------------------------------
	ｲﾍﾞﾝﾄﾃｰﾌﾞﾙ検索ﾃｰﾌﾞﾙ
----------------------------------------------------------------*/
const struct jobtb *const tbevad[] = {
	tb_INIT,						/* 初期化						*/
	tb_NOR1,						/* 通常モードＡ					*/
	tb_NOR2,						/* 通常モードＡ（予備）			*/
};

//***************************************************
//		カードリーダからのデータ
//***************************************************

static Bool parse_crrx_serial( const U1 *p_buff, int len , int evnt)
{

	const struct jobtb	*p;
//	U1 evnt,i;
	
#if 0
	// 受信データ抽出
    for (i=0 ; i < len; i++){
		cr_rev_data[i] = p_buff[i];		
	}

	evnt = p_buff[CR_IDX_CMD];             /* イベント(＝受信コマンド)取り出し  */
#endif

   	/* タスク検索    */
    for (p = tbevad[mainstatus] ; p->evnt != 0; p++){

    	if ( p->evnt == evnt){	                             /* ｲﾍﾞﾝﾄが一致するまで検索   */
        	next_mainstatus = p->next;                       /* 次状態番号を読み込み      */
            (*(p->tsk))();                                   /* ﾀｽｸを実行する             */
            if(next_mainstatus > 0)
            {
            	if(mainstatus != next_mainstatus)
            	{    								        /* 次状態設定           */
                	mainstatus = next_mainstatus;
                }
            }
            break;
        }
	}

    return true;
}







/* USER CODE END 0 */
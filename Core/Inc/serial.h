/******************************************************************************
 *
 *	開発機種			：免許証リーダ制御用アダプタ
 *	Micro-processingUnit：STM32G071GBU
 *-----------------------------------------------------------------------------
 *	機能名称	：ＹＰシリアル通信制御
 *	ファイル名	：serial.h
 *	ファイル説明：ＹＰシリアル通信制御関連定義ヘッダー
 *	作成日		：2021年 x月 x日
 *	作成者		：------
 *
 *			Copyright(C)	2016 YUPITERU Ltd. All rights reserved.
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

#ifndef __SERIAL_H__
#define	__SERIAL_H__




/*------------------------------------------------------------------*/
/*	定数定義														*/
/*------------------------------------------------------------------*/

/* UART 設定パラメータ定義		*/
#define UART_UiBRG			(0x67)			/* UARTiビットレートレジスタ(UiBRG)(i=0、1、3)	*/
												/* Bitﾚｰﾄ	n =  ( f1 / (9600 * 16 ) )  - 1	*/
#define	UART_UiMR			(0x15)			/* UARTi送受信モードレジスタ(UiMR)(i=0、1、3)	*/
												/* UARTﾓｰﾄﾞ8Bitﾓｰﾄﾞ  Stop=2Bit  ﾊﾟﾘﾃｨなし	*/
#define	UART_UiC0			(0x00)			/* UARTi送受信制御レジスタ0 (UiC0)(i=0、1、3)	*/
												/* LSBファースト　f1選択　　CMOS出力		*/
#define	UART_UiC1			(0x10)			/* UART2送受信制御レジスタ1 (UiC1)	*/


#define	YPS_TMUNIT			(5)				/* ﾀｲﾏｶｳﾝﾄ単位			*/
#define	YPS_CYCSNDTM		1000/YPS_TMUNIT	/* 周期送信間隔時間1Sec	*/
#define	YPS_RXWTHTM			50/YPS_TMUNIT	/* 受信監視時間50mSec	*/
#define	YPS_TXWTHTM			20/YPS_TMUNIT	/* 送信監視時間20mSec	*/

/* ｼﾘｱﾙｵｰﾌﾟﾝﾓｰﾄﾞ定義 */
#define	YPS_MODE_NORMAL		(0)				/* ｼﾘｱﾙ通信起動ﾓｰﾄﾞ		*/
#define	YPS_MODE_TEST		(1)				/* 周期通知動作起動ﾓｰﾄﾞ	*/



/* ﾃﾞｰﾀ送信先ｱﾄﾞﾚｽ定義 */
#define	YPS_ADR_ALL			(0)				/* 送信先ｱﾄﾞﾚｽ=ﾌﾞﾛｰﾄﾞｷｬｽﾄ			*/
#define	YPS_ADR_RD			(1)				/* 送信先ｱﾄﾞﾚｽ=RD  ﾚｰﾀﾞｰ			*/
#define	YPS_ADR_PND			(2)				/* 送信先ｱﾄﾞﾚｽ=PND 					*/
#define	YPS_ADR_DR			(3)				/* 送信先ｱﾄﾞﾚｽ=DR  ﾄﾞﾗｲﾌﾞﾚｺｰﾀﾞｰ		*/
#define	YPS_ADR_OTH			(4)				/* 送信先ｱﾄﾞﾚｽ=その他				*/
#define	YPS_ADR_ECO			(5)				/* 送信先ｱﾄﾞﾚｽ=ECO-280燃費計		*/
#define	YPS_ADR_OBD			(0x8)			/* 送信元ｱﾄﾞﾚｽ=ODBｱﾀﾞﾌﾟﾀ			*/
#define YPS_ADR_LOWS        (0x9)			/* 送信元ｱﾄﾞﾚｽ=よそ見検知装置		*/
#define YPS_ADR_FCWS        (0xA)			/* 送信元ｱﾄﾞﾚｽ=車間検知装置			*/
#define YPS_ADR_OPTA        (0xF)			/* 送信元ｱﾄﾞﾚｽ=Option Adapter自身	*/


/* YPｼﾘｱﾙ通信ｺﾏﾝﾄﾞ定義		*/
#define		YPS_CMD_CYCL1		0x81		/* 定期通信1ｺﾏﾝﾄﾞ			*/
//#define		YPS_CMD_CYCL2		0x82		/* 定期通信2ｺﾏﾝﾄﾞ			*/
#define		YPS_CMD_CYCL2_1		0x82		/* 定期通信2ｺﾏﾝﾄﾞ_0			*/
#define		YPS_CMD_CYCL2_2		0x83		/* 定期通信2ｺﾏﾝﾄﾞ_1			*/
#define		YPS_CMD_CYCL3		0x91		/* 定期通信3ｺﾏﾝﾄﾞ			*/
#define		YPS_CMD_CYCL4		0x92		/* 定期通信4ｺﾏﾝﾄﾞ			*/
#define		YPS_CMD_CYCL5		0x93		/* 定期通信5ｺﾏﾝﾄﾞ			*/

#define		YPS_CMD_NOCOMM		0x84		/* 車両通信NG				*/
#define		YPS_CMD_FBRAKE		0x85		/* ﾌﾞﾚｰｷON/OFF				*/
#define		YPS_CMD_TURNSIG		0x86		/* ﾀｰﾝｼｸﾞﾅﾙON/OFF			*/
#define		YPS_CMD_PWROFF		0x8F		/* 電源OFF通知				*/

#define		YPS_CMD_REQ_TROUBLE	0x01		/* 故障ｺｰﾄﾞ要求				*/
#define		YPS_CMD_RPY_TROUBLE	0x41		/* 故障ｺｰﾄﾞ応答				*/
#define		YPS_CMD_REQ_CARTYP	0x02		/* 車両ﾀｲﾌﾟ要求				*/
#define		YPS_CMD_RPY_CARTYP	0x42		/* 車両ﾀｲﾌﾟ応答				*/
#define		YPS_CMD_REQ_DELTRBL	0x03		/* 故障ｺｰﾄﾞ消去要求			*/
#define		YPS_CMD_RPY_DELTRBL	0x43		/* 故障ｺｰﾄﾞ消去応答			*/
#define		YPS_CMD_REQ_KLINE	0x04		/* ﾀﾞｲﾚｸﾄｺﾏﾝﾄﾞ要求(K-Line)	*/
#define		YPS_CMD_RPY_KLINE	0x44		/* ﾀﾞｲﾚｸﾄｺﾏﾝﾄﾞ(K-Line)応答	*/
#define		YPS_CMD_REQ_CAN		0x05		/* ﾀﾞｲﾚｸﾄｺﾏﾝﾄﾞ要求(CAN)		*/
#define		YPS_CMD_RPY_CAN		0x45		/* ﾀﾞｲﾚｸﾄｺﾏﾝﾄﾞ(CAN)応答		*/

#define		YPS_CMD_REQ_GETINF	0x06		/* 取得情報要求				*/
#define		YPS_CMD_RPY_GETINF	0x46		/* 取得情報応答				*/
#define		YPS_CMD_REQ_CYCCHG	0x07		/* 通信周期切り替え要求		*/
#define		YPS_CMD_RPY_CYCCHG	0x47		/* 通信周期切り替え応答		*/
#define		YPS_CMD_REQ_SPDCHG	0x08		/* 通信速度切替え要求		*/
#define		YPS_CMD_RPY_SPDCHG	0x48		/* 通信速度切り替え応答		*/
#define		YPS_CMD_REQ_SFTVER	0x09		/* SoftVersion要求			*/
#define		YPS_CMD_RPY_SFTVER	0x49		/* Softversion応答			*/

#define		YPS_CMD_REQ_MAKER	0x0A		/* ﾒｰｶｰ通知					*/
#define		YPS_CMD_RPY_MKRACK	0x4A		/* ﾒｰｶｰ通知応答				*/
#define		YPS_CMD_RPY_MKRNG	0x4B		/* ﾒｰｶｰ変更NG応答			*/
#define		YPS_CMD_RPY_TSTCHG	0x0F		/* ﾃｽﾄﾓｰﾄﾞ切り替え要求		*/


#define		YPS_CMD_LKOW_CYCL	0xB0		/* わき見検知カメラ周期情報	*/
#define		YPS_CMD_FCWS_CYCL	0xB1		/* 車間距離検知周期情報		*/

#define		YPS_CMD_FCWS_INFO	0xC1		/* 車間距離検知装置への送信	*/


/* 応答ｺｰﾄﾞ定義 */
#define		YPS_ACKCMD_OKRES		0x00	/* 受付OK応答				*/
#define		YPS_ACKCMD_NOINFKRES	0x01	/* 受付NG1応答[無効ﾃﾞｰﾀ取得/K-LINE通信]	*/
#define		YPS_ACKCMD_NGRES		0x02	/* 受付NG2応答[指定最大数越え]			*/






/*------------------------------------------*/
/*	ｼﾘｱﾙ通信ﾊﾞｯﾌｧｻｲｽﾞ定義					*/
/*------------------------------------------*/
#define		YPS_HAEDER			(4 + 1) 		/* ﾍｯﾀﾞｰ部MaxLength値	*/

#define		CRS_HAEDER			(7) 			/* ﾍｯﾀﾞｰ部MaxLength値	*/













/*------------------------------------------*/
/*	受信ﾃﾞｰﾀﾊﾞｯﾌｧ型定義						*/
/*------------------------------------------*/
/* YPｼﾘｱﾙ通信ﾃﾞｰﾀﾌﾚｰﾑﾌｫｰﾏｯﾄ型定義 */
typedef	struct {
//	BUFFERHDR	mhdr;						/* メッセージブロックヘッダー */
	union {
		U1	buf[YPS_RCV_BUF_SIZE];
		struct{
			U1	sa_ta;						/* 上位ﾆｲﾌﾞﾙ：送信元ｱﾄﾞﾚｽ 下位ﾆｲﾌﾞﾙ：送信先ｱﾄﾞﾚｽ */
			U1	len;						/* ﾃﾞｰﾀ長				*/
			U1	sclmbl;						/* ｽｸﾗﾝﾌﾞﾙｺｰﾄﾞ			*/
			U1	cmd;						/* ｺﾏﾝﾄﾞｺｰﾄﾞ			*/

		} part;
	} data;
} YPSRXBUF;



/* YPｼﾘｱﾙ通信ﾃﾞｰﾀﾌﾚｰﾑﾌｫｰﾏｯﾄ型定義 */
typedef	struct {
//	BUFFERHDR	mhdr;						/* メッセージブロックヘッダー */
	union {
		U1	buf[YPS_RCV_BUF_SIZE];
		struct{
			U1	sa_ta;						/* 上位ﾆｲﾌﾞﾙ：送信元ｱﾄﾞﾚｽ 下位ﾆｲﾌﾞﾙ：送信先ｱﾄﾞﾚｽ */
			U1	len;						/* ﾃﾞｰﾀ長				*/
			U1	sclmbl;						/* ｽｸﾗﾝﾌﾞﾙｺｰﾄﾞ			*/
			U1	cmd;						/* ｺﾏﾝﾄﾞｺｰﾄﾞ			*/

		} part;
	} data;
} YPSTXBUF;




/* 送信制御情報型定義(割り込み処理制御用)  */
typedef		struct	{
	U1		stat;						/* 送信状態				*/

	YPSTXBUF	tx;						/* ｽﾛｯﾄ別送信ﾊﾞｯﾌｧ		*/
	U2		cnt;						/* 送信ﾃﾞｰﾀｶｳﾝﾀ			*/
	U2		len;						/* 送信ﾃﾞｰﾀ長			*/
} YPSSNDCTL;


/* 受信制御情報型定義(割り込み処理制御用)  */
typedef		struct	{
	U1		stat;						/* 受信状態				*/
	U1		len;						/* 受信ﾃﾞｰﾀ長			*/
	U1		cnt;						/* 受信ﾃﾞｰﾀ長			*/
	U1		err;						/* 受信ｴﾗｰ情報			*/
	
	YPSRXBUF	rx;						/* 接続YP製品からのｺﾏﾝﾄﾞ受信ﾊﾞｯﾌｧｱﾄﾞﾚｽ*/

	YPSRXBUF	p_rxbuff;				/* １ﾌﾚｰﾑ分の受信を完了した受信ﾊﾞｯﾌｧｱﾄﾞﾚｽ(受信がない場合、NULL)*/
} YPSRCVCTL;


/* ｼﾘｱﾙ通信制御周期処理用の型定義 */
typedef		struct	{
	U1		req;						/* 制御要求				*/
	U1		stat;						/* 制御状態				*/

	U2		rxtmr;						/* ﾃﾞｰﾀ受信監視ﾀｲﾏ用	*/
	U2		txtmr;						/* ﾃﾞｰﾀ受信監視ﾀｲﾏ用	*/

	U1		tx_state;					/* 送信制御状態			*/

} YPS_SERIAL;




/* ｼﾘｱﾙ通信制御周期処理用の型定義 */
typedef		struct	{
	U1		req;						/* 制御要求				*/
	U1		stat;						/* 制御状態				*/

	U2		rxtmr;						/* ﾃﾞｰﾀ受信監視ﾀｲﾏ用	*/
	U2		txtmr;						/* ﾃﾞｰﾀ受信監視ﾀｲﾏ用	*/

	U1		tx_state;					/* 送信制御状態			*/

} CRS_SERIAL;


/*------------------------------------------------------------------*/
/*	外部参照関数定義												*/
/*------------------------------------------------------------------*/

#endif

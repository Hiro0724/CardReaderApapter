/******************************************************************************
 *
 *	開発機種			：免許証リーダ制御用アダプタ
 *	Micro-processingUnit：STM32G071GBU
 *-----------------------------------------------------------------------------
 *	機能名称	：メモリバッファ管理用ヘッダーファイル
 *	ファイル名	：sharedbuffer.h
 *	ファイル説明：共有メモリバッファフォーマット型定義用
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
 #ifndef	__SHAREDMEMORY__
 #define	__SHAREDMEMORY__
 




/*-------------------------------------------------------*
 *	定数定義
 *-------------------------------------------------------*/ 
#define		SB_OFF		(0)				/* 共通メモリバッファ用　OFF定義	*/
#define		SB_ON		(1)				/* 共通メモリバッファ用　ON定義		*/

#define		SB_OK		(0)				/* 正常	*/
#define		SB_NG		(1)				/* 異常	*/
#define		SB_BLOCKNG	(2)				/* 解放するメモリブロックが違う		*/
#define		SB_RELEASENG (3)			/* 解放要求したモジュールが正しくない*/
#define		SB_BLOCKBUSY (4)			/* 使用中モジュール有り				*/


/*------------- バッファID定義 ----------------*/
#define		SBID_NONE	(0)
#define		SBID_BIG	(1)				/* 112Byte単位メモリブロック		*/
#define		SBID_LITTLE (2)				/* 48Byte単位メモリブロック			*/



/*------------- モジュールID定義(リリース) --------------*/
#define		NONE_ID (0xFF)				/* 使用モジュール未確定				*/
#define		OBD2COMM_MODULE		0x01	/* ＯＢＤ２通信モジュール			*/
#define		DTCTCOMM_MODULE		0x02	/* 検知機器通信モジュール			*/
#define		YPDVCOMM_MODULE		0x04	/* ＹＰ機器通信モジュール			*/



/*------------- バッファサイズ定義 --------------*/
#define		CRD_RCV_BUF_SIZE	(127)	/* カードリーダシリアル受信用バッファサイズ 	*/
#define		YPS_RCV_BUF_SIZE	(20)	/* 共通シリアル受信用バッファサイズ				*/

/*------------- 通信デバイスID ------------------*/
#define		YP_DEV_ID			0x01	/* YP製品デバイス					*/
#define		OBD2_DEV_ID			0x02	/* OBD2アダプターデバイス			*/
#define		DTC_DEV_ID			0x03	/* 検知機器							*/
//#define		FCWS_DEV_ID			0x03	/* 車間距離検知デバイス				*/
//#define		LKOW_DEV_ID			0x04	/* よそ見検知カメラ					*/

/*------------- ブロックバッファ数定義 ----------*/
#define		RAM_OBDBUFFER_MAX	(10)		/* 対OBDアダプタ受信専用バッファ最大個数定義 */
#define		RAM_STDBUFFER_MAX	(40)	/* シリアル通信用バッファ最大個数定義*/



/* YPフォーマット通信送受信ステータス用構造体定義 */
typedef		struct	{
	U1			rxreq;						/* 受信データ有り状態			*/
	U1			txreq;						/* 送信データ有り状態			*/
} YPS_STATUS;







/*-------------------------------------------------------*
 *	共通バッファメモリ型定義
 *-------------------------------------------------------*/


/*-------------------------------------------------------*
 *	送受信キュー型定義
 *-------------------------------------------------------*/
typedef struct {
	void *p_tx_top_pri;			/* 送信ﾊﾞｯﾌｧ優先ｷｭｰ					*/
	void *p_tx_top;				/* 送信ﾊﾞｯﾌｧｷｭｰ						*/
	void *p_rx_top;				/* 受信ﾊﾞｯﾌｧｷｭｰ						*/

	U1		enablef;				/* 通信イネーブルフラグ				*/
	U1		connectionDevice;		/* 通信しているデバイスID			*/

} DEVCOMMQUE;



/*-------------------------------------------------------*
 *	外部参照関数定義
 *-------------------------------------------------------*/
void	SB_Initialize( void );

void * SB_IntGetOBDMemory(
	U1	releaseID		/* リリース先モジュールID指定		*/
);


void * SB_IntGetMemory(
	U1	getID,			/* メモリブロック取得元ＩＤ　指定	*/
	U1	releaseID		/* リリース先モジュールID指定		*/
);

void * SB_GetOBDMemory(
	U1	releaseID		/* リリース先モジュールID指定		*/
);

U1 SB_ReleaseMemory(
	U1	releaseID,		/* 解放するモジュールＩＤ			*/
	void 	*release		/* 解放するメモリ先頭アドレス		*/
);

void SB_AllMemoryRelease(
	U1	releaseID,		/* 解放するモジュールＩＤ			*/
 	void 	*release		/* 解放するメモリ先頭アドレス		*/
);

void *SB_QueOutBuffer(
	void	**quetop			/* キューの先頭アドレス				*/
);

void SB_QueInBuffer(
	void	**quetop,		/* キューの先頭アドレス				*/
	void	*buffer			/* 登録するバッファの先頭アドレス	*/
);
 
 #endif
/*******************************************************************************/
/*******************************************************************************/
/******************************** End Of File **********************************/
/*******************************************************************************/
/*******************************************************************************/
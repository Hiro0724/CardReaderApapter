/* ************************************************************ */
/*  															*/
/*	FILE NAME = 'intevt.h'		                        		*/
/*	概要：割り込みレベル発行イベント定義						*/
/*																*/
/*																*/
/*																*/
/*																*/
/*	SUB  CLOCK:----												*/
/* ************************************************************ */

#ifndef INTEVT_H
#define INTEVT_H

/*==============================================================*/
/* [ 定義宣言 ]                                         		*/
/* 																*/
/*==============================================================*/
/*==============================================================*/
/* [ 上位タスク発行イベント定義宣言 ]                           */
/* 	アプリーケションで編集前のイベント							*/
/*  注意：														*/
/*		(1)キーの連続検出はカーソルキーのみ						*/
/*		(2)キーの連続件検出か通常のキー検出かの判定は付加情報	*/
/*		   連続押下はキー検出から1秒後に100ms周期で発行			*/
/*		(3)FUNC+Xのｲﾍﾞﾝﾄはｱﾌﾟﾘで編集すること					*/
/*		(4)DLEYイベントの秒数は付加情報を参照					*/
/*		(5)タイマイベントではアプリでｵｰﾄﾊﾟﾜｰｵﾌ以降に設定		*/
/*		(6)ロータリー検出では、回転数は付加情報に格納			*/
/*		   0〜１２７＝右回転 255〜128は左回転 0がセンター		*/
/*		(7)LOCKｲﾍﾞﾝﾄは前回はUNLOCKの場合のみ発行される			*/
/*==============================================================*/
enum {
	/* 0x00---0x0f:イベントの割り当ては禁止	*/
	QAPEV_NONE = 0,				/* イベントなし判定用								*/
	/*                             	イベント		|	付加情報					*/

	/* アプリケーション用のイベントをここに追加してはいけない						*/

	QAPEV_TSTRX	= (uint8_t)0x20,	/* テスト受信								0x30*/
	QAPEV_TST1,						/* テスト予約								0x31*/
	QAPEV_TST2,						/* テスト予約								0x32*/
	
	/* アプリケーション用のイベントを追加する場合にはこの間に追加する				*/


	QAPEV_INIT = (uint8_t)0x40,		/* 初期イベント								0x40*/
	QAPEV_CMDA,						/* CMD A受信								0x41*/
	QAPEV_CMDB,						/* CMD B受信								0x42*/
	QAPEV_CMDC,						/* CMD C受信								0x43*/
	QAPEV_CMDD,						/* CMD D受信								0x44*/
	QAPEV_CMDE,						/* CMD E受信								0x45*/
	QAPEV_CMDF,						/* CMD F受信								0x46*/
	QAPEV_CMDG,						/* CMD G受信								0x47*/
    QAPEV_CMDH,                     /* CMD H受信                                0x48*/
	QAPEV_CMDI,						/* CMD I受信								0x49*/
	QAPEV_CMDJ,						/* CMD J受信								0x4a*/
	QAPEV_CMDK,						/* CMD K受信								0x4b*/
    QAPEV_CMDL,                     /* CMD L送信                                0x4c*/
    QAPEV_CMDM,                     /* CMD M送信                                0x4d*/
	QAPEV_CMDN,						/* CMD N送信								0x4e*/
	QAPEV_CMDO, 					/* CMD O送信								0x4f*/
	QAPEV_CMDP,						/* CMD P送信								0x50*/
	QAPEV_CMDQ,						/* CMD Q送信   								0x51*/
	QAPEV_CMDR,						/* CMD R送信								0x52*/
	QAPEV_CMDS,						/* CMD S送信								0x53*/
	QAPEV_CMDT,						/* CMD T送信								0x54*/
	QAPEV_CMDU,						/* CMD U送信								0x55*/

	/* タイマーイベントforアプリ	*/
	QAPEV_TMEVT = (uint8_t)0x60,	/* タイマUPイベント	60h以上はタイマイベント	*/
	QAPEV_TSTTM,					/* テストタイマ								0x61*/

	QAPEV_END	= (U1)0xFF,			/* 終端										0xFF*/
};


// Card Reader COMMAND 
#define CR_COM_REQMOD    	(0x4F)
#define CR_COM_SETMOD   	(0x4E)
#define CR_COM_ONBUZZ    	(0x42)
#define CR_COM_CNTPORT		(0x75)
#define CR_COM_TIMINVENTORY (0x6F)
#define CR_COM_SETBPS		(0x7B)
#define CR_COM_READCARDPRA 	(0x78)
#define CR_COM_SETCOMMOD   	(0x70)
#define CR_COM_SETINVENTORY (0x71)
#define CR_COM_POLLING     	(0x72)
#define CR_COM_SETCARDCOMM 	(0x73)
#define CR_COM_MULTIPOLLING (0x81)
#define CR_COM_TYPEACOMM    (0x83)
#define CR_COM_TYPEBCOMM    (0x84)

#define CR_COM_ACK			(0x30)
#define CR_COM_NACK			(0x31)


#define CR_COM_INITIAL    	(0xFF)



typedef struct{
	U1	cmd;
	const	U2	*str;				// 文字列ポインタ
}ST_CRDATA;

							   //   LEN DATA
const U1 TBL_STR_SETINVENTORY[] =	{8,	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const U1 TBL_STR_SETCOMMOD1[] =		{1,	0x00};
const U1 TBL_STR_SETMODE[] =		{7,	0x00,0x00,0x00,0x80,0x00,0x00,0x00};
const U1 TBL_STR_SETCOMMOD2[] =		{1, 0x00};
const U1 TBL_STR_REQMODE1[] =		{2, 0x90,0x00};
const U1 TBL_STR_REQMODE2[] =		{2, 0x92,0x00};
const U1 TBL_STR_REQMODE3[] =		{2, 0x90,0x00};
const U1 TBL_STR_TERM[] =			{1, 0x00};
//const U1 TBL_STR_[] =		{	};


// カードリーダ　イニシャル用コマンド＆送信データ部のみ
const ST_CRDATA CR_TX_INI_DATA_TBL[] = {
	{ CR_COM_SETINVENTORY,	&TBL_STR_SETINVENTORY[0]	},			// 
	{ CR_COM_SETCOMMOD,		&TBL_STR_SETCOMMOD1[0]		},			// 
	{ CR_COM_SETMOD,		&TBL_STR_SETMODE[0]			},			// 
	{ CR_COM_SETCOMMOD,		&TBL_STR_SETCOMMOD2[0]		},			// 
	{ CR_COM_REQMOD,		&TBL_STR_REQMODE1[0]		},			// 
	{ CR_COM_REQMOD,		&TBL_STR_REQMODE2[0]		},			// 
	{ CR_COM_REQMOD,		&TBL_STR_REQMODE3[0]		},			// 

	{ 0x00,					&TBL_STR_TERM[0]			},			//終端 
};


// カードリーダ　通常動作時コマンド＆送信データ部のみ
const ST_CRDATA CR_TX_NOR_DATA_TBL[] = {
	{ CR_COM_SETINVENTORY,	&TBL_STR_SETINVENTORY[0]	},			// 
	{ CR_COM_SETCOMMOD,		&TBL_STR_SETCOMMOD1[0]		},			// 
	{ CR_COM_SETMOD,		&TBL_STR_SETMODE[0]			},			// 
	{ CR_COM_SETCOMMOD,		&TBL_STR_SETCOMMOD2[0]		},			// 
	{ CR_COM_REQMOD,		&TBL_STR_REQMODE1[0]		},			// 
	{ CR_COM_REQMOD,		&TBL_STR_REQMODE2[0]		},			// 
	{ CR_COM_REQMOD,		&TBL_STR_REQMODE3[0]		},			// 

	{ 0x00,					&TBL_STR_TERM[0]			},			//終端 
};




// YPSerial TX COMMAND 

#define YP_COM_STATUS		(0xA0)
#define YP_COM_RESET  		(0xA1)
#define YP_COM_REQKIND		(0xA2)
#define YP_COM_IDDATA		(0xA5)


							   //   LEN DATA
const U1 TBL_STR_ANSSTATUS[] =		{2,	0x00,0x00};
const U1 TBL_STR_ANSRESET[] =		{1,	0x00};
const U1 TBL_STR_ANSKIND[] =		{1,	0x00};
const U1 TBL_STR_ANSDATA[] =		{15, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };



// YPシリアル　通常動作時コマンド＆送信データ部のみ
const ST_CRDATA YP_TX_NOR_DATA_TBL[] = {
	{ YP_COM_STATUS,		&TBL_STR_ANSSTATUS[0]		},			// 
	{ YP_COM_RESET,			&TBL_STR_ANSRESET[0]		},			// 
	{ YP_COM_REQKIND,		&TBL_STR_ANSKIND[0]			},			// 
	{ YP_COM_IDDATA,		&TBL_STR_ANSDATA[0]			},			// 

	{ 0x00,					&TBL_STR_TERM[0]			},			//終端 

};









/*==============================================================*/
/* [ 上位タスク発行イベント付加情報定義宣言 ]                   */
/* 																*/
/*==============================================================*/
/*==============================================================*/
/* [ 変数宣言 ]                                         		*/
/* 																*/
/*==============================================================*/





#endif



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


enum {
	/* 0x00---0x0f:イベントの割り当ては禁止	*/
	QAPEV_NONE = 0,					/* イベントなし判定用							*/
	/*                             	イベント		|	付加情報					*/

	/* アプリケーション用のイベントをここに追加してはいけない						*/

	/* keys		キーイベント内で入れ替え禁止	*/
	QAPEV_NKEY1  = (uint8_t)0x20,	/* key1						|				0x20*/
	QAPEV_NKEY2,					/* key2						|				0x21*/
	QAPEV_NKEY3,					/* SW						|				0x22*/
	QAPEV_DKY12,					/* KEY1 + KEY2				|				0x23*/
	QAPEV_DKY13,					/* KEY1 + KEY3				|				0x24*/
	QAPEV_DKY23,					/* KEY2 + KEY3				|				0x25*/
	QAPEV_TKEYS,					/* TEST SW					|				0x26*/

	QAPEV_TSTRX	= (uint8_t)0x30,	/* テスト受信								0x30*/
	QAPEV_REV0,						/* 											0x31*/
	QAPEV_REV1,						/* 											0x32*/
	QAPEV_REV2,						/* 											0x33*/
	QAPEV_REV3,						/* 											0x34*/
	QAPEV_REV4,						/* 											0x35*/
	
	/* アプリケーション用のイベントを追加する場合にはこの間に追加する				*/
	QAPEV_INIT = (uint8_t)0x40,		/* 初期イベント								0x40*/
	QAPEV_INIT_CMDA,				/* CMD A受信								0x41*/
	QAPEV_INIT_CMDB,				/* CMD B受信								0x42*/
	QAPEV_INIT_CMDC,				/* CMD C受信								0x43*/
	QAPEV_INIT_CMDD,				/* CMD D受信								0x44*/
	QAPEV_INIT_CMDE,				/* CMD E受信								0x45*/
	QAPEV_INIT_CMDF,				/* CMD F受信								0x46*/
	QAPEV_INIT_CMDG,				/* CMD G受信								0x47*/
    QAPEV_INIT_CMDH,                /* CMD H受信                                0x48*/
	QAPEV_INIT_CMDI,				/* CMD I受信								0x49*/
	QAPEV_INIT_CMDJ,				/* CMD J受信								0x4a*/
	QAPEV_INIT_CMDK,				/* CMD K受信								0x4b*/

    QAPEV_NOR_CMDA,                 /* CMD L送信                                0x4c*/
    QAPEV_NOR_CMDB,                 /* CMD M送信                                0x4d*/
	QAPEV_NOR_CMDC,					/* CMD N送信：リモコンユニットのRSSI通知	0x4e*/
	QAPEV_NOR_CMDD, 				/* CMD O送信：(未使用)          			0x4f*/
	QAPEV_NOR_CMDE,					/* CMD P送信：マニュアル信号				0x50*/
	QAPEV_NOR_CMDF,					/* CMD Q送信：スマートACK信号(旧圏外通知)   0x51*/
	QAPEV_NOR_CMDG,					/* CMD R送信：RF情報通知					0x52*/
	QAPEV_NOR_CMDH,					/* CMD S送信：リモコン1ＩＤ通知				0x53*/
	QAPEV_NOR_CMDI,					/* CMD T送信：リモコン2ＩＤ通知				0x54*/
	QAPEV_NOR_CMDJ,					/* CMD U送信：ID書込通知					0x55*/

	/* タイマーイベントforアプリ	*/
	QAPEV_TMEVT = (uint8_t)0x60,	/* タイマUPイベント	60h以上はタイマイベント	*/
									/* 状態により名称を追加する。					*/
	QAPEV_CRRESTM,					/* カードリーダ応答タイムオーバー			0x61*/
	QAPEV_CRRESTM_DECI,				/* カードリーダ応答タイムオーバー確定		0x62*/
	QAPEV_KEYTM,					/* キー操作タイマUP							0x6*/
	QAPEV_TSTTM,					/* テストタイマ								0x6*/
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
const U2 TBL_STR_SETINVENTORY[] =	{8,	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const U2 TBL_STR_SETCOMMOD1[] =		{1,	0x00};
const U2 TBL_STR_SETMODE[] =		{7,	0x00,0x00,0x00,0x80,0x00,0x00,0x00};
const U2 TBL_STR_SETCOMMOD2[] =		{1, 0x00};
const U2 TBL_STR_REQMODE1[] =		{2, 0x90,0x00};
const U2 TBL_STR_REQMODE2[] =		{2, 0x92,0x00};
const U2 TBL_STR_REQMODE3[] =		{2, 0x90,0x00};
const U2 TBL_STR_TERM[] =			{1, 0x00};
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
const U2 TBL_STR_ANSSTATUS[] =		{3,	0x00,0x00,0x00};
const U2 TBL_STR_ANSRESET[] =		{1,	0x00};
const U2 TBL_STR_ANSKIND[] =		{1,	0x00};
const U2 TBL_STR_ANSDATA[] =		{15, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };



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



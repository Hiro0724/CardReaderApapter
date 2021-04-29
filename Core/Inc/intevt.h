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



/******************************************************************************
 *
 *	開発機種			：免許証リーダ制御用アダプタ
 *	Micro-processingUnit：STM32G071GBU
 *-----------------------------------------------------------------------------
 *	機能名称	：共通処理関数定義
 *	ファイル名	：common.c
 *	ファイル説明：共通処理関数(ﾃﾞｰﾀｺﾋﾟｰ、比較、ｸﾘｱ、ﾍﾟﾘﾌｪﾗﾙの設定／制御)
 *	作成日		：2021年 x月 x日
 *	作成者		：------
 *
 *			Copyright(C)	2016 YUPITERU Ltd.  All rights reserved.
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



//--------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------
#include	"cardreader.h"




//--------------------------------------------------------------------------
// マクロ定義
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// 内部型定義
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// 変数定義
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// 内部関数プロトタイプ宣言
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// 外部関数定義
//--------------------------------------------------------------------------



/*****************************************************************************
 *
 *		関数名	：void	c_memcpy( )
 *
 *		機能概要：指定サイズ分メモリをコピーする。
 *
 *		入力	：
 *				U1	*	dptr	out	 コピー先アドレス
 *				U1	*	sptr	in	 コピー元アドレス
 *				U2		size	in	 コピーサイズ
 *
 *		出力	：なし
 *
 *		戻り値	：なし
 *
 *****************************************************************************/
void c_memcpy(
	U1			*dptr,		/* ｺﾋﾟｰ先ｱﾄﾞﾚｽ格納用 */
	U1			*sptr,		/* ｺﾋﾟｰ元ｱﾄﾞﾚｽ格納用 */
	U2			size		/* ｺﾋﾟｰｻｲｽﾞ格納用 */
)
{
	U1	 	*d;
	U1		*s;
	U2	cnt;

	d = dptr;
	s = sptr;
	cnt = size;

	while ( cnt ) {
		*d++ = *s++;
		--cnt;
	}
}


/*****************************************************************************
 *
 *		関数名	：WORD	c_memcmp( )
 *
 *		機能概要：指定されたサイズ分、メモリブロックを比較する。
 *
 *		入力	：
 *				U1	*	s1ptr	in	 比較配列先頭アドレス
 *				U1	*	s2ptr	in	 比較配列先頭アドレス
 *				U2		size	in	 比較サイズ
 *
 *		出力	：なし
 *
 *		戻り値	：0 ：一致
 *				  + ：不一致
 *				  - ：不一致
 *
 *****************************************************************************/
U2 c_memcmp(
	U1			*s1ptr,		/* ｺﾋﾟｰ先ｱﾄﾞﾚｽ格納用 */
	U1			*s2ptr,		/* ｺﾋﾟｰ元ｱﾄﾞﾚｽ格納用 */
	U2			size		/* ｺﾋﾟｰｻｲｽﾞ格納用 */
)
{
	U1			*source1;
	U1			*source2;
	U2			cnt;

	source1 = s1ptr;
	source2 = s2ptr;
	cnt = size;

	while ( cnt ) {
		if ( *source1 != *source2 ) {
			return( *source1 - *source2 );
		}
		++source1;
		++source2;
		--cnt;
    }

    return(0);
}

/*****************************************************************************
 *
 *		関数名	：void	c_memset( )
 *
 *		機能概要：指定されたサイズ分のメモリに指定データを書き込む
 *
 *		入力	：
 *				U1	*	ptr		in	 転送先アドレス
 *				U1		data	in	 クリアデータ
 *				U2		size	in	 クリアサイズ
 *
 *		出力	：なし
 *
 *		戻り値	：なし
 *
 *****************************************************************************/
void
c_memset(
	U1		*ptr,		/* 転送ｱﾄﾞﾚｽ格納用 */
	U1		data,		/* ｸﾘｱﾃﾞｰﾀ格納用 */
	U2		size		/* ｸﾘｱｻｲｽﾞ格納用 */
)
{
	U1			*s;
	U2			cnt;

	s = ptr;
	cnt = size;

    while ( cnt ) {
      *s = data;
	  s++;
      --cnt;
    }
	 
    return;
}


/******************************************************************************/
/****************************** End Of File ***********************************/
/******************************************************************************/
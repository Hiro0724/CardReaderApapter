#ifndef __VERSION_H__
#define __VERSION_H__

//#define VERSION "0.1"		//2020/06/01	初版（バージョン表記できてからの初版）
//#define VERSION "0.2"		//2020/06/09	Type1に警報角度を入れた
//#define VERSION "0.21"	//2020/06/24	デバッグ用警報変更（type1:短い）
//#define VERSION "0.3"		//2020/06/26	type4追加
//#define VERSION "0.31"	//2020/07/02	警報ディレー通常
//#define VERSION "0.4"		//2020/07/08	type4不具合修正
//#define VERSION "0.41"	//2020/07/17	進行方向のvalid
//#define VERSION "0.5"		//2020/08/04	警告音声番号の追加、逆走判定に進行方向も使用する
//#define VERSION "0.51"	//2020/08/05	warning対応
//#define VERSION "0.6"		//2020/08/12	警報ディレーの仕様変更、逆走角度検知の変更
//#define VERSION "0.61"	//2020/08/17	逆走角度検知の不具合修正
//#define VERSION "0.62"	//2020/08/19	逆走角度検知の不具合修正
//#define VERSION "0.63"	//2020/08/19	type2、type4のガード値
//#define VERSION "0.64"	//2020/08/20	type2「逆走中」のディレータイマ１５秒、逆走検知角度を９０以上に変更
//#define VERSION "0.65"	//2020/08/24	アラームLEDをtype1の属性によってパルスでカウント出力するようにした
//#define VERSION "0.66"	//2020/08/26	逆走角度検知のあと、距離での逆走検知が動作していなかったのを修正
//#define VERSION "0.67"	//2020/09/02	距離での逆走検知に不具合があったのを修正(type2、type4)
//#define VERSION "0.671"	//2020/09/02	逆走の角度検知角を45°に（結果・・・良好だが、進入による車線変更で検知してしまわないか？）
//#define VERSION "0.672"	//2020/09/04	type4で逆走中になったときは、type1の検知（警告）はさせない
//#define VERSION "0.673"	//2020/09/09	type2の逆走角度検知、距離検知の処理見直し（併用処理すると無理がでてきたため）
//#define VERSION "0.674"	//2020/09/09	一度、type2の枠内を検知したら、枠内で停止しても、再度動き出せば角度検知は動作させる
//#define VERSION "0.68"	//2020/09/10	100B2002.bin ９月１０日版
//#define VERSION "0.681"	//2020/09/14	有効データ（停車判断）の速度のスレッシュを1km/hにした
//#define VERSION "0.69"	//2020/09/15	100B2002.bin ９月１５日版
//#define VERSION "0.70"	//2020/09/18	100B2002.bin ９月１８日版
//#define VERSION "0.71"	//2020/09/24	100B2002.bin ９月２４日版。距離検知の不具合を修正。type2の警告をディレーなしに鳴らすようにテスト用で変更
//#define VERSION "0.72"		//2020/09/30	100B2002.bin ９月３０日版。

#define REVRUN_DEBUG	1	//0:type1警報長さ３ｓ通常 1:type1警報長さ１０ｓ
#if REVRUN_DEBUG==0
#define VERSION "0.80"		//2020/10/01	警告音長さ３ｓ（通常）、type2の枠内ディレーなし、type4は警告音はなし、100B2002.bin １０月２日版
#else
#define VERSION "0.81"		//2020/10/01	警告音長さ１０ｓ（確認用）、type2の枠内ディレーあり、type4は警告音はなし、100B2002.bin １０月２日版
#endif

#endif

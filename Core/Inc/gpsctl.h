#ifndef __GPSCTL_H__
#define __GPSCTL_H__

#define POI_DATASPEC    (0xB)

// POIヘッダー
typedef struct
{
    U4    u4_headCode;                      // ヘッダコード
    U4    u4_staAdr;                        // 開始アドレス
    U4    u4_endAdr;                        // 終了アドレス
    U4    u4_allChksum;                     // 全データチェックサム

    union
    {
        struct
        {
            U2    b_orbisDataArea   :1;     // オービスデータエリア
            U2    u2_orbisDataSpec  :15;    // オービスデータスペック
        }orbis;
        U2 u2_mapMainVer;
    }mainVer;
    U2    u2_subVer;                        // サブバージョン

    U1    u1_hour;                          // 時
    U1    u1_day;                           // 日
    U1    u1_month;                         // 月
    U1    u1_year;                          // 年

    U1    u1_min;                           // 分
    U1    u1_sec;                           // 秒
    U1    u1_reserved[2];                   // 予約
    U4    u4_headerChksum;                  // ヘッダチェックサム
}ST_POI_HEADER;

void GpsCtl_Routine(void);
#ifdef _ON_PC_WINDOWS_
void GpsCtlTaskEntry(void const* argument);
#endif
Bool GpsCtl_GetPoiHeader( ST_POI_HEADER *p_header );
Bool GpsCtl_CheckPoiHeader( const ST_POI_HEADER *p_header );
U4  GpsCtl_CalU4Checksum( const U1 *p, int len );
void GpsCtl_SetPoiValid( Bool valid );
void GpsCtl_ReqIdle( Bool idle );
#endif

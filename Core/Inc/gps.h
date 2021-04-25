#ifndef __GPS_H__
#define __GPS_H__

#define LAT_MIN         ((uint32_t)0x00000001)
#define LAT_MAX         ((uint32_t)0x00FFFFFF)
#define LON_MIN         ((uint32_t)0x00000000)
#define LON_MAX         ((uint32_t)0x00860000)

typedef struct
{
    uint8_t     b_sokui     :1;
    uint8_t     b_height_ok :1;
    uint8_t     b_tim_ok    :1;
    uint8_t     b_deg_ok    :1;
    uint8_t     b_rsv       :4;

    uint32_t    lat;
    uint32_t    lon;
    uint16_t    spd;
    uint16_t    deg;
    int16_t     height;

    uint8_t     year;
    uint8_t     month;
    uint8_t     day;
    uint8_t     hour;
    uint8_t     min;
    uint8_t     sec;

    uint16_t    pdop;
    uint16_t    hdop;
    uint16_t    vdop;

}GpsInfo_t;

typedef	struct
{
    uint32_t    u4_carLat;
    uint32_t    u4_carLon;
    uint16_t    u2_carDeg;
    int16_t     s2_carHeight;
    uint16_t    u2_carSpd;

    uint8_t     b_curSokui      :1;
    uint8_t     b_sysSokui      :1;
    uint8_t     b_degValid      :1;
    uint8_t     b_heightValid   :1;
    uint8_t     b_timValid      :1;
    uint8_t     b_spdValid      :1;
    uint8_t     b_sys1stFix     :1;
    uint8_t                     :2;

    uint8_t     u1_latarea;
    uint16_t    u2_lonarea;

    uint8_t     u1_year;
    uint8_t     u1_month;
    uint8_t     u1_day;
    uint8_t     u1_hour;
    uint8_t     u1_min;
    uint8_t     u1_sec;
}ST_GPS_STS;


void Gps_Init( UART_HandleTypeDef *huart );
void Gps_Routine(void);
#ifdef _ON_PC_WINDOWS_
void GpsRxTaskEntry(void const* argument);
void GpsRxIntTaskEntry(void const* argument);
#endif

#endif
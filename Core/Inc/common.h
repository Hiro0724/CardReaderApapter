#ifndef __COMMON_H__
#define __COMMON_H__

#include <limits.h>
#include <stdbool.h>

typedef unsigned char   U1;
typedef unsigned short  U2;
typedef unsigned long   U4;
typedef unsigned long long U8;
typedef signed char     S1;
typedef signed short    S2;
typedef signed long     S4;
typedef signed long long S8;
typedef _Bool   Bool;


typedef	union{
	U1		byte;
	struct{
		U1	b0:1;
		U1	b1:1;
		U1	b2:1;
		U1	b3:1;
		U1	b4:1;
		U1	b5:1;
		U1	b6:1;
		U1	b7:1;
	}bit;
	struct{
		U1	lo:4;
		U1	hi:4;
	}nbl;
}F1;

typedef	union{
	U2	word;
	struct{
		U1	lo;
		U1	hi;
	}byte;
	struct{
		U2	b00:1;
		U2	b01:1;
		U2	b02:1;
		U2	b03:1;
		U2	b04:1;
		U2	b05:1;
		U2	b06:1;
		U2	b07:1;

		U2	b08:1;
		U2	b09:1;
		U2	b10:1;
		U2	b11:1;
		U2	b12:1;
		U2	b13:1;
		U2	b14:1;
		U2	b15:1;
	}bit;
}F2;

// 4byte ãƒ“ãƒƒãƒˆãƒ•ã‚£ãƒ¼ãƒ«ãƒ‰å…±ç”¨ä½“
typedef union{
    U4  dword;
    U4  word;
    struct{
        U2    lo;
        U2    hi;
    }hword;
    U1  byte[4];
    struct{
        U4  b00:1;
        U4  b01:1;
        U4  b02:1;
        U4  b03:1;
        U4  b04:1;
        U4  b05:1;
        U4  b06:1;
        U4  b07:1;

        U4  b08:1;
        U4  b09:1;
        U4  b10:1;
        U4  b11:1;
        U4  b12:1;
        U4  b13:1;
        U4  b14:1;
        U4  b15:1;

        U4  b16:1;
        U4  b17:1;
        U4  b18:1;
        U4  b19:1;
        U4  b20:1;
        U4  b21:1;
        U4  b22:1;
        U4  b23:1;

        U4  b24:1;
        U4  b25:1;
        U4  b26:1;
        U4  b27:1;
        U4  b28:1;
        U4  b29:1;
        U4  b30:1;
        U4  b31:1;
    }bit;
}F4;

#define U1_MAX  UCHAR_MAX
#define U2_MAX  USHRT_MAX
#define U4_MAX  ULONG_MAX


/* bit’è‹` */
#define	BIT0	((U1)0x01)
#define	BIT1	((U1)0x02)
#define	BIT2	((U1)0x04)
#define	BIT3	((U1)0x08)
#define	BIT4	((U1)0x10)
#define	BIT5	((U1)0x20)
#define	BIT6	((U1)0x40)
#define	BIT7	((U1)0x80)

/* ‹¤’Ê’è‹` */

#define	OFF		0
#define	ON		1

#define	FALSE	0
#define	TRUE	1

#define	LO		0
#define	HI		1

#define	NG		0
#define	OK		1
#define ERR     2

#define	NO		0
#define	YES		1

#define	UP		0
#define	DOWN	1

#define	IN		0
#define	OUT		1

//#define	DISABLE	0
//#define	ENABLE	1

#define	SMALL	0
#define	LARGE	1

#define	UBYTE_MAX	((U1)0xFF)
#define	UWORD_MAX	((U2)0xFFFF)
#define	DWORD_MAX	((U4)0xFFFFFFFF)

#define	S1_MAX	((S1)127)
#define	S1_MIN	((S1)-128)
#define	S2_MAX	((S2)32767)
#define	S2_MIN	((S2)-32768)
#define	S4_MAX	((S4)2147483647)
#define	S4_MIN	((S4)-2147483648)

#define	NULL	0



#define NELEMS(array)   (sizeof(array)/sizeof(array[0]))
#define DECCNT(cnt)     (((cnt) != 0) ? --(cnt) : (cnt))
#define ABS_SUB(a,b)    ((a >= b) ? (a-b) : (b-a))

// msã‚ªãƒ¼ãƒ€å¾…ã¡
#define WAIT_LOOP_MS(MS)	{		\
	U4	i;							\
	for(i=MS*1600; i>0; i--){}	\
}


/****************************************************************************
 *  ŠO•”QÆ‹¤’ÊŠÖ”ƒvƒƒgƒ^ƒCƒvéŒ¾										*
 ****************************************************************************/
extern	void	c_memcpy( U1 *, U1 *, U2 );
extern	U2		c_memcmp( U1 *, U1 *, U2 );
extern	void	c_memset( U1 *, U1, U2 );



#endif

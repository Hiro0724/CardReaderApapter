/******************************************************************************
 *
 *	�J���@��			�F�Ƌ��؃��[�_����p�A�_�v�^
 *	Micro-processingUnit�FSTM32G071GBU
 *-----------------------------------------------------------------------------
 *	�@�\����	�FUART1 �ʐM�����@
 *	�t�@�C����	�FuartCardReader.c
 *	�t�@�C�������F�Ƌ��؁i�J�[�h�j���[�_�Ƃ̒ʐM����
 *				�F38400bps, 8bit, start 1bit, stop 1bit, non parity
 *
 *	�쐬��		�F2021�N X�� X��
 *	�쐬��		�FHayashi
 *
 *			Copyright(C)	2021 YUPITERU Ltd. All rights reserved.
 *=============================================================================
 *
 *-----------------------------------------------------------------------------
 *�y revision history �z
 *
 *	Version	�F
 *	Update	�F
 *	summary �F
 *	author	�F
 *
 ******************************************************************************/


/********************************************************************/
/*  �C���N���[�h�t�@�C��                                            */
/********************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include <string.h>
#include "stdlib.h"
#include "cardreader.h"
#include "cmsis_os.h"


/********************************************************************/
/*  �O���Q�Ɛ錾	                                                */
/********************************************************************/
//extern void xxxx(void);
//extern	uint8_t	;
extern	void MX_USART1_UART_Init(void);
extern osMailQId CrSerialTxMailHandle;
extern osMailQId CrSerialRxMailHandle;
extern osMailQId MainControlMailHandle;
extern osThreadId CrSerialTxHandle;

extern void init_sq_up( void );

/********************************************************************/
/*  �v���g�^�C�v�錾	                                            */
/********************************************************************/
//void	(void);
//static void(void);
//static void (void);
//static void (void);
//static void (void);


/********************************************************************/
/*	�����萔��`													*/
/********************************************************************/

/* �رّ��M��Ԓ�` */
enum{
	TX_STOP=0,								/* �رّ��M��~			*/
	TX_DATA,								/* �ް������M��			*/
	TX_CMPWAIT								/* ���M�����҂�			*/
};


/* �رْʐM�����Ԓ�` */
enum{
	NOCONN_STATUS = 0,						/* �ʐM�Ȃ�				*/
	CONNECTED_STATUS						/* �ʐM�ڑ������		*/
};

/*------------------------------------------*/
/*	���M�����Ԓ�`						*/
/*------------------------------------------*/
enum{
	TXST_WAIT=0,							/* ���M�v���҂����		*/
	TXST_SENDING							/* ���M�����			*/

};

#define		CR_RXBUFMAX	(CRD_RCV_BUF_SIZE - CRS_HAEDER )	/* ��M�M�ޯ̧Max�l		*/

#define	LEN_CR_SERIAL	5
#define GET_ADR	 (0x00)	// Fix
#define GET_DSTADR(adr) ((adr) & 0x0F)

#define ADR_ANY             (0x0)
#define ADR_SELF            (0xC)
#define CR_SERIAL_LEN_MAX   (CR_SERIAL_BUFF_MAX - 5)
#define NO_SCR      (0x7A)
#define IDX_STX     (0)
#define IDX_ADR     (1)
#define IDX_CMD     (2)
#define IDX_LEN     (3)
#define IDX_DAT     (4)

#define LABL_DAT_STX            (0x02)
#define LABL_DAT_ETX            (0x03)
#define LABL_DAT_CR             (0x0D)



enum {
	ADD_CR_SERIAL_STX = 0,	
	ADD_CR_SERIAL_ADD,	
	ADD_CR_SERIAL_COMMAND,	
	ADD_CR_SERIAL_LEN,	
	ADD_CR_SERIAL_DATA,	
	ADD_CR_SERIAL_ETX,	
	ADD_CR_SERIAL_SUM,	
	ADD_CR_SERIAL_CR,
};

/* �رَ�M��Ԓ�` */
typedef enum
{
	RX_STOP=0,								/* �رَ�M��~			*/
	RX_ACTVE,								/* �رَ�M�҂���		*/
	RX_RCVHEAD,								/* ͯ�ް����M��		*/
	RX_RCVDATA,								/* �ް�����M��			*/
	RX_RCVERR,								/* ��M�װ����			*/
}CrSerialRxStatus_e;

typedef struct
{
    U1 rxtmr;
    U1 byte_buff;
    CrSerialRxStatus_e  stat;
    int cnt;
    int len;
    U1  buff[CR_SERIAL_BUFF_MAX];
}CrSerialRx_t;


typedef enum
{
    CMD_CARDREADERINVENTORY 	= 0x71,
    CMD_CARDREADERCHGCOMMOD  	= 0x70,
    CMD_CARDREADERSETMODE 		= 0x4E,
    CMD_CARDREADERREQMODE		= 0x4F,
    CMD_CARDREADERMULTIPOLLING 	= 0x81,
}CRSerialCmd_e;



/********************************************************************/
/*	�}�N����`														*/
/********************************************************************/



/********************************************************************/
/*  �����ϐ���`                                                    */
/********************************************************************/
static CRS_SERIAL cr_serial;				/* �رْʐM��������������		*/

static CrSerialRx_t cr_rx;					/* �رْʐM���̎�M������		*/

static CrSerialBuff_t	cr_tx;				/* �رْʐM���̑��M������		*/

static  uint8_t buffer;


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;



/********************************************************************/
/*  �v���g�^�C�v��`                                                */
/********************************************************************/

void	cr_SerialInit( void );

U1	CR_SerialOpen(	U1 );
U1	cr_SerialStop( void );

static Bool parse_cr_serial( const U1 *p_buff, int len );
static void cr_ResetRxError( void );


/*****************************************************************************
 *
 *	�֐����́Fvoid CR_Init( void )
 *	�����T�v�FCR�رُ�����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
void CR_Init( void )
{

	/* �رْʐM��������������ر */
	c_memset( (U1 *)&cr_serial, 0x00, sizeof( CRS_SERIAL ) );

	/* �رْʐM���̎�M������ر */
	c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );

	/* �رْʐM���̑��M������ر */
	c_memset( (U1 *)&cr_tx, 0x00, sizeof( CrSerialBuff_t ) );


	CR_SerialOpen( YPS_MODE_NORMAL );

}



/*****************************************************************************
 *
 *	�֐����́FU1 CR_SerialOpen( U1 )
 *	�����T�v�FCR�رق���݂���
 *	����	�F�Ȃ�
 *	�߂�l	�FOK--.����
 *			  NG--.�ُ�
 *
 *****************************************************************************/
U1 CR_SerialOpen(U1	mode)
{
	U1	ret = OK;		/* �֐��̖߂�l�i�[�p */


	/* ���ɋN���ς݂��H */
	if ( cr_serial.stat != NOCONN_STATUS ) {
		return( NG );
	}

	/* �رْʐMӰ�ގw�� */
	switch ( mode ) {

	case YPS_MODE_NORMAL:				/* �������MӰ��		*/
		cr_serial.stat = CONNECTED_STATUS;
		/* CR�ʐM���������{ */
		cr_SerialInit();

	default:							/* ���̑�			*/
		ret = NG;
		break;
	}

	return( ret );
}


/*****************************************************************************
 *
 *	�֐����́Fvoid cr_SerialInit( void )
 *	�����T�v�FCR�رق���݂���
 *	����	�F�Ȃ�
 *	�߂�l	�F
 *
 *****************************************************************************/
void cr_SerialInit( void )
{

//	MX_USART1_UART_Init();

	/* �رَ�M����J�n */
	cr_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart1, &buffer, 1);		// ��M�J�nUART1(TO �J�[�h���[�_�j

}


/*****************************************************************************
 *
 *	�֐����́FU1 cr_SerialStop( void )
 *	�����T�v�FCR�رق�۰�ނ���
 *	����	�F�Ȃ�
 *	�߂�l	�F
 *
 *****************************************************************************/
U1 cr_SerialStop( void )
{
	U1	ret = OK;

	cr_serial.stat = NOCONN_STATUS;

	return( ret );
}



/*****************************************************************************
 *
 *	�֐����́Fvoid	UART1_RxCpltCallback(void)
 *	�����T�v�FUART1��M���荞�ݏ���
 *	����	�F�Ȃ�
 *	�߂�l	�F
 *
 *****************************************************************************/
void	UART1_RxCpltCallback(void)
{

	uint8_t	rxdat;


	rxdat = buffer;										// ��M�f�[�^���o

	// �f�[�^��M��Ԃɂ�菈��
	switch ( cr_rx.stat ) {
	case RX_ACTVE:										//��M�܂�
		// �����M�H
		if ( cr_rx.cnt == 0 ) {
			if (rxdat == LABL_DAT_STX)					// (0x02) 
			{
				cr_rx.stat = RX_RCVHEAD;				// ��M�J�n
				cr_rx.buff[cr_rx.cnt] = rxdat;
				cr_rx.cnt++;
				cr_rx.rxtmr = 0;
				cr_rx.len = 0;
			}
		} else {
			cr_rx.cnt = 0;
			cr_rx.len = 0;
		}
		
		break;

	case RX_RCVHEAD:									/* �f�[�^����M�҂�		*/
		cr_rx.buff[cr_rx.cnt] = rxdat;
		if (cr_rx.cnt == ADD_CR_SERIAL_LEN){
			cr_rx.len = rxdat;

			if ( cr_rx.len > CR_RXBUFMAX ) {
				/* ��M�G���[ */
				cr_rx.stat = RX_ACTVE;
				cr_rx.len = 0;
				cr_rx.cnt = 0;
			} else {
				cr_rx.len = cr_rx.len + ( CRS_HAEDER );	// �S�o�C�g��(�f�[�^�{���̑��j
				cr_rx.stat = RX_RCVDATA;
				cr_rx.cnt++;
				cr_rx.rxtmr = 0;
			}
		}else{
			cr_rx.cnt++;
		}
		break;

	case RX_RCVDATA:									/* �f�[�^����M			*/
		cr_rx.buff[cr_rx.cnt] = rxdat;
		cr_rx.cnt++;
		cr_rx.rxtmr = 0;
		/* ��M�f�[�^�� */
		if ( cr_rx.cnt >= cr_rx.len ) {

			/* ��M�m��f�[�^�o�b�t�@���o�� */

            CrSerialBuff_t *p_mail = osMailAlloc(CrSerialRxMailHandle, osWaitForever);
            if( p_mail )
            {
                p_mail->len = cr_rx.len;
                memcpy(p_mail->buff, cr_rx.buff, p_mail->len);
                osMailPut(CrSerialRxMailHandle, p_mail);
            }

			//��M�o�b�t�@�N���A
			c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );
			cr_rx.stat = RX_ACTVE;
			cr_rx.len = 0;
			cr_rx.cnt = 0;

		}
		break;

	case RX_RCVERR:
	default:
		cr_rx.stat = RX_RCVERR;
		break;
	}

	HAL_UART_Receive_IT(&huart1, &buffer, 1);			//	��M�����荞�݋���

}



/*------------------------------------------------------------------*/
/*																	*/
/*		UART1��M�G���[����											*/
/*																	*/
/*------------------------------------------------------------------*/
void	UART1_ErrorCallback(void)
{

	cr_ResetRxError( );

}


/*****************************************************************************
 *
 *	�֐����́FU1 cr_CalcCheckSum( U1 len ,U1 *buf)
 *	�����T�v�F�����Ѻ��ނ��Z�o����
 *	����	�F�f�[�^���@�f�[�^�擪�A�h���X
 *	�߂�l	�F
 *
 *****************************************************************************/
static U1 cr_serial_checksum( const U1 *p_frame )
{
    U1 sum = 0;
    U1 len = p_frame[ADD_CR_SERIAL_LEN] + 5;

    for( int i=0 ; i<len ; i++ )
    {
        sum += p_frame[i];
    }

    return sum;
}

//--------------------------------------------------------------------------------
static int idx_sum( const U1 *p_frame )
{
    return 5 + p_frame[IDX_LEN];
}

//--------------------------------------------------------------------------------
static int idx_etx( const U1 *p_frame )
{
    return 4 + p_frame[IDX_LEN];
}

//--------------------------------------------------------------------------------
static int idx_cr( const U1 *p_frame )
{
    return 6 + p_frame[IDX_LEN];
}

//--------------------------------------------------------------------------------
static int idx_cal( const U1 *p_frame , U1 adj)
{
    return adj + p_frame[IDX_LEN];
}





/*****************************************************************************
 *
 *	�֐����́Fvoid cr_ResetRxError( void )
 *	�����T�v�F��M�G���[���������ɁA��M�@�\�����Z�b�g����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
static void cr_ResetRxError( void )
{

	// ��M�o�b�t�@�N���A
	c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );

	cr_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart1, &buffer, 1);		// ��M�J�nUART2(TO �O���ڑ��@��iYP�t�H�[�}�b�g�j

	return;
}

/*****************************************************************************
 *
 *	�֐����́Fvoid cr_ClearRxError( void )
 *	�����T�v�F��M�f�[�^�Ԋu�Ń^�C���A�E�g�������������̎�M�N���A
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
static void cr_ClearRxError( void )
{

	/* ��M�����~ */
    __disable_irq();

	// ��M�o�b�t�@�N���A
	c_memset( (U1 *)&cr_rx, 0x00, sizeof( CrSerialRx_t ) );

	cr_rx.stat = RX_ACTVE;

	/* ��M����J�n */
    __enable_irq();

	HAL_UART_Receive_IT(&huart1, &buffer, 1);		// ��M�J�nUART1(TO �J�[�h���[�_�[�j

}

/********************************************************************/
/*																	*/
/*					 �V���A����Ԑ���֐� 							*/
/*																	*/
/********************************************************************/

/*****************************************************************************
 *
 *	�֐����́Fvoid CR_SerialRxCheak( void )
 *	�����T�v�FCR�ر�RX���菈���@�i5ms���������j
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/

void CR_SerialRxCheak( void )
{

	/* ��M����Ă����邩�H */
	switch ( cr_rx.stat ) {
	case RX_STOP:			/* ��M�����~			*/
	case RX_ACTVE:			/* �ް���M�҂�			*/
		break;

	case RX_RCVERR:			/* ��M�װ���m			*/

		/* ��Mؾ�� */
		cr_ClearRxError();
		break;

	default:					/* ���̑�				*/
		cr_rx.rxtmr++;
		if ( cr_rx.rxtmr > YPS_RXWTHTM ) {

			/* ��Mؾ�� */
			cr_ClearRxError();

		}
		break;
	}
}


/*****************************************************************************
 *
 *	�����T�v�FCR�رَ�M�^�X�N����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
/* USER CODE Header_CrSerialRxTaskEntry */

void CrSerialRxTaskEntry(void const * argument)
{

  /* USER CODE BEGIN YpSerialRxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
		osEvent event = osMailGet(CrSerialRxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			CrSerialBuff_t *p_Rxmail = (CrSerialBuff_t *)event.value.p;
			if (p_Rxmail)
			{
				if( p_Rxmail->len )
				{

					//��M�f�[�^���
                    parse_cr_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len );

               	}
               	osMailFree(CrSerialRxMailHandle, p_Rxmail);
           	}
		}
	    osDelay(1);
	}
}

/*****************************************************************************
 *
 *	�����T�v�FCR�رّ��M�^�X�N����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
/* USER CODE Header_CRSerialTxTaskEntry */

void CrSerialTxTaskEntry(void const * argument)
{
  for(;;)
  {
		osEvent event = osMailGet(CrSerialTxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			CrSerialBuff_t *p_mail = (CrSerialBuff_t *)event.value.p;
			if (p_mail)
			{
				if( p_mail->len )
				{
                	cr_tx.len = p_mail->len;
                	memcpy(cr_tx.buff, p_mail->buff, cr_tx.len);

               		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)cr_tx.buff, cr_tx.len);
               	}
               	osMailFree(CrSerialTxMailHandle, p_mail);
           	}
		}
		//waiting for tx complete signal (or timeout)
		osSignalWait(SIGNAL_CR_TXCOMP, cr_tx.len*100);

		osDelay(1);
  }
}

/*****************************************************************************
 *
 *	�����T�v�FCR�رّ��M����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
void UART1_TxCpltCallback( void )
{
    osSignalSet(CrSerialTxHandle, SIGNAL_CR_TXCOMP);
}



/*****************************************************************************
 *
 *	�����T�v�FCR�رّ��M����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
//--------------------------------------------------------------------------------
static void tx_cr_serial( U1 dst_adr, CRSerialCmd_e cmd, const U1 *p_data, int data_len )
{
    CrSerialBuff_t *p_yps = osMailAlloc( CrSerialTxMailHandle, osWaitForever );
    
    if( p_yps )
    {
        memset( p_yps, 0, sizeof(*p_yps) );

        p_yps->buff[IDX_STX] = LABL_DAT_STX;					
        p_yps->buff[IDX_ADR] = dst_adr;
        p_yps->buff[IDX_LEN] = (U1)data_len;
        p_yps->buff[IDX_CMD] = (U1)cmd;
        if( p_data )
        {
            memcpy(&p_yps->buff[IDX_DAT], p_data, data_len );
        }

		/* ���M���������т��v�Z���đ��M�ް��Ƃ��ľ�Ă���*/
		/* �����юZ�o */
 
      	p_yps->buff[4 + (p_yps->buff[IDX_LEN])] = LABL_DAT_ETX;	// (0x03)
 	    p_yps->buff[5 + (p_yps->buff[IDX_LEN])] = cr_serial_checksum( p_yps->buff );
      	p_yps->buff[6 + (p_yps->buff[IDX_LEN])] = LABL_DAT_CR;	// (0x0D)

        p_yps->len = 7 + p_yps->buff[IDX_LEN];					//���M�o�C�g���Z�b�g

        osMailPut(CrSerialTxMailHandle, p_yps);

    }
}


//--------------------------------------------------------------------------------
//
//		��M�f�[�^���
//
//--------------------------------------------------------------------------------
static Bool parse_cr_serial( const U1 *p_buff, int len )
{

	// CHECKSUM����
    if( cr_serial_checksum(p_buff) != p_buff[idx_sum(p_buff)] )
    {
        return false;
    }

	// �A�h���X�O����
    if( p_buff[IDX_ADR] != 0x00 )
    {
        return false;
    }

	// ���C������փf�[�^���M   
   	MainControlBuff_t *p_mail = osMailAlloc(MainControlMailHandle, osWaitForever);
   	if( p_mail )
   	{
       	p_mail->status = RX_CRSERIAL;
       	p_mail->len = len;
       	memcpy(p_mail->buff, p_buff, len);
       	osMailPut(MainControlMailHandle, p_mail);
   	}

    return true;
}


//---------------------------------------
//
//			���M�v��
//
//---------------------------------------
void	req_tx_cr_serial( U1 src_adr, U1 command, const U1 *p_data, U1 len )
{

	tx_cr_serial( GET_ADR, command, &p_data[0], len );

}



/* USER CODE END 0 */
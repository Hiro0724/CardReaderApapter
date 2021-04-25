/******************************************************************************
 *
 *	�J���@��			�F�Ƌ��؃��[�_����p�A�_�v�^
 *	Micro-processingUnit�FSTM32G071GBU
 *-----------------------------------------------------------------------------
 *	�@�\����	�FUART2 �ʐM�����@
 *	�t�@�C����	�Fuart_ypserial.c
 *	�t�@�C�������F�O���ڑ��@��Ƃ�YYP�t�H�[�}�b�g�ʐM����
 *				�F�I�v�V�����A�_�v�^�o�R�\��
 *				�F  9600bps, 8bit, start 1bit, stop 2bit, non parity
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
extern	void MX_USART2_UART_Init(void);
extern osMailQId YpSerialTxMailHandle;
extern osMailQId YpSerialRxMailHandle;
extern osMailQId MainControlMailHandle;
extern osThreadId YpSerialTxHandle;

extern void req_status( U1 *p_status, U1 *p_kind, U1 *p_err );

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

#define		YP_RXBUFMAX	(YPS_RCV_BUF_SIZE - YPS_HAEDER )	/* ��M�M�ޯ̧Max�l		*/

#define	LEN_YP_SERIAL	5
#define GET_SRCADR(adr) (((adr) & 0xF0)>>4)
#define GET_DSTADR(adr) ((adr) & 0x0F)

#define ADR_ANY             (0x0)
#define ADR_SELF            (0xC)
#define YP_SERIAL_LEN_MAX   (YP_SERIAL_BUFF_MAX - 5)
#define NO_SCR      (0x7A)
#define IDX_ADR     (0)
#define IDX_LEN     (1)
#define IDX_SCR     (2)
#define IDX_CMD     (3)
#define IDX_DAT     (4)

enum {
	ADD_YP_SERIAL_ADD = 0,	
	ADD_YP_SERIAL_LEN,	
	ADD_YP_SERIAL_SCRAMBLE,	
	ADD_YP_SERIAL_COMMAND,	
	ADD_YP_SERIAL_DATA,	
	ADD_YP_SERIAL_CHKSUM,
};

/* �رَ�M��Ԓ�` */
typedef enum
{
	RX_STOP=0,								/* �رَ�M��~			*/
	RX_ACTVE,								/* �رَ�M�҂���		*/
	RX_RCVHEAD,								/* ͯ�ް����M��		*/
	RX_RCVDATA,								/* �ް�����M��			*/
	RX_RCVERR,								/* ��M�װ����			*/
}YpSerialRxStatus_e;

typedef struct
{
    U1 rxtmr;
    U1 byte_buff;
    YpSerialRxStatus_e  stat;
    int cnt;
    int len;
    U1  buff[YP_SERIAL_BUFF_MAX];
}YpSerialRx_t;


typedef enum
{
    ReqCmd_YpSerialStatus = 0xA0,
    ReqCmd_YpSerialReset  = 0xA1,
    ReqCmd_YpSerialReqKind = 0xA2,
    ReqCmd_YpSerialReserve1 = 0xA3,
    ReqCmd_YpSerialReserve2 = 0xA4,
}YpSerialReqCmd_e;


typedef enum
{
    Cmd_YpSerialStatus = 0xA0,
    Cmd_YpSerialReset  = 0xA1,
    Cmd_YpSerialReqKind = 0xA2,
    Cmd_YpSerialReserve1 = 0xA3,
    Cmd_YpSerialReserve2 = 0xA4,
}YpSerialCmd_e;








/********************************************************************/
/*	�}�N����`														*/
/********************************************************************/



/********************************************************************/
/*  �����ϐ���`                                                    */
/********************************************************************/
static YPS_SERIAL yp_serial;				/* �رْʐM��������������		*/

static YpSerialRx_t yp_rx;					/* �رْʐM���̎�M������		*/

static YpSerialBuff_t	yp_tx;				/* �رْʐM���̑��M������		*/

static  uint8_t buffer;


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
IWDG_HandleTypeDef hiwdg;



/********************************************************************/
/*  �v���g�^�C�v��`                                                */
/********************************************************************/

void	yp_SerialInit( void );

U1	YP_SerialOpen(	U1 );
U1	yp_SerialStop( void );

static Bool parse_yp_serial( const U1 *p_buff, int len );
static void yp_ResetRxError( void );

extern void	 req_chgkind( U1 *p);

/*****************************************************************************
 *
 *	�֐����́Fvoid YP_SerialInit( void )
 *	�����T�v�FYP�رُ�����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
void YP_Init( void )
{

	/* �رْʐM��������������ر */
	c_memset( (U1 *)&yp_serial, 0x00, sizeof( YPS_SERIAL ) );

	/* �رْʐM���̎�M������ر */
	c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );

	/* �رْʐM���̑��M������ر */
	c_memset( (U1 *)&yp_tx, 0x00, sizeof( YpSerialBuff_t ) );


	YP_SerialOpen( YPS_MODE_NORMAL );

}



/*****************************************************************************
 *
 *	�֐����́FU1 YP_SerialOpen( U1 )
 *	�����T�v�FYP�رق���݂���
 *	����	�F�Ȃ�
 *	�߂�l	�FOK--.����
 *			  NG--.�ُ�
 *
 *****************************************************************************/
U1 YP_SerialOpen(U1	mode)
{
	U1	ret = OK;		/* �֐��̖߂�l�i�[�p */


	/* ���ɋN���ς݂��H */
	if ( yp_serial.stat != NOCONN_STATUS ) {
		return( NG );
	}

	/* �رْʐMӰ�ގw�� */
	switch ( mode ) {

	case YPS_MODE_NORMAL:				/* �������MӰ��		*/
		yp_serial.stat = CONNECTED_STATUS;
		/* YP�ʐM���������{ */
		yp_SerialInit();

	default:							/* ���̑�			*/
		ret = NG;
		break;
	}

	return( ret );
}


/*****************************************************************************
 *
 *	�֐����́Fvoid yp_SerialInit( void )
 *	�����T�v�FYP�رق���݂���
 *	����	�F�Ȃ�
 *	�߂�l	�F
 *
 *****************************************************************************/
void yp_SerialInit( void )
{

//	MX_USART2_UART_Init();

	/* �رَ�M����J�n */
	yp_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart2, &buffer, 1);		// ��M�J�nUART2(TO �O���ڑ��@��iYP�t�H�[�}�b�g�j

}


/*****************************************************************************
 *
 *	�֐����́FU1 yp_SerialStop( void )
 *	�����T�v�FYP�رق�۰�ނ���
 *	����	�F�Ȃ�
 *	�߂�l	�F
 *
 *****************************************************************************/
U1 yp_SerialStop( void )
{
	U1	ret = OK;

	yp_serial.stat = NOCONN_STATUS;

	return( ret );
}



/*****************************************************************************
 *
 *	�֐����́Fvoid	UART2_RxCpltCallback(void)
 *	�����T�v�FUART2��M���荞�ݏ���
 *	����	�F�Ȃ�
 *	�߂�l	�F
 *
 *****************************************************************************/
void	UART2_RxCpltCallback(void)
{

	uint8_t	rxdat;
	uint8_t	data;

	rxdat = buffer;										// ��M�f�[�^���o

	// �f�[�^��M��Ԃɂ�菈��
	switch ( yp_rx.stat ) {
	case RX_ACTVE:										//��M�܂�
		// �����M�H
		if ( yp_rx.cnt == 0 ) {
			data = rxdat & 0xF0;
			data = data >> 4;
			if ( ( data != 0 ) && ( data <= 7 ) ) {
				yp_rx.stat = RX_RCVHEAD;				//���M������
				yp_rx.buff[yp_rx.cnt] = rxdat;
				yp_rx.cnt++;
				yp_rx.rxtmr = 0;
			}
		} else {
			yp_rx.cnt = 0;
		}
		
		break;

	case RX_RCVHEAD:									/* �w�b�_��M		*/
		yp_rx.buff[yp_rx.cnt] = rxdat;
		yp_rx.len = rxdat;
		if ( yp_rx.len > YP_RXBUFMAX ) {
			/* ��M�G���[ */
			yp_rx.stat = RX_ACTVE;
			yp_rx.len = 0;
			yp_rx.cnt = 0;
		} else {
			yp_rx.len = yp_rx.len + ( YPS_HAEDER );
			yp_rx.stat = RX_RCVDATA;
			yp_rx.cnt++;
			yp_rx.rxtmr = 0;
		}
		break;

	case RX_RCVDATA:									/* �f�[�^����M			*/
		yp_rx.buff[yp_rx.cnt] = rxdat;
		yp_rx.cnt++;
		yp_rx.rxtmr = 0;
		/* ��M�f�[�^�� */
		if ( yp_rx.cnt >= yp_rx.len ) {

			/* ��M�m��f�[�^�o�b�t�@���o�� */

            YpSerialBuff_t *p_mail = osMailAlloc(YpSerialRxMailHandle, osWaitForever);
            if( p_mail )
            {
                p_mail->len = yp_rx.len;
                memcpy(p_mail->buff, yp_rx.buff, p_mail->len);
                osMailPut(YpSerialRxMailHandle, p_mail);
            }

			//��M�o�b�t�@�N���A
			c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );
			yp_rx.stat = RX_ACTVE;
			yp_rx.len = 0;
			yp_rx.cnt = 0;

		}
		break;

	case RX_RCVERR:
	default:
		yp_rx.stat = RX_RCVERR;
		break;
	}

	HAL_UART_Receive_IT(&huart2, &buffer, 1);			//	��M�����荞�݋���

}



/*------------------------------------------------------------------*/
/*																	*/
/*		UART2��M�G���[����											*/
/*																	*/
/*------------------------------------------------------------------*/
void	UART2_ErrorCallback(void)
{

	yp_ResetRxError( );

}


/*****************************************************************************
 *
 *	�֐����́FU1 yp_CalcCheckSum( U1 len ,U1 *buf)
 *	�����T�v�F�����Ѻ��ނ��Z�o����
 *	����	�F�f�[�^���@�f�[�^�擪�A�h���X
 *	�߂�l	�F
 *
 *****************************************************************************/
static U1 yp_CalcCheckSum(	
	U1		len,			/* �ް����i�[�p			*/
	U1		*buf			/* �ް���̐擪���ڽ	*/
)
{
	U1		sum = 0;
	U1		cnt;


	/* �����юZ�o */
	for ( cnt = 0 ; cnt < len ; cnt++ ) {
		sum = sum + buf[cnt];
	}

	return( sum );
}

//--------------------------------------------------------------------------------
static U1 yp_serial_checksum( const U1 *p_frame )
{
    U1 sum = 0;
    U1 len = p_frame[1] + 4;

    for( int i=0 ; i<len ; i++ )
    {
        sum += p_frame[i];
    }

    return sum;
}

//--------------------------------------------------------------------------------
static int idx_sum( const U1 *p_frame )
{
    return 4 + p_frame[IDX_LEN];
}



/*****************************************************************************
 *
 *	�֐����́Fvoid yp_ResetRxError( void )
 *	�����T�v�F��M�G���[���������ɁA��M�@�\�����Z�b�g����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
static void yp_ResetRxError( void )
{

	// ��M�o�b�t�@�N���A
	c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );

	yp_rx.stat = RX_ACTVE;

	HAL_UART_Receive_IT(&huart2, &buffer, 1);		// ��M�J�nUART2(TO �O���ڑ��@��iYP�t�H�[�}�b�g�j

	return;
}

/*****************************************************************************
 *
 *	�֐����́Fvoid yp_ClearRxError( void )
 *	�����T�v�F��M�f�[�^�Ԋu�Ń^�C���A�E�g�������������̎�M�N���A
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
static void yp_ClearRxError( void )
{

	/* ��M�����~ */
    __disable_irq();

	// ��M�o�b�t�@�N���A
	c_memset( (U1 *)&yp_rx, 0x00, sizeof( YpSerialRx_t ) );

	yp_rx.stat = RX_ACTVE;

	/* ��M����J�n */
    __enable_irq();

	HAL_UART_Receive_IT(&huart2, &buffer, 1);		// ��M�J�nUART2(TO �O���ڑ��@��iYP�t�H�[�}�b�g�j

}

/********************************************************************/
/*																	*/
/*					 �V���A����Ԑ���֐� 							*/
/*																	*/
/********************************************************************/

/*****************************************************************************
 *
 *	�֐����́Fvoid YP_SerialRxCheak( void )
 *	�����T�v�FYP�ر�RX���菈���@�i5ms���������j
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/

void YP_SerialRxCheak( void )
{

	/* ��M����Ă����邩�H */
	switch ( yp_rx.stat ) {
	case RX_STOP:			/* ��M�����~			*/
	case RX_ACTVE:			/* �ް���M�҂�			*/
		break;

	case RX_RCVERR:			/* ��M�װ���m			*/

		/* ��Mؾ�� */
		yp_ClearRxError();
		break;

	default:					/* ���̑�				*/
		yp_rx.rxtmr++;
		if ( yp_rx.rxtmr > YPS_RXWTHTM ) {

			/* ��Mؾ�� */
			yp_ClearRxError();

		}
		break;
	}
}


/*****************************************************************************
 *
 *	�����T�v�FYP�رَ�M�^�X�N����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
/* USER CODE Header_YpSerialRxTaskEntry */

void YpSerialRxTaskEntry(void const * argument)
{

  /* USER CODE BEGIN YpSerialRxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
		osEvent event = osMailGet(YpSerialRxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			YpSerialBuff_t *p_Rxmail = (YpSerialBuff_t *)event.value.p;
			if (p_Rxmail)
			{
				if( p_Rxmail->len )
				{

					//��M�f�[�^���
                    parse_yp_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len );

               	}
               	osMailFree(YpSerialRxMailHandle, p_Rxmail);
           	}
		}
	    osDelay(1);
	}
}

/*****************************************************************************
 *
 *	�����T�v�FYP�رّ��M�^�X�N����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
/* USER CODE Header_YpSerialTxTaskEntry */

void YpSerialTxTaskEntry(void const * argument)
{
  for(;;)
  {
		osEvent event = osMailGet(YpSerialTxMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
			YpSerialBuff_t *p_mail = (YpSerialBuff_t *)event.value.p;
			if (p_mail)
			{
				if( p_mail->len )
				{
                	yp_tx.len = p_mail->len;
                	memcpy(yp_tx.buff, p_mail->buff, yp_tx.len);

               		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)yp_tx.buff, yp_tx.len);
               	}
               	osMailFree(YpSerialTxMailHandle, p_mail);
           	}
		}
		//waiting for tx complete signal (or timeout)
		osSignalWait(SIGNAL_YPSERIAL_TXCOMP, yp_tx.len*100);

		osDelay(1);
  }
}

/*****************************************************************************
 *
 *	�����T�v�FYP�رّ��M����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
void UART2_TxCpltCallback( void )
{
    osSignalSet(YpSerialTxHandle, SIGNAL_YPSERIAL_TXCOMP);
}



/*****************************************************************************
 *
 *	�����T�v�FYP�رّ��M����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
//--------------------------------------------------------------------------------
static void tx_yp_serial( U1 dst_adr, YpSerialCmd_e cmd, const U1 *p_data, int data_len )
{
    YpSerialBuff_t *p_yps = osMailAlloc( YpSerialTxMailHandle, osWaitForever );
    
    if( p_yps )
    {
        memset( p_yps, 0, sizeof(*p_yps) );

        p_yps->buff[IDX_ADR] = (U1)((ADR_SELF << 4) | (dst_adr & 0x0f));
        p_yps->buff[IDX_LEN] = (U1)data_len;
        p_yps->buff[IDX_SCR] = NO_SCR;
        p_yps->buff[IDX_CMD] = (U1)cmd;
        if( p_data )
        {
            memcpy(&p_yps->buff[IDX_DAT], p_data, data_len );
        }

		/* ���M���������т��v�Z���đ��M�ް��Ƃ��ľ�Ă���*/
		/* �����юZ�o */
        p_yps->buff[idx_sum(p_yps->buff)] = yp_serial_checksum( p_yps->buff );

        p_yps->len = 5 + p_yps->buff[IDX_LEN];

        osMailPut(YpSerialTxMailHandle, p_yps);
    }
}


//--------------------------------------------------------------------------------
//
//		��M�f�[�^���
//
//--------------------------------------------------------------------------------
static Bool parse_yp_serial( const U1 *p_buff, int len )
{

	// CHECKSUM����
    if( yp_serial_checksum(p_buff) != p_buff[idx_sum(p_buff)] )
    {
        return false;
    }

	// �X�N�����u���VA����
    if( p_buff[IDX_SCR] != 0x7A )
    {
        return false;
    }

	// �A�h���X�ݒ�
    U1 src_adr = GET_SRCADR(p_buff[IDX_ADR]);

    switch( p_buff[IDX_CMD] )
    {

	case	ReqCmd_YpSerialStatus:	// 0xA0
			{
				//�X�e�[�^�X����
				U1 ret_code[3];
			  	U1 ret_status;
			  	U1 ret_cardkind;
			  	U1 ret_err;
				
				// �X�e�[�^�X�v��
				req_status(&ret_status, &ret_cardkind,	&ret_err );

				//�X�e�[�^�X�Z�b�g 
				ret_code[0] = ret_status;
				//�J�[�h��ʃZ�b�g 
				ret_code[1] = ret_cardkind;
				//�G���[�R�[�h�Z�b�g 
				ret_code[2] = ret_err;

				tx_yp_serial( src_adr, Cmd_YpSerialStatus, &ret_code[0], sizeof(ret_code) );
							//�A�h���X    �@�@�R�}���h�@�@�@�@�f�[�^�@�@�@�@�@�@�f�[�^��

			}
	        break;

	case	ReqCmd_YpSerialReset:	// 0xA1
			{
				// ���Z�b�g����
		  		U1 ret_code[1] = {0x00};// 0x00 fix
				tx_yp_serial( src_adr, Cmd_YpSerialReset, &ret_code[0], sizeof(ret_code) );





			}        
		
			break;

	case	ReqCmd_YpSerialReqKind:	// 0xA2
			{
				// �J�[�h��ʗv��
		  		U1 ret_code[1];
				U1 p;

				// �ύX�f�[�^�ݒ�
				p = p_buff[IDX_DAT];
				req_chgkind(&p);

		  		
				// �J�[�h�ǎ��ʉ���
		  		ret_code[0] = p_buff[IDX_DAT];
		  		
				tx_yp_serial( src_adr, Cmd_YpSerialReqKind, &ret_code[0], sizeof(ret_code) );


			}        
        	break;

// TEST

	case	ReqCmd_YpSerialReserve1:	// 0xA3
			{
				// ���C������փf�[�^���M   
		    	MainControlBuff_t *p_mail = osMailAlloc(MainControlMailHandle, osWaitForever);
            	if( p_mail )
            	{
                	p_mail->status = RX_YPSERIAL;
                	p_mail->len = len;
                	memcpy(p_mail->buff, p_buff, len);
                	osMailPut(MainControlMailHandle, p_mail);
            	}
	        }	
			break;

	case	ReqCmd_YpSerialReserve2: // 0xA4
			//TEST
			{
			  	U1 ret_code[15];
				tx_yp_serial( src_adr, Cmd_YpSerialReserve2, &ret_code[0], sizeof(ret_code) );

		    	MainControlBuff_t *p_mail = osMailAlloc(MainControlMailHandle, osWaitForever);
            	if( p_mail )
            	{
                	p_mail->status = RX_YPSERIAL;
                	p_mail->len = len;
                	memcpy(p_mail->buff, p_buff, len);
                	osMailPut(MainControlMailHandle, p_mail);
            	}


			}        
//TEST
			
        	break;


    default:
	    	return false;

    }
    return true;
}


//---------------------------------------
//
//			���M�v��
//
//---------------------------------------
void	req_tx_yp_serial( U1 src_adr, U1 command, const U1 *p_data, U1 len )
{

	tx_yp_serial( src_adr, command, &p_data[0], len );

}



/* USER CODE END 0 */
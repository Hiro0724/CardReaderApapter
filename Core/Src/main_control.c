
/******************************************************************************
 *
 *	�J���@��			�F�Ƌ��؃��[�_����p�A�_�v�^
 *	Micro-processingUnit�FSTM32G071GBU
 *-----------------------------------------------------------------------------
 *	�@�\����	�FMAIN���䏈���@
 *	�t�@�C����	�Fmain_control.c
 *	�t�@�C�������F���C�����䏈��
 *
 *	�쐬��		�F2021�N X�� X��
 *	�쐬��		�FHayashi
 *
 *			Copyright(C)	2021 YUPITERU Ltd. All rights reserved.
 *=============================================================================
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
#include "intevt.h"



/********************************************************************/
/*  �v���g�^�C�v��`                                                */
/********************************************************************/

extern osMailQId MainControlMailHandle;

extern void	req_tx_cr_serial( U1 src_adr, U1 command, const U1 *p_data, U1 len );


static Bool parse_yprx_serial( const U1 *p_buff, int len );
static Bool parse_crrx_serial( const U1 *p_buff, int len );
static void init_cr_tx_dataset(void);
static void nor_cr_tx_dataset(void);



typedef enum
{
    YPCMD_CARDREADERSTATUS = 0xA0,
    YPCMD_CARDREADERRESET  = 0xA1,
    YPCMD_CARDREADERREQKIND = 0xA2,
    YPCMD_CARDREADERRESERVE1 = 0xA3,
    YPCMD_CARDREADERRESERVE2 = 0xA4,
}YpSerialCmd_e;


typedef enum
{
    MAINSTATUS_INI = 0x00,
    MAINSTATUS_NOR = 0x01,
    MAINSTATUS_ERR = 0x02,
    MAINSTATUS_TST = 0x03,
}MainStatus_e;


#define YP_IDX_ADR     (0)
#define YP_IDX_LEN     (1)
#define YP_IDX_SCR     (2)
#define YP_IDX_CMD     (3)
#define YP_IDX_DAT     (4)

#define CR_IDX_STX     (0)
#define CR_IDX_ADR     (1)
#define CR_IDX_CMD     (2)
#define CR_IDX_LEN     (3)
#define CR_IDX_DAT     (4)

#define CR_KIND_INI		(0x04) //TYPE B


/*==============================================================*/
/* [ ��`�錾 ]                                         		*/
/* 																*/
/*==============================================================*/
struct	jobtb	{
	const 	uint8_t	evnt;				/* �����		*/
	const	uint8_t	next;				/* ����Ԕԍ�	*/  // �����ɓ�����͖̂�����(No Condition)��
	void    (* const tsk)(void);		/* ������ڽ		*/
};


// �C�j�V�����V�[�P���X�ԍ�
typedef enum
{
    INIT_SEQ_CR_START = 0,              // ��������J�n��M
 
    INIT_SEQ_CR_TX_SETINVENTORY,        // INVENTORY���M
    INIT_SEQ_CR_RX_SETINVENTORY,        // INVENTORY��M
    INIT_SEQ_CR_TX_SETCOMMOD1,          // �ʐM���[�h�ݒ著�M
    INIT_SEQ_CR_RX_SETCOMMOD1,          // �ʐM���[�h�ݒ��M
    INIT_SEQ_CR_TX_SETMODE,             // ���샂�[�h�ݒ著�M
    INIT_SEQ_CR_RX_SETMODE,        		// ���샂�[�h�ݒ��M
    INIT_SEQ_CR_TX_SETCOMMOD2,          // �ʐM���[�h�ݒ著�M
    INIT_SEQ_CR_RX_SETCOMMOD2,       	// �ʐM���[�h�ݒ��M
    INIT_SEQ_CR_TX_REQMODE1,    		// �q�n�l�o�[�W�����擾���M
    INIT_SEQ_CR_RX_REQMODE1,            // �q�n�l�o�[�W�����擾��M
    INIT_SEQ_CR_TX_REQMODE2,    		// ���W���[���^�C�v�擾���M
    INIT_SEQ_CR_RX_REQMODE2,            // ���W���[���^�C�v�擾��M
    INIT_SEQ_CR_TX_REQMODE3,    		// �q�n�l�o�[�W�����擾���M
    INIT_SEQ_CR_RX_REQMODE3,            // �q�n�l�o�[�W�����擾��M

    INIT_SEQ_CR_COMPLETE,               // ����

}e_INIT_SEQ;

typedef enum
{
    STATUS_INIT = 0, 		             // �������쒆
    STATUS_NOR1,  			             // �ʏ�1���쒆
    STATUS_NOR2,  			             // �ʏ�2���쒆
    STATUS_TST,  			             // �e�X�g���쒆
}e_MAIN_STATUS;

static U1 init_seq;

typedef enum
{
    ERR_NON = 0, 			             // �G���[����
    ERR_CR, 			 	    	     // �J�[�h���[�_�G���[�i��������)
}e_MAIN_ERR;


/*==============================================================*/
/* [ �ϐ���`]                                         			*/
/* 																*/
/*==============================================================*/
static e_MAIN_STATUS	mainstatus;					/* ��Ԕԍ�		*/
static e_MAIN_STATUS	next_mainstatus;			/* ����Ԕԍ�	*/

static e_MAIN_ERR		err_status;					/* �G���[���	*/

F1 	read_cardkind;									// �ǂݍ��݃J�[�h���

/********************************************************************/
/*																	*/
/*				���C�����쐧��֐� 									*/
/*																	*/
/********************************************************************/
/*****************************************************************************
 *
 *	�֐����́Fvoid MAIN_Init( void )
 *	�����T�v�F���C�����쏉����
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
void MAIN_Init( void )
{

	mainstatus = STATUS_INIT; 			             // �������쒆
	next_mainstatus = STATUS_INIT; 			         // �������쒆
	err_status = ERR_NON;

	init_seq = INIT_SEQ_CR_TX_SETINVENTORY;

 	read_cardkind.byte = CR_KIND_INI;				// �ǂݍ��݃J�[�h��ʏ����l
	

	// �J�[�h���[�_�����ݒ�J�n
	init_cr_tx_dataset();

}


/*****************************************************************************
 *
 *	�����T�v�F�J�[�h���[�_�����ݒ�f�[�^���M����
 *	����	�F�R�}���h
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
static void init_cr_tx_dataset(void)
{
	const ST_CRDATA *ans;
	U1 *pp;

	U1 p_data[30];
	U1 i,j,len,cmd;
		
	if (init_seq >= INIT_SEQ_CR_COMPLETE)
	{
		return;
	}

	//�@�f�[�^�����o
	i = init_seq / 2;

	cmd = CR_TX_INI_DATA_TBL[i].cmd;

	ans = CR_TX_INI_DATA_TBL;

	for(j = 0; j < i; j++)
	{
		++ans;
	}


	len = *ans->str;
	pp = ans->str;

	for(j = 1; j <= len; j++)
	{
		p_data[(j-1)] = *(pp+j);
	}

	// �R�}���h�Z�b�g



	req_tx_cr_serial( 0x00, cmd ,&p_data[0], len );
}


/*****************************************************************************
 *
 *	�����T�v�F�J�[�h���[�_�f�[�^���M����
 *	����	�F�R�}���h
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
static void nor_cr_tx_dataset(void)
{
	const ST_CRDATA *ans;
	U1 *pp;

	U1 p_data[30];
	U1 i,j,len,cmd;
		
	//�@�f�[�^�����o
	i = init_seq / 2;

	cmd = CR_TX_NOR_DATA_TBL[i].cmd;

	ans = CR_TX_NOR_DATA_TBL;

	for(j = 0; j < i; j++)
	{
		++ans;
	}


	len = *ans->str;
	pp = ans->str;

	for(j = 1; j <= len; j++)
	{
		p_data[(j-1)] = *(pp+j);
	}

	// �R�}���h�Z�b�g



	req_tx_cr_serial( 0x00, cmd ,&p_data[0], len );
}


/*****************************************************************************
 *
 *	�����T�v�FYP�V���A���f�[�^���M����
 *	����	�F�R�}���h
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/
static void nor_yp_tx_dataset(void)
{
	const ST_CRDATA *ans;
	U1 *pp;

	U1 p_data[30];
	U1 i,j,len,cmd;
		
	//�@�f�[�^�����o

//test
	i = init_seq;



	cmd = YP_TX_NOR_DATA_TBL[i].cmd;


	ans = YP_TX_NOR_DATA_TBL;

	for(j = 0; j < i; j++)
	{
		++ans;
	}


	len = *ans->str;
	pp = ans->str;

	for(j = 1; j <= len; j++)
	{
		p_data[(j-1)] = *(pp+j);
	}

	// �R�}���h�Z�b�g



	req_tx_yp_serial( 0x00, cmd ,&p_data[0], len );
}







/*****************************************************************************
 *
 *	�����T�v�F���C�����䏈��
 *	����	�F�Ȃ�
 *	�߂�l	�F�Ȃ�
 *
 *****************************************************************************/

void MainControlTaskEntry(void const * argument)
{

  /* USER CODE BEGIN MainControlTaskEntry */
  /* Infinite loop */
  for(;;)
  {
		osEvent event = osMailGet(MainControlMailHandle, osWaitForever);
		if (event.status == osEventMail )
		{
				MainControlBuff_t *p_Rxmail = (MainControlBuff_t *)event.value.p;
				if (p_Rxmail)
				{
					//��M�f�[�^���
					switch(p_Rxmail->status)
					{
						case RX_YPSERIAL:		// from yp serial
				   			parse_yprx_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len );
							break;

						case RX_CRSERIAL:		// from card reader
				   			parse_crrx_serial( (const U1 *)p_Rxmail->buff, p_Rxmail->len );
							break;

    					default:
	    					return false;

					}
				}
       	osMailFree(MainControlMailHandle, p_Rxmail);
		}
	    osDelay(1);
	}
}



//--------------------------------------------------------------------------------
//
//						��M�f�[�^���
//
//--------------------------------------------------------------------------------
//***************************************************
//		YP�V���A������̃f�[�^
//***************************************************

static Bool parse_yprx_serial( const U1 *p_buff, int len )
{

	switch( p_buff[YP_IDX_CMD] )
	{
		case YPCMD_CARDREADERRESERVE1:

			{
			  	U1 ret_code[15];
				ret_code[0] = 1;
				ret_code[1] = 2;
			}
			break;

		case YPCMD_CARDREADERRESERVE2:
			{
			  	U1 ret_code[2];
				ret_code[0] = 1;
				ret_code[1] = 2;
			}

			break;

    default:
	    	return false;

    }
    return true;
}



//******************************************
//		�J�[�h���[�_�@�C�j�V�����ݒ�
//		���M��C�j�V�����V�[�P���XUP
//
//******************************************
void init_sq_up( void )
{
	if (mainstatus == STATUS_INIT) 			             // �������쒆
	{
		++init_seq;
	}
}


//******************************************
//		�X�e�[�^�X�⍇���v������
//
//******************************************
void req_status( U1 *p_status, U1 *p_kind, U1 *p_err )
{
	U1 a;


	a = mainstatus;
	*p_status = a;

	*p_kind = read_cardkind.byte;	 

	a = err_status;
	*p_err = a;

}

//******************************************
//		�ǎ�J�[�h��ʕύX
//
//******************************************
void req_chgkind( U1 *p_kind)
{

	read_cardkind.byte = *p_kind;	 

}


//******************************************
//		�J�[�h���[�_�@�C�j�V�����ݒ�
//		�C�j�V�����J�n 
//      ACK�@��M
//******************************************
void	tsk000(void)
{

	switch (init_seq)
	{

		case INIT_SEQ_CR_TX_SETINVENTORY:        // INVENTORY��M
			
			
			init_seq = INIT_SEQ_CR_TX_SETCOMMOD1;
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD1:          // �ʐM���[�h�ݒ��M

			init_seq = INIT_SEQ_CR_TX_SETMODE;
			break;   

		case INIT_SEQ_CR_TX_SETMODE:        	// ���샂�[�h�ݒ��M

			init_seq = INIT_SEQ_CR_TX_SETCOMMOD2;
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD2:       	// �ʐM���[�h�ݒ��M

			init_seq = INIT_SEQ_CR_TX_REQMODE1;
			break;   

		case INIT_SEQ_CR_TX_REQMODE1:            // �q�n�l�o�[�W�����擾��M

			init_seq = INIT_SEQ_CR_TX_REQMODE2;
			break;   

		case INIT_SEQ_CR_TX_REQMODE2:            // ���W���[���^�C�v�擾��M

			init_seq = INIT_SEQ_CR_TX_REQMODE3;
			break;   

		case INIT_SEQ_CR_TX_REQMODE3:            // �q�n�l�o�[�W�����擾��M

			mainstatus = STATUS_NOR1; 			 // �ʏ퓮�쒆
			next_mainstatus = STATUS_NOR1; 		 // �ʏ퓮�쒆
			init_seq = INIT_SEQ_CR_COMPLETE;

			//YP SERIAL�ɏ����ݒ芮���ʒm
init_seq = 0;
			nor_yp_tx_dataset();
		

			break;   
	}
	

	// �J�[�h���[�_�����ݒ著�M
	if (init_seq < INIT_SEQ_CR_COMPLETE)
	{
		init_cr_tx_dataset();
	}
}


//******************************************
//		�J�[�h���[�_�@�C�j�V�����ݒ�
//		�C�j�V�����J�n 
//      NACK�@��M
//******************************************
void	tsk001(void)
{
	switch (init_seq)
	{

		case INIT_SEQ_CR_TX_SETINVENTORY:        // INVENTORY��M

			// �đ��M			
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD1:          // �ʐM���[�h�ݒ��M

			// �đ��M			
			break;   

		case INIT_SEQ_CR_TX_SETMODE:        	// ���샂�[�h�ݒ��M

			// �đ��M			
			break;   

		case INIT_SEQ_CR_TX_SETCOMMOD2:       	// �ʐM���[�h�ݒ��M

			// �đ��M			
			break;   

		case INIT_SEQ_CR_TX_REQMODE1:            // �q�n�l�o�[�W�����擾��M

			// �đ��M			
			break;   

		case INIT_SEQ_CR_TX_REQMODE2:            // ���W���[���^�C�v�擾��M

			// �đ��M			
			break;   

		case INIT_SEQ_CR_TX_REQMODE3:            // �q�n�l�o�[�W�����擾��M

			// �đ��M			
			break;   
	}


}

/*------------------------------------------*/
/*	�h�m�h�s()	����������					*/
/*------------------------------------------*/
const	struct	jobtb tb_INIT[] =
{
    {CR_COM_ACK, STATUS_INIT,tsk000},			// ACK��M

    {CR_COM_NACK,STATUS_INIT,tsk001},       	// NACK��M

    {  		   0,    	   0,     0},           // �I�[
};



//******************************************
//		�J�[�h���[�_�@�ʏ펞
//		������M
//
//******************************************
void	tsk010(void)
{

		// �e�X�g�@test
		
init_seq = 1;
		nor_yp_tx_dataset();				// YP Serial�֑��M
		next_mainstatus = STATUS_NOR2; 		 // �ʏ퓮�쒆
}

void	tsk011(void)
{
}

void	tsk012(void)
{
		// �e�X�g test
init_seq = 3;
		nor_yp_tx_dataset();				// YP Serial�֑��M
		next_mainstatus = STATUS_NOR1; 		 // �ʏ퓮�쒆
}


/*------------------------------------------*/
/*		�ʏ퓮��@NOR1						*/
/*------------------------------------------*/
const	struct	jobtb tb_NOR1[] =
{
    {CR_COM_ACK, STATUS_INIT,tsk010},			// ACK��M

    {CR_COM_NACK,STATUS_INIT,tsk011},       	// NACK��M

    {  		   0,    	   0,     0},           // �I�[
};


/*------------------------------------------*/
/*		�ʏ퓮��@NOR2						*/
/*------------------------------------------*/
const	struct	jobtb tb_NOR2[] =
{
    {CR_COM_ACK, STATUS_INIT,tsk012},			// ACK��M

    {CR_COM_NACK,STATUS_INIT,tsk001},       	// NACK��M

    {  		   0,    	   0,     0},           // �I�[
};


/*---------------------------------------------------------------
	�����ð��ٌ���ð���
----------------------------------------------------------------*/
const struct jobtb *const tbevad[] = {
	tb_INIT,						/* ������						*/
	tb_NOR1,						/* �ʏ탂�[�h�`					*/
	tb_NOR2,						/* �ʏ탂�[�h�`�i�\���j			*/
};

//***************************************************
//		�J�[�h���[�_����̃f�[�^
//***************************************************

static Bool parse_crrx_serial( const U1 *p_buff, int len )
{

	const struct jobtb	*p;
	U1 evnt;
	
	evnt = p_buff[CR_IDX_CMD];             /* �C�x���g(����M�R�}���h)���o��  */

   	/* �^�X�N����    */
    for (p = tbevad[mainstatus] ; p->evnt != 0; p++){

    	if ( p->evnt == evnt){	                             /* ����Ă���v����܂Ō���   */
        	next_mainstatus = p->next;                       /* ����Ԕԍ���ǂݍ���      */
            (*(p->tsk))();                                   /* ��������s����             */
            if(next_mainstatus > 0)
            {
            	if(mainstatus != next_mainstatus)
            	{    								        /* ����Ԑݒ�           */
                	mainstatus = next_mainstatus;
                }
            }
            break;
        }
	}

    return true;
}







/* USER CODE END 0 */
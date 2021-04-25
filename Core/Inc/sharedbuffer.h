/******************************************************************************
 *
 *	�J���@��			�F�Ƌ��؃��[�_����p�A�_�v�^
 *	Micro-processingUnit�FSTM32G071GBU
 *-----------------------------------------------------------------------------
 *	�@�\����	�F�������o�b�t�@�Ǘ��p�w�b�_�[�t�@�C��
 *	�t�@�C����	�Fsharedbuffer.h
 *	�t�@�C�������F���L�������o�b�t�@�t�H�[�}�b�g�^��`�p
 *	�쐬��		�F2021�N x�� x��
 *	�쐬��		�F------
 *
 *			Copyright(C)	2016 YUPITERU Ltd. All rights reserved.
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
 #ifndef	__SHAREDMEMORY__
 #define	__SHAREDMEMORY__
 




/*-------------------------------------------------------*
 *	�萔��`
 *-------------------------------------------------------*/ 
#define		SB_OFF		(0)				/* ���ʃ������o�b�t�@�p�@OFF��`	*/
#define		SB_ON		(1)				/* ���ʃ������o�b�t�@�p�@ON��`		*/

#define		SB_OK		(0)				/* ����	*/
#define		SB_NG		(1)				/* �ُ�	*/
#define		SB_BLOCKNG	(2)				/* ������郁�����u���b�N���Ⴄ		*/
#define		SB_RELEASENG (3)			/* ����v���������W���[�����������Ȃ�*/
#define		SB_BLOCKBUSY (4)			/* �g�p�����W���[���L��				*/


/*------------- �o�b�t�@ID��` ----------------*/
#define		SBID_NONE	(0)
#define		SBID_BIG	(1)				/* 112Byte�P�ʃ������u���b�N		*/
#define		SBID_LITTLE (2)				/* 48Byte�P�ʃ������u���b�N			*/



/*------------- ���W���[��ID��`(�����[�X) --------------*/
#define		NONE_ID (0xFF)				/* �g�p���W���[�����m��				*/
#define		OBD2COMM_MODULE		0x01	/* �n�a�c�Q�ʐM���W���[��			*/
#define		DTCTCOMM_MODULE		0x02	/* ���m�@��ʐM���W���[��			*/
#define		YPDVCOMM_MODULE		0x04	/* �x�o�@��ʐM���W���[��			*/



/*------------- �o�b�t�@�T�C�Y��` --------------*/
#define		CRD_RCV_BUF_SIZE	(127)	/* �J�[�h���[�_�V���A����M�p�o�b�t�@�T�C�Y 	*/
#define		YPS_RCV_BUF_SIZE	(20)	/* ���ʃV���A����M�p�o�b�t�@�T�C�Y				*/

/*------------- �ʐM�f�o�C�XID ------------------*/
#define		YP_DEV_ID			0x01	/* YP���i�f�o�C�X					*/
#define		OBD2_DEV_ID			0x02	/* OBD2�A�_�v�^�[�f�o�C�X			*/
#define		DTC_DEV_ID			0x03	/* ���m�@��							*/
//#define		FCWS_DEV_ID			0x03	/* �Ԋԋ������m�f�o�C�X				*/
//#define		LKOW_DEV_ID			0x04	/* �悻�����m�J����					*/

/*------------- �u���b�N�o�b�t�@����` ----------*/
#define		RAM_OBDBUFFER_MAX	(10)		/* ��OBD�A�_�v�^��M��p�o�b�t�@�ő����` */
#define		RAM_STDBUFFER_MAX	(40)	/* �V���A���ʐM�p�o�b�t�@�ő����`*/



/* YP�t�H�[�}�b�g�ʐM����M�X�e�[�^�X�p�\���̒�` */
typedef		struct	{
	U1			rxreq;						/* ��M�f�[�^�L����			*/
	U1			txreq;						/* ���M�f�[�^�L����			*/
} YPS_STATUS;







/*-------------------------------------------------------*
 *	���ʃo�b�t�@�������^��`
 *-------------------------------------------------------*/


/*-------------------------------------------------------*
 *	����M�L���[�^��`
 *-------------------------------------------------------*/
typedef struct {
	void *p_tx_top_pri;			/* ���M�ޯ̧�D�淭�					*/
	void *p_tx_top;				/* ���M�ޯ̧���						*/
	void *p_rx_top;				/* ��M�ޯ̧���						*/

	U1		enablef;				/* �ʐM�C�l�[�u���t���O				*/
	U1		connectionDevice;		/* �ʐM���Ă���f�o�C�XID			*/

} DEVCOMMQUE;



/*-------------------------------------------------------*
 *	�O���Q�Ɗ֐���`
 *-------------------------------------------------------*/
void	SB_Initialize( void );

void * SB_IntGetOBDMemory(
	U1	releaseID		/* �����[�X�惂�W���[��ID�w��		*/
);


void * SB_IntGetMemory(
	U1	getID,			/* �������u���b�N�擾���h�c�@�w��	*/
	U1	releaseID		/* �����[�X�惂�W���[��ID�w��		*/
);

void * SB_GetOBDMemory(
	U1	releaseID		/* �����[�X�惂�W���[��ID�w��		*/
);

U1 SB_ReleaseMemory(
	U1	releaseID,		/* ������郂�W���[���h�c			*/
	void 	*release		/* ������郁�����擪�A�h���X		*/
);

void SB_AllMemoryRelease(
	U1	releaseID,		/* ������郂�W���[���h�c			*/
 	void 	*release		/* ������郁�����擪�A�h���X		*/
);

void *SB_QueOutBuffer(
	void	**quetop			/* �L���[�̐擪�A�h���X				*/
);

void SB_QueInBuffer(
	void	**quetop,		/* �L���[�̐擪�A�h���X				*/
	void	*buffer			/* �o�^����o�b�t�@�̐擪�A�h���X	*/
);
 
 #endif
/*******************************************************************************/
/*******************************************************************************/
/******************************** End Of File **********************************/
/*******************************************************************************/
/*******************************************************************************/
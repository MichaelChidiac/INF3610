/*
*********************************************************************************************************
*                                                 uC/OS-II
*                                          The Real-Time Kernel
*
*
*
*            					�cole Polytechnique de Montr�al, Qc, CANADA
*                                                  09/2013
*
* File : routeur.C
*********************************************************************************************************
*/

#include "includes.h"
/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

// Size of each task's stacks (# of WORDs)

#define	UBYTE unsigned char

// Application tasks IDs
#define TASK_STOP_ID    		0
#define TASK_INJECTPACKET_ID  	1
#define TASK_PRINT_ID 			2
#define TASK_COMPUTING_ID     	3
#define TASK_FORWARDING_ID    	4
#define TASK_VERIFICATION_ID 	5

// Application tasks priorities
#define TASK_STOP_PRIO 			10
#define TASK_VERIFICATION_PRIO 	11
#define TASK_INJECTPACKET_PRIO 	12
#define TASK_PRINT_PRIO 		13
#define TASK_COMPUTING_PRIO 	14
#define TASK_FOWARDING_PRIO 	15

// Routing info.
#define INT4_LOW  0				/* 0x00000000 */
#define INT4_HIGH 0x3fffffff 	/* 1073741823 */
#define INT3_LOW  0x40000000 	/* 1073741824 */
#define INT3_HIGH 0x7fffffff 	/* 2147483647 */
#define INT2_LOW  0x80000000 	/* 2147483648 */
#define INT2_HIGH 0xC0000000 	/* 3221225472 */
#define INT1_LOW  0xC0000001 	/* 3221225473 */
#define INT1_HIGH 0xffffffff 	/* 4294967295 */

// Reject source info.
#define REJECT_LOW1  0x10000000
#define REJECT_HIGH1 0x100FFFFF
#define REJECT_LOW2  0x50000000
#define REJECT_HIGH2 0x500FFFFF
#define REJECT_LOW3  0x60000000
#define REJECT_HIGH3 0x600FFFFF
#define REJECT_LOW4  0xD0000000
#define REJECT_HIGH4 0xD00FFFFF

#define FIFO_IN_NUM_MSG 	16
#define FIFO_QOS_NUM_MSG 	4

#define CHANNEL1 1

#define High 	0
#define	Medium 	1
#define	Low 	2

typedef struct {
	unsigned int src;
	unsigned int dst;
	unsigned int type;
    unsigned int crc;
 	unsigned int data[12];
}Packet;

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

/* Declare global variables */
static XGpio instancePtrLED;
static const int LEDMask = 0xff;

OS_STK TaskStopStk[TASK_STK_SIZE];
OS_STK TaskInjectPacketStk[TASK_STK_SIZE];
OS_STK TaskComputingStk[TASK_STK_SIZE];
OS_STK TaskForwardingStk[TASK_STK_SIZE];
OS_STK TaskVerificationStk[TASK_STK_SIZE];
OS_STK TaskPrintStk[TASK_STK_SIZE];

int nbPacket 				= 0; // Nb de packets total traites
int nbPacketLowRejete 		= 0; // Nb de packets low rejetes
int nbPacketMediumRejete 	= 0; // Nb de packets Medium rejetes
int nbPacketHighRejete 		= 0; // Nb de packets High rejetes
int nbPacketCRCRejete 		= 0; // Nb de packets rejetes pour mauvais crc
int nbPacketSourceRejete 	= 0; // Nb de packets rejetes pour mauvaise source
INT8U i = 0;

// Print function parameters
typedef struct
{
  unsigned int interfaceID;
  OS_EVENT *Mbox;
} PRINT_PARAM;

PRINT_PARAM printRequest;

typedef enum { false, true } bool;

OS_EVENT *ptrFifoIn;
void *FifoInMsgTbl[FIFO_IN_NUM_MSG];

OS_EVENT *ptrFifoVideo, *ptrFifoAudio, *ptrFifoOtherwise, *ptrFifoAuxilary;
void *FifoInMsgTbl[FIFO_IN_NUM_MSG];
void *FifoVideoMsgTbl[FIFO_QOS_NUM_MSG];
void *FifoAudioMsgTbl[FIFO_QOS_NUM_MSG];
void *FifoOtherwiseMsgTbl[FIFO_QOS_NUM_MSG];
void *FifoAuxilaryMsgTbl[FIFO_QOS_NUM_MSG];

OS_EVENT *SemStop, *SemPrint, *SemVerification;
OS_EVENT *Int1Mbox, *Int2Mbox, *Int3Mbox, *Int4Mbox;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
/* Function prototypes of tasks */
void initLED();
void TaskInjectPacket(void *data);
void TaskComputing(void *data);
void TaskForwarding(void *data);
void TaskPrint(void *data);
void TaskVerification (void *pdata);
void TaskStop(void *data);
void _CreateTask();
void _DeleteTask();

/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/
int main (void)
{
	 /* Initialize �C/OS-II */
	OSInit();

    /* Create Semaphore */
	SemPrint		= OSSemCreate(0);
    SemStop 		= OSSemCreate(0);
    SemVerification = OSSemCreate(0);

    initLED();

    /* Create queue */
    ptrFifoIn 			= OSQCreate(&FifoInMsgTbl[0], 			FIFO_IN_NUM_MSG);
    ptrFifoVideo 		= OSQCreate(&FifoVideoMsgTbl[0], 		FIFO_QOS_NUM_MSG);
    ptrFifoAudio 		= OSQCreate(&FifoAudioMsgTbl[0], 		FIFO_QOS_NUM_MSG);
    ptrFifoOtherwise 	= OSQCreate(&FifoOtherwiseMsgTbl[0], 	FIFO_QOS_NUM_MSG);
    ptrFifoAuxilary 	= OSQCreate(&FifoAuxilaryMsgTbl[0], 	FIFO_QOS_NUM_MSG);

    /* Create mailbox */
    Int1Mbox 	= OSMboxCreate((void *)0);
    Int2Mbox 	= OSMboxCreate((void *)0);
    Int3Mbox 	= OSMboxCreate((void *)0);
    Int4Mbox 	= OSMboxCreate((void *)0);

    _CreateTask();

    BSP_InitIO();

    /* Start Multitasking */
    OSStart();

    return 1;
}

/*
*********************************************************************************************************
*                                              fitTimerHandler
* Ajoutez les xil_printf dans le code de la t�che aux bons endroits
*xil_printf("\nVerification completee %d!\n", i);
*
*********************************************************************************************************
*/
void fitTimerHandler (void* par)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = 0;
#endif

	OS_ENTER_CRITICAL(); /* Disable interrupts */
	if(nbPacketCRCRejete > 15)
	{
		xil_printf("\nLa tache logicielle taskStop doit etre reveillee, "
				"car le nombre de paquets qui a �t� rejet� � cause que le CRC �tait invalide est > 15 (%d).\n", nbPacketCRCRejete);
		OSSemPost(SemStop);
	}
	xil_printf("\nVerification completee %d!\n", i++);
	OS_EXIT_CRITICAL(); /* Enable interrupts */
}

/*
*********************************************************************************************************
*                                              fitTimer2Handler
* Ajoutez les xil_printf dans le code de la t�che aux bons endroits
*xil_printf("\nLancement tache verification %d!\n", i);
*
*********************************************************************************************************
*/
void fitTimer2Handler (void* par)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = 0;
#endif
	/* devra r�veiller la t�che verification pour qu�elle ex�cute son code */
	OSSemPost(SemVerification);
	OS_ENTER_CRITICAL();
	xil_printf("\nLancement tache verification %d!\n", ++i);
	OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                              initLED
*********************************************************************************************************
*/
void initLED()
{
	/* Initialize LEDs on the board. */
	XGpio_Initialize(&instancePtrLED, XPAR_LEDS_8BIT_DEVICE_ID);
	XGpio_SetDataDirection(&instancePtrLED, CHANNEL1, 0x00); /* set Leds as output ports */
}

/*
*********************************************************************************************************
*                                              computeCRC
*********************************************************************************************************
*/
unsigned int computeCRC( INT16U* w, int nleft)
{
    unsigned int sum = 0;
    INT16U answer = 0;

    // Adding words of 16 bits
    while (nleft > 1)  {
      sum += *w++;
      nleft -= 2;
    }

    // Handling the last byte
    if (nleft == 1) {
      *(unsigned char *)(&answer) = *(const unsigned char *)w ;
      sum += answer;
    }

    // Handling overflow
    sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);

    answer = ~sum;
    return (unsigned int) answer;
}
/*
*********************************************************************************************************
*                                              TaskStop
*Ajoutez les xil_printf dans le code de la t�che aux bons endroits
*xil_printf("ERREUR : Trop de CRC invalide !\n");
*
*********************************************************************************************************
*/
void TaskStop (void *data)
{
	INT8U err;
	OSSemPend(SemStop, 0, &err);
	xil_printf("ERREUR : Trop de CRC invalide ! Le routeur va s'arreter!");
	BSP_IntDisAll();
	_DeleteTask();
}

/*
*********************************************************************************************************
*                                              TaskInjectPacket
*********************************************************************************************************
*/
void TaskInjectPacket (void *data)
{
   int i, j;
   INT8U err;

    // Initialization of the rand function
    srand(4600);
    Packet *packet;

    i = 0;
    for (;;) {

    	packet = malloc(sizeof(Packet));

    	// Creating packet
        packet->src = rand();
        packet->dst = 2*(unsigned int)rand();
        packet->type = rand() % 3;		// Three types of packet (High = 0, Medium = 1, Low = 2)

        for(j=0; j<12; j++) {
            packet->data[i] = rand();
        }

        packet->crc = 0;
        if (rand()%10 == 9)			// 10% of Packets with bad CRC
			packet->crc = 1234;
		else
			packet->crc = computeCRC( (INT16U*)(packet), 64 );

		xil_printf("\n********Generation du Paquet # %d ******** \n", ++i);
		xil_printf("ADD 0x%8x \n", packet);
		xil_printf("	** src : 0x%8x \n", packet->src);
		xil_printf("	** dst : 0x%8x \n", packet->dst);
		xil_printf("	** type : %d \n", packet->type);
		xil_printf("	** crc : %8x \n", packet->crc);

		err = OSQPost(ptrFifoIn, packet);

		if (err == OS_ERR_Q_FULL) {
			xil_printf("--TaskInjectPacket: Paquet rejete a l'entree car la fifo est pleine !\n");
		}

		//Packet debit (2 packets per second)
		OSTimeDly(50);
   }
}


/*
*********************************************************************************************************
*                                              TaskComputing
*Ajoutez les xil_printf dans le code de la t�che aux bons endroits
*xil_printf("\n--TaskComputing: Source invalide (Paquet rejete) (total : %d)\n\n", ++nbPacketSourceRejete);
*xil_printf("\n--TaskComputing: CRC invalide (Paquet rejete) (total : %d)\n", ++nbPacketCRCRejete);
*xil_printf("\n--TaskComputing: Fifo high pleine (Paquet rejete) (total : %d) !\n", ++nbPacketHighRejete);
*xil_printf("\n--TaskComputing: Fifo medium pleine (Paquet rejete) (total : %d) !\n", ++nbPacketMediumRejete);
*xil_printf("\n--TaskComputing: Fifo low pleine (Paquet rejete) (total : %d) !\n", ++nbPacketLowRejete);
*														
*********************************************************************************************************
*/
void TaskComputing (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = 0;
#endif

	INT8U err;
	void* msg;
	Packet *packet;
	bool bRejectInterval, bRejectInterval1, bRejectInterval2, bRejectInterval3, bRejectInterval4;
	unsigned int crc;

	for(;;)
	{
		msg = OSQPend(ptrFifoIn, 0, &err);
		packet = (Packet *)msg;

		/* rejeter les paquets qui ne sont pas dans son espace d�adressage */
		bRejectInterval1 	= packet->src > REJECT_LOW1 && packet->src < REJECT_HIGH1;
		bRejectInterval2 	= packet->src > REJECT_LOW2 && packet->src < REJECT_HIGH2;
		bRejectInterval3 	= packet->src > REJECT_LOW3 && packet->src < REJECT_HIGH3;
		bRejectInterval4 	= packet->src > REJECT_LOW4 && packet->src < REJECT_HIGH4;

		bRejectInterval 	= bRejectInterval1 || bRejectInterval2 || bRejectInterval3 || bRejectInterval4;

		/* tous les paquets ayant une adresse source contenue entre REJECT_LOW et REJECT_HIGH de chaque plage pour les interfaces devront �tre rejet�s */
		if(bRejectInterval)
		{
			xil_printf("\n--TaskComputing: Source invalide (Paquet rejete) (total : %d)\n\n", ++nbPacketSourceRejete);
			free(packet); packet = 0;
		}
		else
		{
			/* Les paquets contenant des erreurs sont simplement rejet�s*/
			crc = packet->crc;
			packet->crc = 0;
			if(crc != computeCRC((INT16U*)(packet), 64))
			{
				OS_ENTER_CRITICAL();
				xil_printf("\n--TaskComputing: CRC invalide (Paquet rejete) (total : %d)\n", ++nbPacketCRCRejete);
				OS_EXIT_CRITICAL();
				free(packet); packet = 0;
			}
			else
			{
				/* selon le type de paquets (vid�o, audio et autres, respectivement 1, 2 et 3), les paquets valides seront envoy�s dans trois queues diff�rentes */
				/* Si les queues sont pleines vous devrez rajouter les paquets dans une queue qui sera lu par la t�che verification.*/
				/* Si cette queue est pleine aussi, vous devez d�truire le paquet */
				packet->crc = crc;
				switch(packet->type)
				{
				case High:
					err = OSQPost(ptrFifoVideo, packet);
					if (err == OS_ERR_Q_FULL)
					{
						xil_printf("\n--TaskComputing: Fifo high pleine !\n");
						err = OSQPost(ptrFifoAuxilary, packet);
						if (err == OS_ERR_Q_FULL)
						{
							xil_printf("\n--TaskComputing: Fifo verification pleine (Paquet high efface (total : %d)) !\n", ++nbPacketHighRejete);
							free(packet); packet = 0;
						}
					}
					break;

				case Medium:
					err = OSQPost(ptrFifoAudio, packet);
					if (err == OS_ERR_Q_FULL)
					{
						xil_printf("\n--TaskComputing: Fifo medium pleine !\n", ++nbPacketMediumRejete);
						err = OSQPost(ptrFifoAuxilary, packet);
						if (err == OS_ERR_Q_FULL)
						{
							xil_printf("\n--TaskComputing: Fifo verification pleine (Paquet medium efface (total : %d)) !\n", ++nbPacketMediumRejete);
							free(packet); packet = 0;
						}
					}
					break;

				case Low:
					err = OSQPost(ptrFifoOtherwise, packet);
					if (err == OS_ERR_Q_FULL)
					{
						xil_printf("\n--TaskComputing: Fifo low pleine !\n");
						err = OSQPost(ptrFifoAuxilary, packet);
						if (err == OS_ERR_Q_FULL)
						{
							xil_printf("\n--TaskComputing: Fifo verification pleine (Paquet low efface (total : %d)) !\n", ++nbPacketLowRejete);
							free(packet); packet = 0;
						}
					}
					break;

				default:
					break;
				}
			}
		}
	}
}

/*
*********************************************************************************************************
*                                              TaskForwarding
*Ajoutez les xil_printf dans le code de la t�che aux bons endroits
*xil_printf("\n--TaskForwarding: %d paquets envoyes\n\n", ++nbPacket);
*********************************************************************************************************
*/
void TaskForwarding (void *pdata)
{
  INT8U err;
  void* msg;
  Packet *packet;
  int LEDValue = 0;

  pdata = pdata;

  bool bIntervalValid, bIntervalValid1, bIntervalValid2, bIntervalValid3, bIntervalValid4;

  for(;;)
  {
	  /*
	   * les paquets de haute priorit� doivent d�abord �tre trait�s.
	   * Si aucun paquet de haute priorit� n�est pr�t, alors un paquet de moyenne priorit� peut �tre envoy�.
	   * Sinon, un paquet de basse priorit� pourra �tre envoy�
	   */
	  if((msg = OSQAccept(ptrFifoVideo, &err))) {}
	  else if((msg = OSQAccept(ptrFifoAudio, &err))) {}
	  else if((msg = OSQAccept(ptrFifoOtherwise, &err))) {}

	  if(msg != (void *)0)
	  {
		  packet = (Packet *)msg;

		  bIntervalValid1 	= packet->dst > INT1_LOW && packet->dst < INT1_HIGH;
		  bIntervalValid2 	= packet->dst > INT2_LOW && packet->dst < INT2_HIGH;
		  bIntervalValid3 	= packet->dst > INT3_LOW && packet->dst < INT3_HIGH;
		  bIntervalValid4 	= packet->dst > INT4_LOW && packet->dst < INT4_HIGH;

		  /* afin de simuler l�acc�s � cette table, un wait d�une seconde doit �tre ajout� */
		  OSTimeDly(OS_TICKS_PER_SEC);

		  bIntervalValid = bIntervalValid1 || bIntervalValid2 || bIntervalValid3 || bIntervalValid4;

		  if(bIntervalValid)
		  {
			  if(bIntervalValid1)
			  {
				  printRequest.interfaceID = 1;
				  printRequest.Mbox = Int1Mbox;
			  }
			  else if(bIntervalValid2)
			  {
				  printRequest.interfaceID = 2;
				  printRequest.Mbox = Int2Mbox;
			  }
			  else if(bIntervalValid3)
			  {
				  printRequest.interfaceID = 3;
				  printRequest.Mbox = Int3Mbox;
			  }
			  else if(bIntervalValid4)
			  {
				  printRequest.interfaceID = 4;
				  printRequest.Mbox = Int4Mbox;
			  }
			  err = OSMboxPost(printRequest.Mbox, packet);
			  OSSemPost(SemPrint);
			  /* la communication via la t�che forwarding et l�interface se fera par un mailbox*/
			  if(err == OS_ERR_MBOX_FULL)
			  {
				  xil_printf("\n--TaskForwarding: Impossbile d'envoyer le paquet a l'interface.\n\n");
				  free(packet); packet = 0;
			  }
			  else
			  {
				  /* t�che forwarding met � jour une variable globale du nom de nbPacket indiquant le nombre total de paquets trait�s et �crire sur les DELs la valeur du nbPacket */
				  xil_printf("\n--TaskForwarding: %d paquets envoyes\n\n", ++nbPacket);
				  LEDValue = LEDMask & nbPacket;
				  XGpio_DiscreteWrite(&instancePtrLED, CHANNEL1, LEDValue);
			  }
		  }
	  }
  }
}

/*
*********************************************************************************************************
*                                              TaskVerification
*Ajoutez les xil_printf dans le code de la t�che aux bons endroits
*		xil_printf("\n********Remise du Paquet # %d ******** \n", ++i);
		xil_printf("	** src : 0x%8x \n", packet->src);
		xil_printf("	** dst : 0x%8x \n", packet->dst);
*********************************************************************************************************
*/
void TaskVerification (void *data)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = 0;
#endif

	INT8U err;
	void* msg;
	Packet *packet;
	int j = 0;
	INT16U nmsgs;
	OS_Q_DATA *pdata;

	data = data;

	for(;;)
	{
		j = 0;
		OSSemPend(SemVerification, 0, &err); /* s�ex�cutera p�riodiquement � toutes les cinq secondes gr�ce � une synchronisation avec l�interruption du deuxi�me fit_timer. */
		OSQQuery(ptrFifoAuxilary, pdata);
		nmsgs = pdata->OSNMsgs;
		while(j < nmsgs)
		{
			msg = OSQPend(ptrFifoAuxilary, 0, &err);
			packet = (Packet *)msg;
			switch(packet->type)
			{
				case High:
				err = OSQPost(ptrFifoVideo, packet);
				if (err == OS_ERR_Q_FULL)
				{
					xil_printf("\n--TaskVerification: Fifo high pleine (Paquet rejete) (total : %d) !\n", ++nbPacketHighRejete);
					free(packet); packet = 0;
				}
				break;

				case Medium:
				err = OSQPost(ptrFifoAudio, packet);
				if (err == OS_ERR_Q_FULL)
				{
					xil_printf("\n--TaskVerification: Fifo medium pleine (Paquet rejete) (total : %d) !\n", ++nbPacketMediumRejete);
					free(packet); packet = 0;
				}
				break;

				case Low:
				err = OSQPost(ptrFifoOtherwise, packet);
				if (err == OS_ERR_Q_FULL)
				{
					xil_printf("\n--TaskVerification: Fifo low pleine (Paquet rejete) (total : %d) !\n", ++nbPacketLowRejete);
					free(packet); packet = 0;
				}
				break;

				default:
					break;
			}


			if (packet != 0)
			{
				OS_ENTER_CRITICAL();
				xil_printf("\n********Remise du Paquet # %d ******** \n", ++i);
				OS_EXIT_CRITICAL();
				xil_printf("	** src : 0x%8x \n", packet->src);
				xil_printf("	** dst : 0x%8x \n", packet->dst);
			}
			++j;
		}
	}
}


/*
*********************************************************************************************************
*                                              TaskPrint
*********************************************************************************************************
*/
void TaskPrint(void *data)
{
	INT8U err;
	void* msg;
	Packet *packet;
	unsigned int interfaceID;

	data = data;

	for (;;)
	{
		OSSemPend(SemPrint, 0, &err);
		msg = OSMboxPend(printRequest.Mbox, 0, &err);
		if(msg != (void *)0)
		{
			interfaceID = printRequest.interfaceID;
			packet = (Packet*)msg;;
			xil_printf("\nPaquet recu en %d \n", interfaceID);
			xil_printf("	>> src : 0x%8x \n", packet->src);
			xil_printf("	>> dst : 0x%8x \n", packet->dst);
			xil_printf("	>> type : %d \n", packet->type);
			free(packet); packet = 0;
		}
	}
}


/*
*********************************************************************************************************
*                                              _CreateTask
*********************************************************************************************************
*/
void _CreateTask()
{
	OSTaskCreateExt(
			TaskVerification,
			(void *)0,									// Utilis� pour passer des param�tres � la t�che au d�but de l'ex�cution.
			&TaskVerificationStk[TASK_STK_SIZE-1], 		// Pointeur au top du stack
			(INT8U)(TASK_VERIFICATION_PRIO),			// La priorit� unique de la t�che
			(INT16U)(TASK_VERIFICATION_ID),				// Le num�ro d'identification de la t�che
			&TaskVerificationStk[0],					// Pointeur au bottom du stack
			TASK_STK_SIZE,								// Taille de la pile
			(void *)0,									// TCB Extension
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR 	// Option Stack checking enabled
			);

	OSTaskCreateExt(
			TaskStop,
			(void *)0,
			&TaskStopStk[TASK_STK_SIZE-1],
			(INT8U)(TASK_STOP_PRIO),
			(INT16U)(TASK_STOP_ID),
			&TaskStopStk[0],
			TASK_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
			);

	OSTaskCreateExt(
			TaskInjectPacket,
			(void *)0,
			&TaskInjectPacketStk[TASK_STK_SIZE-1],
			(INT8U)(TASK_INJECTPACKET_PRIO),
			(INT16U)(TASK_INJECTPACKET_ID),
			&TaskInjectPacketStk[0],
			TASK_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
		);

	OSTaskCreateExt(
			TaskComputing,
			(void *)0,
			&TaskComputingStk[TASK_STK_SIZE-1],
			(INT8U)(TASK_COMPUTING_PRIO),
			(INT16U)(TASK_COMPUTING_ID),
			&TaskComputingStk[0],
			TASK_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
		);

	OSTaskCreateExt(
			TaskForwarding,
			(void *)0,
			&TaskForwardingStk[TASK_STK_SIZE-1],
			(INT8U)(TASK_FOWARDING_PRIO),
			(INT16U)(TASK_FORWARDING_ID),
			&TaskForwardingStk[0],
			TASK_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
		);

	OSTaskCreateExt(
			TaskPrint,
			(void *)0,
			&TaskPrintStk[TASK_STK_SIZE-1],
			(INT8U)(TASK_PRINT_PRIO),
			(INT16U)(TASK_PRINT_ID),
			&TaskPrintStk[0],
			TASK_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
			);
}


/*
*********************************************************************************************************
*                                              _DeleteTask()
*********************************************************************************************************
*/
void _DeleteTask()
{
	INT8U err;
	err = OSTaskDel(TASK_INJECTPACKET_PRIO);
	err = OSTaskDel(TASK_PRINT_PRIO);
	err = OSTaskDel(TASK_COMPUTING_PRIO);
	err = OSTaskDel(TASK_FOWARDING_PRIO);
	err = OSTaskDel(TASK_VERIFICATION_PRIO);
	err = OSTaskDel(TASK_STOP_PRIO);
}

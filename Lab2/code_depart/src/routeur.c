/*
*********************************************************************************************************
*                                                 uC/OS-II
*                                          The Real-Time Kernel
*
*
*
*            					École Polytechnique de Montréal, Qc, CANADA
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

#define	     	 UBYTE		unsigned char

#define          TASK_INJECTPACKET_ID  	2                	// Task IDs
#define          TASK_COMPUTING_ID     	7
#define          TASK_FORWARDING1_ID    8
#define          TASK_VERIFICATION_ID    9
#define          TASK_STOP_ID    1
#define          TASK_PRINT1_ID		3
#define          TASK_PRINT2_ID   	4
#define          TASK_PRINT3_ID 	5
#define          TASK_PRINT4_ID  	6


#define MUTEX_SOURCE_REJETE_PRIO		9					// Task Prio
#define MUTEX_BAD_CRC_PRIO				10
#define MUTEX_PAQUET_REJETE_PRIO		11
/*
		À compléter
*/

// Routing info.
#define INT1_LOW  0
#define INT1_HIGH 1073741823 //3FFFFFF
#define INT2_LOW  1073741824 //4000000
#define INT2_HIGH 2147483647 //7FFFFFF
#define INT3_LOW  2147483648//80000000
#define INT3_HIGH 3221225472//C0000000

// Reject source info.
#define REJECT_LOW1  0x10000000
#define REJECT_HIGH1 0x100FFFFF
#define REJECT_LOW2  0x50000000
#define REJECT_HIGH2 0x500FFFFF
#define REJECT_LOW3  0x60000000
#define REJECT_HIGH3 0x600FFFFF
#define REJECT_LOW4  0xD0000000
#define REJECT_HIGH4 0xD00FFFFF

#define INTERFACE_LOW1		0
#define INTERFACE_HIGH1		1073741823	
#define INTERFACE_LOW2		1073741824
#define INTERFACE_HIGH2		2147483647
#define INTERFACE_LOW3		2147483648
#define INTERFACE_HIGH3		3221225472
#define INTERFACE_LOW4		3221225473
#define INTERFACE_HIGH4		4294967295

#define FIFO_IN_NUM_MSG 	16
#define FIFO_QOS_NUM_MSG 	4

#define BAD_CRC 1234

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


/*
		À compléter
*/
int nbPacket = 0;					// Nb de packets total traites
int nbPacketLowRejete = 0;			// Nb de packets low rejetes
int nbPacketMediumRejete = 0;       // Nb de packets Medium rejetes
int nbPacketHighRejete = 0;         // Nb de packets High rejetes
int nbPacketCRCRejete = 0 ;			// Nb de packets rejetes pour mauvais crc
int nbPacketSourceRejete = 0;		// Nb de packets rejetes pour mauvaise source
INT8U i = 0;

// Print function parameters
typedef struct
{
  unsigned int interfaceID;
  OS_EVENT *Mbox;
} PRINT_PARAM;


OS_EVENT *ptrFifoIn;
void *FifoInMsgTbl[FIFO_IN_NUM_MSG];

OS_EVENT *ptrFifoVideo, *ptrFifoAudio, *ptrFifoOtherwise, ptrFifoAuxilary;
void *FifoInMsgTbl[FIFO_IN_NUM_MSG];
void *FifoVideoMsgTbl[FIFO_QOS_NUM_MSG];
void *FifoAudioMsgTbl[FIFO_QOS_NUM_MSG];
void *FifoOtherwiseMsgTbl[FIFO_QOS_NUM_MSG];
void *FifoAuxilaryMsgTbl[FIFO_QOS_NUM_MSG];

OS_EVENT *SemStop, SemVerification;
OS_EVENT *MutexSourceRejete, * MutexBadCrc, *MutexPacketRejete;
OS_EVENT *PacketMbox;
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void 			 initLED();
void             TaskInjectPacket(void *data);               // Function prototypes of tasks
void             TaskComputing(void *data);
void             TaskForwarding(void *data);
void             TaskPrint(void *data);
void  			 TaskVerification (void *pdata)
void             TaskStop(void *data);

/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

int main (void)
{

/*
		À compléter
*/
	INT8U err;
	BSP_IntDisAll();
	
	OSInit(); /* Initialize µC/OS-II */
	
	/* Create Semaphore */ 
	SemStop 		= OSSemCreate(0);
	SemVerification = OSSemCreate(0); 

	ptrFifoIn 			= OSQCreate((Packet*)FifoInMsgTbl, 			FIFO_IN_NUM_MSG);
	
	ptrFifoVideo 		= OSQCreate((Packet*)FifoVideoMsgTbl, 		FIFO_QOS_NUM_MSG);
	ptrFifoAudio 		= OSQCreate((Packet*)FifoAudioMsgTbl, 		FIFO_QOS_NUM_MSG);
	ptrFifoOtherwise 	= OSQCreate((Packet*)FifoOtherwiseMsgTbl, 	FIFO_QOS_NUM_MSG);
	ptrFifoAuxilary 	= OSQCreate((Packet*)FifoAuxilaryMsgTbl, 	FIFO_QOS_NUM_MSG);
	
	PacketMbox = OSMboxCreate((void *)0); /* Create mailbox */
	
	MutexSourceRejete 	= OSMutexCreate(MUTEX_SOURCE_REJETE_PRIO, &err);
	MutexBadCrc 		= OSMutexCreate(MUTEX_BAD_CRC_PRIO, &err);
	MutexPacketRejete 	= OSMutexCreate(MUTEX_PAQUET_REJETE_PRIO, &err);
/*
		À compléter
*/
   
    BSP_InitIO();
	
/*
		À compléter
*/  

	OSStart(); /* Start Multitasking */ 
	
    return 1;
}

/*
*********************************************************************************************************
*                                              fitTimerHandler
* Ajoutez les xil_printf dans le code de la tâche aux bons endroits
*xil_printf("\nVerification completee %d!\n", i);
*
*********************************************************************************************************
*/
void  fitTimerHandler (void* par) {
/*
		À compléter
*/  
}

/*
*********************************************************************************************************
*                                              fitTimer2Handler
* Ajoutez les xil_printf dans le code de la tâche aux bons endroits
*xil_printf("\nLancement tache verification %d!\n", i);
*
*********************************************************************************************************
*/
void  fitTimer2Handler (void* par) {
/*
		À compléter
*/  
}

/*
*********************************************************************************************************
*                                              initLED
*********************************************************************************************************
*/
void initLED()
{
/*
		À compléter
*/  

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
*Ajoutez les xil_printf dans le code de la tâche aux bons endroits
*xil_printf("ERREUR : Trop de CRC invalide !\n");
*
*********************************************************************************************************
*/
void  TaskStop (void *data)
{
	INT8U err;
	
	OSSemPend(SemStop, 0, &err);
	xil_printf("ERREUR : Trop de CRC invalide !\n");
	
	OSTaskDelReq(TASK_BIDON_PRIO); /* TaskInjectPacket */
	OSTaskDelReq(TASK_BIDON_PRIO); /* TaskComputing */
	OSTaskDelReq(TASK_BIDON_PRIO); /* TaskForwarding */
	OSTaskDelReq(TASK_BIDON_PRIO); /* TaskPrint */
	OSTaskDelReq(TASK_BIDON_PRIO); /* TaskVerification */
}
/*
*********************************************************************************************************
*                                              TaskInjectPacket
*********************************************************************************************************
*/
void  TaskInjectPacket (void *data)
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
*Ajoutez les xil_printf dans le code de la tâche aux bons endroits
*xil_printf("\n--TaskComputing: Source invalide (Paquet rejete) (total : %d)\n\n", ++nbPacketSourceRejete);
*xil_printf("\n--TaskComputing: CRC invalide (Paquet rejete) (total : %d)\n", ++nbPacketCRCRejete);
*xil_printf("\n--TaskComputing: Fifo high pleine (Paquet rejete) (total : %d) !\n", ++nbPacketHighRejete);
*xil_printf("\n--TaskComputing: Fifo medium pleine (Paquet rejete) (total : %d) !\n", ++nbPacketMediumRejete);
*xil_printf("\n--TaskComputing: Fifo low pleine (Paquet rejete) (total : %d) !\n", ++nbPacketLowRejete);
*														
*********************************************************************************************************
*/
void  TaskComputing (void *pdata)
{
  INT8U err;
  void* msg;
  Packet *packet;
  bool bRejectInterval, bRejectInterval1, bRejectInterval2, bRejectInterval3, bRejectInterval4;
  
  for(;;) 
  {
	  msg = OSQPend(ptrFifoIn, 0, &err);
	  packet = (Packet *)msg;
	  
	  /* rejeter les paquets qui ne sont pas dans son espace d’adressage */
	  bRejectInterval1 	= packet->src > REJECT_LOW1 && msg < REJECT_HIGH1;
	  bRejectInterval2 	= packet->src > REJECT_LOW2 && msg < REJECT_HIGH2;
	  bRejectInterval3 	= packet->src > REJECT_LOW3 && msg < REJECT_HIGH3;
	  bRejectInterval4 	= packet->src > REJECT_LOW4 && msg < REJECT_HIGH4;
	  
	  bRejectInterval 	= bRejectInterval1 || bRejectInterval2 || bRejectInterval3 || bRejectInterval4;
	  
	  /* tous les paquets ayant une adresse source contenue entre REJECT_LOW et REJECT_HIGH de chaque plage pour les interfaces devront être rejetés */
	  if(bRejectInterval)
	  {
		OSMutexPend(MutexSourceRejete,0, &err);
		xil_printf("\n--TaskComputing: Source invalide (Paquet rejete) (total : %d)\n\n", ++nbPacketSourceRejete);
		delete packet; packet = 0;
		OSMutexPost(MutexSourceRejete);
	  }
	  else 
	  {
		/* Les paquets contenant des erreurs sont simplement rejetés*/
		if(packet->crc == BAD_CRC)
		{
			OSMutexPend(MutexBadCrc,0, &err);
			xil_printf("\n--TaskComputing: CRC invalide (Paquet rejete) (total : %d)\n", ++nbPacketCRCRejete);
			delete packet; packet = 0;
			OSMutexPost(MutexBadCrc);
		}
		else 
		{
			switch(packet->type)
			{
				case High:
					err = OSQPost(ptrFifoVideo, packet);
					if (err == OS_ERR_Q_FULL) {
						err = OSQPost(ptrFifoAuxilary, packet);
						if (err == OS_ERR_Q_FULL) {
							OSMutexPend(MutexPacketRejete,0, &err);
							xil_printf("\n--TaskComputing: Fifo high pleine (Paquet rejete) (total : %d) !\n", ++nbPacketHighRejete);
							delete packet; packet = 0;
							OSMutexPost(MutexPacketRejete);
						}
					}
					break;
					
				case Medium:
					err = OSQPost(ptrFifoAudio, packet);
					if (err == OS_ERR_Q_FULL) {
						err = OSQPost(ptrFifoAuxilary, packet);
						if (err == OS_ERR_Q_FULL) {
							OSMutexPend(MutexPacketRejete,0, &err);
							xil_printf("\n--TaskComputing: Fifo medium pleine (Paquet rejete) (total : %d) !\n", ++nbPacketMediumRejete);
							delete packet; packet = 0;
							OSMutexPost(MutexPacketRejete);
						}
					}
					break;
					
				case Low:
					err = OSQPost(ptrFifoOtherwise, packet);
					if (err == OS_ERR_Q_FULL) {
						err = OSQPost(ptrFifoAuxilary, packet);
						if (err == OS_ERR_Q_FULL) {
							OSMutexPend(MutexPacketRejete,0, &err);
							xil_printf("\n--TaskComputing: Fifo low pleine (Paquet rejete) (total : %d) !\n", ++nbPacketLowRejete);
							delete packet; packet = 0;
							OSMutexPost(MutexPacketRejete);
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
*Ajoutez les xil_printf dans le code de la tâche aux bons endroits
*xil_printf("\n--TaskForwarding: %d paquets envoyes\n\n", ++nbPacket);
*********************************************************************************************************
*/
void  TaskForwarding (void *pdata)
{
  INT8U err;
  void* msg;
  Packet *packet;
  bool bIntervalValid, bIntervalValid1, bIntervalValid2, bIntervalValid3, bIntervalValid4;
  
  for(;;)
  {
	if((msg = OSQAccept(ptrFifoVideo, &err))) {}
	else if((msg = OSQAccept(ptrFifoAudio, &err))) {}
	else if((msg = OSQAccept(ptrFifoOtherwise, &err))) {}

	if(packet = (Packet *)msg)
	{
		bRejectInterval1 	= packet->src > INTERFACE_LOW1 && msg < INTERFACE_HIGH1;
		bRejectInterval2 	= packet->src > INTERFACE_LOW2 && msg < INTERFACE_HIGH1;
		bRejectInterval3 	= packet->src > INTERFACE_LOW3 && msg < INTERFACE_HIGH1;
		bRejectInterval4 	= packet->src > INTERFACE_LOW4 && msg < INTERFACE_HIGH1;
		
		/* simulation de la table de routage */
		OSTimeDly(OS_TICKS_PER_SEC); 
		
		bIntervalValid = bRejectInterval1 || bRejectInterval2 || bRejectInterval3 || bRejectInterval4;
		
		if(bIntervalValid)
		{
			
		}
	}
  }
}

/*
*********************************************************************************************************
*                                              TaskVerification
*Ajoutez les xil_printf dans le code de la tâche aux bons endroits
*		xil_printf("\n********Remise du Paquet # %d ******** \n", ++i);
		xil_printf("	** src : 0x%8x \n", packet->src);
		xil_printf("	** dst : 0x%8x \n", packet->dst);
*********************************************************************************************************
*/
void  TaskVerification (void *pdata)
{
	  INT8U err;
	  void* msg;
	  Packet *packet;
	  
	  data = data;
	  
	  for(;;)
	  {
		OSPend(SemVerification, 0, &err); /* s’exécutera périodiquement à toutes les cinq secondes grâce à une synchronisation avec l’interruption du deuxième fit_timer. */
		msg = OSQPend(ptrFifoAuxilary, 0, &err);
		packet = (Packet *)msg;
		switch(packet->type)
		{
			case High:
			err = OSQPost(ptrFifoVideo, packet);
			if (err == OS_ERR_Q_FULL)
			{
				OSMutexPend(MutexPacketRejete,0, &err);
				xil_printf("\n--TaskComputing: Fifo high pleine (Paquet rejete) (total : %d) !\n", ++nbPacketHighRejete);
				delete packet; packet = 0;
				OSMutexPost(MutexPacketRejete);
			}
			break;
					
			case Medium:
			err = OSQPost(ptrFifoAudio, packet);
			if (err == OS_ERR_Q_FULL)
			{
				OSMutexPend(MutexPacketRejete, 0, &err);
				xil_printf("\n--TaskComputing: Fifo medium pleine (Paquet rejete) (total : %d) !\n", ++nbPacketMediumRejete);
				delete packet; packet = 0;
				OSMutexPost(MutexPacketRejete);
			}
			break;
					
			case Low:
			err = OSQPost(ptrFifoOtherwise, packet);
			if (err == OS_ERR_Q_FULL)
			{
				OSMutexPend(MutexPacketRejete,0, &err);
				xil_printf("\n--TaskComputing: Fifo low pleine (Paquet rejete) (total : %d) !\n", ++nbPacketLowRejete);
				delete packet; packet = 0;
				OSMutexPost(MutexPacketRejete);
			}
			break;
					
			default:
				break;
		}
			
		if (err == OS_NO_ERR) 
		{
			xil_printf("\n********Remise du Paquet # %d ******** \n", ++i);
			xil_printf("	** src : 0x%8x \n", packet->src);
			xil_printf("	** dst : 0x%8x \n", packet->dst);
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

  data = data;
  
  for (;;)
  { 
	msg = OSMboxPend(PacketMbox, 0, &err); /* Check mailbox for a message */
	if (msg != (void *)0) { /* Message received, process */
		packet = (Packet *)msg;
		xil_printf("\nPaquet recu en %d \n", intID);
        xil_printf("	>> src : 0x%8x \n", packet->src);
        xil_printf("	>> dst : 0x%8x \n", packet->dst);
        xil_printf("	>> type : %d \n", packet->type);
	}
	}    	
}


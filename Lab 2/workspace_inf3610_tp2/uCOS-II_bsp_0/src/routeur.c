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


															// Task Prio
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
OS_EVENT *ptrHighIn;
OS_EVENT *ptrMediumIn;
OS_EVENT *ptrLowIn;
OS_EVENT *ptrVerificationIn;
XGpio instancePtrLED;

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
void			 PacketDelete(Packet *packet);

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
	BSP_IntDisAll();
	
/*
		À compléter
*/
   
    BSP_InitIO();
	
/*
		À compléter
*/  

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
	XGpio_Initialize( &instancePtrLED, XPAR_LEDS_8BIT_DEVICE_ID);
	XGpio_SetDataDirection(&instancePtrLED, 1,0x0);

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
/*
		À compléter
*/  

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
  
  for (;;) {

	  msg = OSQPend(ptrFifoIn, 0, &err);
	  if (err == OS_NO_ERR) {
		  *xil_printf("\n--TaskComputing: Source reçu\n", ++nbPacketSourceRejete);
		  packet = (Packet *) msg;
		  if (packet->src => REJECT_LOW1 || packet->src <= REJECT_HIGH4){

			  /*dautres trucs)*/
			  unsigned int answer = computeCRC( packet->data, 4 );  //AUCUNE IDEE COMMENT CA FONCTIONNE
			  if ( packet->crc != answer ){
				  xil_printf("\n--TaskComputing: CRC invalide (Paquet rejete) (total : %d)\n", ++nbPacketCRCRejete);
			  }
			  else if( packet->type == 1 ){
				  err = OSQPost(ptrHighIn, packet);
				  if (err == OS_ERR_Q_FULL) {
					  xil_printf("\n--TaskComputing: Fifo high pleine (Paquet rejete) (total : %d) !\n", ++nbPacketHighRejete);
					  err = OSQPost(ptrVerificationIn, packet);
					  if (err == OS_ERR_Q_FULL) {
						  xil_printf("\n--TaskComputing: Fifo verification pleine (Paquet efface) !\n");
						  	 PacketDelete();
					  }
				  }
			  }
			  else if( packet->type == 2 ){
				  err = OSQPost(ptrMediumIn, packet);
				  if (err == OS_ERR_Q_FULL) {
					  xil_printf("\n--TaskComputing: Fifo medium pleine (Paquet rejete) (total : %d) !\n", ++nbPacketMediumRejete);
					  err = OSQPost(ptrVerificationIn, packet);
					  if (err == OS_ERR_Q_FULL) {
						  xil_printf("\n--TaskComputing: Fifo verification pleine (Paquet efface) !\n");
							 PacketDelete();
					  }
				  }
			  }
			  else if( packet->type == 3 ){
				  err = OSQPost(ptrLowIn, packet);
				  if (err == OS_ERR_Q_FULL) {
					  xil_printf("\n--TaskComputing: Fifo low pleine (Paquet rejete) (total : %d) !\n", ++nbPacketLowRejete);
					  err = OSQPost(ptrVerificationIn, packet);
					  if (err == OS_ERR_Q_FULL) {
						  xil_printf("\n--TaskComputing: Fifo verification pleine (Paquet efface) !\n");
							 PacketDelete();
					  }
				  }
			  }

		  }

	  } else {
		  *xil_printf("\n--TaskComputing: Erreur\n", ++nbPacketSourceRejete);
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
  
/*
		À compléter
*/ 

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
	  
	/*
			À compléter
	*/ 
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

/*
		À compléter
*/ 
 
        	xil_printf("\nPaquet recu en %d \n", intID);
        	xil_printf("	>> src : 0x%8x \n", packet->src);
        	xil_printf("	>> dst : 0x%8x \n", packet->dst);
        	xil_printf("	>> type : %d \n", packet->type);
/*
		À compléter
*/ 

}

/*
*********************************************************************************************************
*                                              PacketDelete
*********************************************************************************************************
*/
void PacketDelete(Packet *packet){

	//A completer

}

/*
*********************************************************************************************************
*                                                 uC/OS-II
*                                          The Real-Time Kernel
*                                                 PORT LINUX
*
*
*            Jerome CHEVALIER, Ecole Polytechnique de Montreal, Qc, CANADA
*                                                       09/2001
*
* File : tp.C
* By   : Jerome CHEVALIER	jerome.chevalier@polymtl.ca
*********************************************************************************************************
*/

#include "includes.h"
/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define                 TASK_STK_SIZE    8192                // Size of each task's stacks (# of WORDs)
#define                 N                 256

#define	     UBYTE		unsigned char

#define          TASK_CTRL_ID     	0                // Application tasks IDs
#define          TASK_CONS1_ID     	1
#define          TASK_CONS2_ID     	2
#define          TASK_PRODFIFO_ID	3
#define          TASK_PRODMBX_ID   	4
#define          TASK_DRIVFIFO_ID 	5
#define          TASK_DRIVMBX_ID  	6
#define          TASK_DEVFIFO_ID  	7
#define          TASK_DEVMBX_ID   	8

#define          TASK_CTRL_PRIO    		10                // Application tasks priorities
#define          TASK_CONS1_PRIO    	18
#define          TASK_CONS2_PRIO    	17
#define          TASK_PRODFIFO_PRIO 	14
#define          TASK_PRODMBX_PRIO  	13
#define          TASK_DRIVFIFO_PRIO	    16
#define          TASK_DRIVMBX_PRIO  	15
#define          TASK_DEVFIFO_PRIO  	11
#define          TASK_DEVMBX_PRIO  	    12

#define          MSG_QUEUE_SIZE     256                // Size of message queue used in example


typedef struct {
	unsigned short value;
	unsigned char device;
}msg;

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK           TaskCtrlStk[TASK_STK_SIZE];     	// Startup    task stack
OS_STK           TaskCons1Stk[TASK_STK_SIZE];        	// Task #1    task stack
OS_STK           TaskCons2Stk[TASK_STK_SIZE];       	// Task #2    task stack
OS_STK           TaskProdFifoStk[TASK_STK_SIZE];   	// Task #3    task stack
OS_STK           TaskProdMbxStk[TASK_STK_SIZE];  	// Task #4    task stack
OS_STK           TaskDrivFifoStk[TASK_STK_SIZE];  	// Task #5    task stack
OS_STK           TaskDrivMbxStk[TASK_STK_SIZE];    	// Task #6    task stack
OS_STK           TaskDevFifoStk[TASK_STK_SIZE];  	// Task #7    task stack
OS_STK           TaskDevMbxStk[TASK_STK_SIZE];    	// Task #8    task stack

  /* This disables all code until the next "#endif" */
OS_EVENT        	*Fifo;                            // Message queue pointer
msg            	FifoTbl[ MSG_QUEUE_SIZE];                     // Storage for messages
OS_EVENT        	*Mbox;                             // Message mailboxes for Tasks #4 and #6
msg 		Mbx;
OS_EVENT		*Mutex;
OS_EVENT		*SemFull;
OS_EVENT		*SemEmpty;
OS_EVENT		*Irq1;
OS_EVENT		*Irq2;


msg buffer[N];
int add = 0, rem = 0;
unsigned short reg1, reg2;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void             TaskCtrl(void *data);               // Function prototypes of tasks
void             Cons1(void *data);
void             Cons2(void *data);
void             ProdFifo(void *data);
void             ProdMbx(void *data);
void             DrivFifo(void *data);
void             DrivMbx(void *data);
void             DevFifo(void *data);
void             DevMbx(void *data);

/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

int main (void)
{
     UBYTE err;

	OSInit();                                              // InitializeuC/OS-II




    Mutex = OSMutexCreate(9, &err);
    SemFull = OSSemCreate(0);
    SemEmpty = OSSemCreate(N);
    Irq1 = OSSemCreate(0);
    Irq2 = OSSemCreate(0);
    Mbox = OSMboxCreate(&Mbx,sizeof(msg) );                 // Create message mailboxes
    Fifo = OSQCreate(&FifoTbl[0], MSG_QUEUE_SIZE, sizeof(msg)); // Create a message queue


	OSTaskCreateExt(TaskCtrl, (void *)0, &TaskCtrlStk[TASK_STK_SIZE-1], TASK_CTRL_PRIO,
                   TASK_CTRL_ID, &TaskCtrlStk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Cons1, (void *)0, &TaskCons1Stk[TASK_STK_SIZE-1], TASK_CONS1_PRIO,
                   TASK_CONS1_ID, &TaskCons1Stk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Cons2, (void *)0, &TaskCons2Stk[TASK_STK_SIZE-1], TASK_CONS2_PRIO,
                   TASK_CONS2_ID, &TaskCons2Stk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(ProdFifo, (void *)0, &TaskProdFifoStk[TASK_STK_SIZE-1], TASK_PRODFIFO_PRIO,
                   TASK_PRODFIFO_ID, &TaskProdFifoStk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(ProdMbx, (void *)0, &TaskProdMbxStk[TASK_STK_SIZE-1], TASK_PRODMBX_PRIO,
                   TASK_PRODMBX_ID, &TaskProdMbxStk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(DrivFifo, (void *)0, &TaskDrivFifoStk[TASK_STK_SIZE-1], TASK_DRIVFIFO_PRIO,
                   TASK_DRIVFIFO_ID, &TaskDrivFifoStk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(DrivMbx, (void *)0, &TaskDrivMbxStk[TASK_STK_SIZE-1], TASK_DRIVMBX_PRIO,
                   TASK_DRIVMBX_ID, &TaskDrivMbxStk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(DevFifo, (void *)0, &TaskDevFifoStk[TASK_STK_SIZE-1], TASK_DEVFIFO_PRIO,
                   TASK_DEVFIFO_ID, &TaskDevFifoStk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(DevMbx, (void *)0, &TaskDevMbxStk[TASK_STK_SIZE-1], TASK_DEVMBX_PRIO,
                   TASK_DEVMBX_ID, &TaskDevMbxStk[0], TASK_STK_SIZE, (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSStart();
    return 1;                                         // Start multitasking
}

/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

void  TaskCtrl (void *data)
{

     int i=0;



    for (;;) {


        printf("******** Task now at: %d ******** \n",i++);
		if(i>10)
			OSuCStop();

       
        
        OSTimeDly(100);                       // Wait one second
   }
}


  /* This disables all code until the next "#endif" */
void  Cons1 (void *pdata)
{
	UBYTE err;
	msg out;
   	pdata = pdata;
	while(1){
        	OSSemPend(SemFull, 0, &err);           // Acquire semaphore
		OSMutexPend(Mutex, 0, &err);
			out = buffer[rem++];//memcpy
			if(rem>=N)
				rem =0;
        		OSMutexPost(Mutex);                    // Release semaphore
        	OSSemPost(SemEmpty);
	printf("Cons1 Value = %d Device:  %d\n",out.value, out.device);
	OSTimeDlyHMSM(0, 0, 0,30);
		//Cons out
	}
}

void  Cons2 (void *pdata)
{
	UBYTE err;
	msg out;
   	pdata = pdata;
	while(1){
        	OSSemPend(SemFull, 0, &err);           // Acquire semaphore
		OSMutexPend(Mutex, 0, &err);
 			out = buffer[rem++];		//memcpy
			if(rem>=N)
				rem =0;
        		OSMutexPost(Mutex);                    // Release semaphore
 	OSSemPost(SemEmpty);
	printf("Cons2 Value = %d Device:  %d\n",out.value, out.device);
	OSTimeDlyHMSM(0, 0, 0, 20);
		//Cons out
	}
}

void  ProdFifo (void *data)
{
    	UBYTE err;
	msg in;
	data = data;

	while(1){

		 OSQPend(Fifo, (void*)&in, 0);    // Wait forever for message
		 printf("ProdFifo Value = %d Device:  %d\n",in.value, in.device);
		OSSemPend(SemEmpty, 0, &err);		// Acquire semaphore
		OSSemPend(Mutex, 0, &err);
			buffer[add++] = in;	//memcpy
			if(add>=N)
				add =0;
   		OSSemPost(Mutex);                    //Release semaphore
		OSSemPost(SemFull);
	}
}

void  ProdMbx (void *data)
{
	UBYTE err;
	msg in;
	data = data;
	while(1){
		OSMboxPend(Mbox, (void*)&in, 0);  // Wait for message
		 printf("ProdMBx  Value = %d Device:  %d\n",in.value, in.device);
		OSSemPend(SemEmpty, 0, &err);		// Acquire semaphore
  		OSSemPend(Mutex, 0, &err);
			buffer[add++] = in;//memcpy
			if(add>=N)
				add =0;
        		OSSemPost(Mutex);                    // Release semaphore
 		OSSemPost(SemFull);
	}
}


void  DrivFifo (void *data)
{
	msg dat;
	UBYTE err;
	data = data;
	dat.device = 1;
	while(1){
		OSSemPend(Irq1, 0, &err);
		dat.value = reg1;
		OSQPost(Fifo, (void*)&dat);
	}
}

void  DrivMbx (void *data)
{
	msg dat;
	UBYTE err;
	data = data;
	dat.device = 2;
	while(1){
		OSSemPend(Irq2, 0, &err);
		dat.value = reg2;
		OSMboxPost(Mbox, (void *)&dat);  // Send message
	}
}


void  DevFifo (void *data)
{

	 int i;
	unsigned short value = 0;
	data = data;
	while(1){
 		for(i=0; i<100; i++) {
			OSTimeDlyHMSM(0, 0, 0, 10);        // Delay for 10 millisecond
 			OSSemAccept(Irq1);
			reg1 = value++;
			OSSemPost(Irq1);
		}
		 OSTimeDlyHMSM(0, 0, 1,400);

	}
}

void  DevMbx (void *data)
{

	int i;
	unsigned short value = 0;
	data = data;
	while(1){
		for(i=0; i<100; i++) {
        	OSTimeDlyHMSM(0, 0, 0, 10);        // Delay for 10 millisecond
			OSSemAccept(Irq2);
			reg2 = value++;
			OSSemPost(Irq2);
		}
		OSTimeDlyHMSM(0, 0, 1, 400);
	}
}



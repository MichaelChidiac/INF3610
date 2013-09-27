/*
*********************************************************************************************************
*                                                 uC/OS-II
*                                          The Real-Time Kernel
*                                                 PORT Windows
*
*
*            			Guy BOIS, Ecole Polytechnique de Montreal, Qc, CANADA
*                                                       09/2003
*
* File : tp.C
* 
*********************************************************************************************************
*/

#include "includes.h"
/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/
#define                 TASK_STK_SIZE    16384            // Size of each task's stacks (# of WORDs)

#define			UBYTE		unsigned char

#define TASK1_PRIO  8    // Priorité de Task1 à compléter
#define TASK2_PRIO  9    // Priorité de Task2 à compléter
#define TASK3_PRIO  10    // Priorité de Task3 à compléter

#define TASK1_ID 0     // ID de Task1 à compléter
#define TASK2_ID 1     // ID de Task2 à compléter
#define TASK3_ID 2     // ID de Task3 à compléter

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK           Task1Stk[TASK_STK_SIZE];	
OS_STK           Task2Stk[TASK_STK_SIZE];
OS_STK           Task3Stk[TASK_STK_SIZE];	

//Déclarations Mutexs et Semaphores
OS_EVENT		*R1;
OS_EVENT		*R2;
OS_EVENT		*Sem1;
OS_EVENT		*Sem2;
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void    Task1(void *data);
void    Task2(void *data);
void    Task3(void *data);

/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

int main(void)
{
	UBYTE err;
	OSInit();

	printf("Main execute\n");
	R1 = OSMutexCreate(6, &err);
	R2 = OSMutexCreate(7, &err);
	Sem1 = OSSemCreate(0);
	Sem2 = OSSemCreate(0);

	OSTaskCreateExt(Task3, (void *)0, &Task3Stk[TASK_STK_SIZE-1], (INT8U)(TASK3_PRIO),
                   (INT16U)(TASK3_ID), &Task3Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskCreateExt(Task2, (void *)0, &Task2Stk[TASK_STK_SIZE-1], (INT8U)(TASK2_PRIO),
                   (INT16U)(TASK2_ID), &Task2Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskCreateExt(Task1, (void *)0, &Task1Stk[TASK_STK_SIZE-1], (INT8U)(TASK1_PRIO),
                   (INT16U)(TASK1_ID), &Task1Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	printf("Main end\n");
	OSStart();
    return 1;
}

/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

void Task1(void *data)
{
	UBYTE err;
	printf("Task1\n");
	OSSemPend(Sem2, 0, &err);
	printf("Task1 pend Sem2\n");

	OSMutexPend(R1, 0, &err);
	printf("Task1 pend R1\n");

	OSMutexPend(R2, 0, &err);
	printf("Task1 pend R2\n");

	OSMutexPost(R1);
	printf("Task1 post R1\n");

	OSMutexPost(R2);
	printf("Task1 post R2\n");
}

void Task2(void *data)
{	
	UBYTE err;

	OSSemPend(Sem1, 0, &err);
	printf("Task2 pend Sem1\n");

	OSSemPost(Sem2);
	printf("Task2 post Sem2\n");
}

void Task3(void *data)
{	
	UBYTE err;

	OSMutexPend(R2, 0, &err);
	printf("Task3 pend R2\n");

	OSSemPost(Sem1);
	printf("Task3 post Sem1\n");

	OSMutexPend(R1, 0, &err);
	printf("Task3 pend R1\n");

	OSMutexPost(R2);
	printf("Task3 post R2\n");

	OSMutexPost(R1);
	printf("Task3 post R1\n");
}


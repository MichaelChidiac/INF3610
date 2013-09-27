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
* File : tpPart2.C
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

//#define			UBYTE		unsigned char

#define TASK_PERIOD1_PRIO  10   // Priorité de TaskPeriodic à compléter
#define TASK_PERIOD2_PRIO  20   // Priorité de TaskPeriodic à compléter
#define TASK_PERIOD3_PRIO  30   // Priorité de TaskPeriodic à compléter
#define TASK_TIMER_PRIO	   40	// Priorité de TaskTimer à compléter

#define TASK_PERIOD1_ID    0	// Priorité de TaskPeriodic à compléter
#define TASK_PERIOD2_ID    1	// Priorité de TaskPeriodic à compléter
#define TASK_PERIOD3_ID    2	// Priorité de TaskPeriodic à compléter
#define TASK_TIMER_ID      3	// Priorité de TaskTimer à compléter

#define PERIOD			// Période de TaskPeriodic en ticks d’horloge
#define PERIOD_TASK_A  // Période pour la tâche A en ticks d’horloge


/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK      TaskPeriodic1Stk[TASK_STK_SIZE];	
OS_STK      TaskPeriodic2Stk[TASK_STK_SIZE];
OS_STK      TaskPeriodic3Stk[TASK_STK_SIZE];
OS_STK      TaskTimerStk[TASK_STK_SIZE];	

//Déclarations Mutexs et Semaphores
OS_EVENT	*SemStart;
OS_EVENT	*SemStop;
OS_EVENT	*Sem1Synchro;
OS_EVENT	*Sem2Synchro;
OS_EVENT	*MutexA;
OS_EVENT	*MutexB;
OS_EVENT	*R1;
OS_EVENT	*R2;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void    TaskPeriodic1(void *data);
void    TaskPeriodic2(void *data);
void    TaskPeriodic3(void *data);
void    TaskTimer(void *data);

/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main(void)
{
	UBYTE err;

	OSInit();

	printf("Main execute\n");
	R1 = OSMutexCreate(6, &err);
	R2 = OSMutexCreate(7, &err);
	Sem1Synchro = OSSemCreate(0);
	Sem2Synchro = OSSemCreate(0);
	SemStart = OSSemCreate(0);

	OSTaskCreateExt(TaskPeriodic1, (void *)0, &TaskPeriodic1Stk[TASK_STK_SIZE-1], (INT8U)(TASK_PERIOD1_PRIO),
                   (INT16U)(TASK_PERIOD1_ID), &TaskPeriodic1Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskCreateExt(TaskPeriodic2, (void *)0, &TaskPeriodic2Stk[TASK_STK_SIZE-1], (INT8U)(TASK_PERIOD2_PRIO),
                   (INT16U)(TASK_PERIOD2_ID), &TaskPeriodic2Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskCreateExt(TaskPeriodic3, (void *)0, &TaskPeriodic3Stk[TASK_STK_SIZE-1], (INT8U)(TASK_PERIOD3_PRIO),
                   (INT16U)(TASK_PERIOD3_ID), &TaskPeriodic3Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskCreateExt(TaskTimer, (void *)0, &TaskTimerStk[TASK_STK_SIZE-1], (INT8U)(TASK_TIMER_PRIO),
                   (INT16U)(TASK_TIMER_ID), &TaskTimerStk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	printf("Main end\n");
	OSStart();
	
    return;
}

/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

void TaskPeriodic1()
{
	UBYTE err;
	while(1)
	{
		OSMutexPend(R1, 0, &err);		//Section critique liée à R1
		printf("Task1 pend R1\n");
		OSSemPost(Sem1Synchro);
		printf("Task1 post Sem1Synchro\n");
		OSTimeDly(5);					//Attente non‐active de 5 ticks
		printf("Task1 attente non-active\n");
		OSMutexPend(R2, 0, &err);		//Section critique liée à R2
		printf("Task1 pend R2\n");
		OSMutexPost(R1);
		printf("Task1 post R1\n");
		OSMutexPost(R2);
		printf("Task1 post R2\n");
	}
}

void TaskPeriodic2()
{	
	UBYTE err;
	int period = 10;
	int tick_count = 0;
	int old_time;
	while(1)
	{
		OSSemPend(Sem1Synchro, 0, &err);
		printf("Task2 pend Sem1Synchro\n");
        old_time = OSTimeGet();
		tick_count = 0;
		while ( tick_count < period){
            tick_count = OSTimeGet() - old_time;
			printf("Attente Active Task2 %Lu \n", tick_count);
			OSTimeDly(1);
		}
		printf("Task2 Fin attente active\n");

		OSSemPost(Sem2Synchro);
		printf("Task2 post Sem2Synchro\n");
		period--;
		printf("period %i\n", period);
	}
}

void TaskPeriodic3()
{	
	UBYTE err;
	int period = 2;
	int tick_count = 0;
	int old_time;
	while(1)
	{
		OSSemPend(Sem2Synchro, 0, &err);
		printf("Task3 pend Sem2Synchro\n");
		OSMutexPend(R2, 0, &err);		//Section critique liée à R2
		printf("Task3 pend R2\n");
		OSMutexPend(R1, 0, &err);		//Section critique liée à R1
		printf("Task3 pend R1\n");
		old_time = OSTimeGet();
		tick_count = 0;

		while ( tick_count < period ){
            tick_count = OSTimeGet() - old_time;
			printf("Attente Active Task3 %Lu \n", tick_count);
			OSTimeDly(1);
		}

		OSMutexPost(R1);
		printf("Task3 post R1\n");
		OSMutexPost(R2);
		printf("Task3 post R2\n");
		OSSemPost(SemStop);
	}
}

void TaskTimer()
{
	UBYTE err;
	UBYTE err2;
	int period = 50;
	int tick_count = 0;
	int old_time;
	while(1)
	{
		old_time = OSTimeGet();
		tick_count = 0;
		while ( tick_count < period){
            tick_count = OSTimeGet() - old_time;
			OSTimeDly(1);
		}

		OSMutexPend(R1, 5, &err);
		OSMutexPend(R1, 5, &err2);
		if (err == OS_TIMEOUT || err2 == OS_TIMEOUT )
		{
			printf("Suppression de la tache bidon par la tache suppression\n");
			OSTaskDel(TASK_PERIOD1_PRIO);
			OSTaskDel(TASK_PERIOD2_PRIO);
			OSTaskDel(TASK_PERIOD3_PRIO);

			OSInit();
			
			OSMutexDel(R1, OS_DEL_ALWAYS, &err);
			OSMutexDel(R2, OS_DEL_ALWAYS, &err2);
			
			R1 = OSMutexCreate(6, &err);
			R2 = OSMutexCreate(7, &err);

			OSTaskCreateExt(TaskPeriodic1, (void *)0, &TaskPeriodic1Stk[TASK_STK_SIZE-1], (INT8U)(TASK_PERIOD1_PRIO),
                   (INT16U)(TASK_PERIOD1_ID), &TaskPeriodic1Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			OSTaskCreateExt(TaskPeriodic2, (void *)0, &TaskPeriodic2Stk[TASK_STK_SIZE-1], (INT8U)(TASK_PERIOD2_PRIO),
                   (INT16U)(TASK_PERIOD2_ID), &TaskPeriodic2Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			OSTaskCreateExt(TaskPeriodic3, (void *)0, &TaskPeriodic3Stk[TASK_STK_SIZE-1], (INT8U)(TASK_PERIOD3_PRIO),
                   (INT16U)(TASK_PERIOD3_ID), &TaskPeriodic3Stk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			OSTaskCreateExt(TaskTimer, (void *)0, &TaskTimerStk[TASK_STK_SIZE-1], (INT8U)(TASK_TIMER_PRIO),
                   (INT16U)(TASK_TIMER_ID), &TaskTimerStk[0], TASK_STK_SIZE, (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			
			OSStart();

		}
		else
		{
			printf("Tache suppression possede le mutex\n");
			OSMutexPost(R1);
			printf("Tache suppression relache le mutex\n");
		}
	}
}
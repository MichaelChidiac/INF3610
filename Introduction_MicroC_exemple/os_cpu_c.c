/*
*********************************************************************************************************
*                                                 uC/OS-II
*                                          The Real-Time Kernel
*                                      PORT LINUX CYGWIN and WINDOWS
*
*
*						Jerome CHEVALIER, Ecole Polytechnique de Montreal, Qc, CANADA
*                                                09/2001       
*
* File : OS_CPU_C.C
* By   : Jerome CHEVALIER	jerome.chevalier@polymtl.ca
* And the participation of :  Karim YAGHMOUR         yaghmourkarym@opersys.com
* Update :	20/08/2002,  Jerome CHEVALIER	        jerome.chevalier@polymtl.ca   Bugs fix 
*			09/08/2003,  Jerome CHEVALIER	        jerome.chevalier@polymtl.ca	  Cygwin and Windows port
*********************************************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"


#ifdef __CYGWIN__

	#include <sys/time.h>

	#define SA_NOMASK SA_NODEFER

	#define JB_AX 0
	#define JB_BX 1
	#define JB_CX 2
	#define JB_DX 3
	#define JB_SI 4
	#define JB_DI 5
	#define JB_BP 6
	#define JB_SP 7
	#define JB_PC 8
	#define JB_FLAGS 53

	#define	TIMER_MODE	ITIMER_REAL
	#define TIMER	SIGALRM

#elif WIN32

	#include <windows.h>
	#include <mmsystem.h>
	#include <float.h>
	#include <math.h>

	typedef struct
	{
		jmp_buf __jmpbuf ;
		int __saved_mask;
		int __save;
	} sigjmp_buf[1];

	extern int flag_reg;


	#define JB_BP 0 
	#define JB_BX 1 
	#define JB_DI 2 
	#define JB_SI 3 
	#define JB_SP 4
	#define JB_PC 5

	#define TIMER	SIGFPE

	#define sigsetjmp(env, savemask)		\
		((env)[0].__save = savemask,		\
		(env)[0].__saved_mask = flag_reg,	\
		setjmp((env)[0].__jmpbuf))			\

	#define siglongjmp(env, val)												\
	{																			\
  		int HFS;																\
		((env)[0].__save) ? flag_reg = (env)[0].__saved_mask : (flag_reg = 0);	\
		__asm	/*hack Windows protection against longjmp in handler*/			\
		{		/*env[0].__jmpbuf[6] = fs:[0]*/									\
			__asm mov eax,fs:[0]												\
			__asm mov dword ptr [HFS],eax										\
		};																		\
		(env)[0].__jmpbuf[6] = HFS;												\
		longjmp ((env)[0].__jmpbuf, val);										\
	} 

#else //Linux

	#include <sys/time.h>

	#define	TIMER_MODE	ITIMER_VIRTUAL
	#define TIMER		SIGVTALRM

#endif

/* Function Prototypes */
static void OSTaskEnd();

/* Glogal variable */
sigjmp_buf env[64];
sigjmp_buf envret;

int cpt=0;

/*
**********************************************************************************************************
*                                          OSUCINTHANDLER
*
* Description: This function is called when the uC process receive a signal
*
* Arguments  : none
*
* Note :
*.
**********************************************************************************************************
*/
void OSuCIntHandler(int sig)
{
   switch(sig){
	case TIMER:
		OSIntEnter();
		OSTimeTick();
		OSIntExit();
		
      	break;

	case SIGTERM:
	    	printf("Got signal %d (SIGTERM) during execution.\n uC terminating...\n", sig);
			OSuCStop();
      	break;
	default:
	    printf("Got signal %d during execution\n", sig);
	break;
   }
}


#ifdef WIN32

/*
**********************************************************************************************************
*                                          OSWINDOWSTIMERHANDLER
*
* Description: This function is called when the Windows timer expires.
*			   It generate a signal if the flags are unmask
*
* Arguments  : none
*
* Note : 
*.
**********************************************************************************************************
*/
VOID CALLBACK OSWindowsTimerHandler(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if(!flag_reg)
	{
		signal(TIMER,OSuCIntHandler);
		raise(TIMER);
	}
}

/*
**********************************************************************************************************
*                                          OSWINDOWSTIMER
*
* Description: This function is called in OSStartHighRdy when you start uC.
*              It initialize and start the windows timer
*
* Arguments  : none
*
* Note :
*.
**********************************************************************************************************
*/
void OSWindowsTimer()
{
	signal(TIMER,OSuCIntHandler);

	if((timeSetEvent(10, 0, OSWindowsTimerHandler, 0, TIME_PERIODIC))==0)
		printf("timeSetEvent failed (%d)\n", GetLastError());
}

#else //Cygwin and Linux


/* Signal handling */
struct sigaction uCSigAct;        /* Structure passed on to sigaction() */
extern sigset_t set;

/* System timer */
struct itimerval uCTimerVal;      /* Value of Linux timer */


/*
**********************************************************************************************************
*                                          OSLINUXTIMER
*
* Description: This function is called in OSStartHighRdy when you start uC.
*              It initialize and start the timer in Linux or Cygwin
*
* Arguments  : none
*
* Note :
*.
**********************************************************************************************************
*/
void OSLinuxTimer()
{


	/* Initialize the structure passed on to sigaction() */
    uCSigAct.sa_handler = (void*) OSuCIntHandler;   /* Signal handler */
    sigemptyset(&(uCSigAct.sa_mask));        /* Signal mask */
    uCSigAct.sa_flags   = SA_NOMASK;         /* Action flags, OK for nested signals */


    /* Initialize structure passed to setitimer() */
    uCTimerVal.it_interval.tv_sec  = 0;
    uCTimerVal.it_interval.tv_usec = 5000;
    uCTimerVal.it_value.tv_sec  = 0;
    uCTimerVal.it_value.tv_usec = 5000;
    sigfillset(&set);

	/* Setup signal handling */
    if((sigaction(SIGALRM,   &uCSigAct, NULL)<0)
     ||(sigaction(SIGVTALRM, &uCSigAct, NULL)<0)
     ||(sigaction(SIGTERM,   &uCSigAct, NULL)<0)
     ||(sigaction(SIGUSR1,   &uCSigAct, NULL)<0)
     ||(sigaction(SIGUSR2,   &uCSigAct, NULL)<0))
      	printf("Unable to register signal handler\n");

    /* Set up the alarm */
    if(setitimer(TIMER_MODE, &uCTimerVal, NULL)<0)
      printf("Unable to register timer\n");

}
#endif 


/*
**********************************************************************************************************
*                                          OSuCSTOP
*
* Description: This function must be called when you exit from uC
*
* Arguments  : none
*
* Note :
*.
**********************************************************************************************************
*/
void OSuCStop()
{
	OS_ENTER_CRITICAL();

	siglongjmp(envret,1);
	
}


/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created.  This function is highly processor specific.
*
* Arguments  : task          is a pointer to the task code
*
*              pdata         is a pointer to a user supplied data area that will be passed to the task
*                            when the task first executes.
*
*              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to
*                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then
*                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
*                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
*                            of the stack.
*
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
*                            (see uCOS_II.H for OS_TASK_OPT_???).
*
* Returns    : Always returns a pointer to Linux environement
*
* Note(s)    : Interrupts are enabled when your task starts executing.
*
*********************************************************************************************************
*/

OS_STK * OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt)
{
	OS_STK*	stk;

	stk =  ptos;					// Load stack pointer
	
	*stk = 0xffffffff;				// thread's stack limit contains -1
	*--stk = (INT32U) pdata;		// Simulate a function call (to pass the parameter)
#ifdef WIN32
	--stk;	
#endif
	*--stk = (INT32U) OSTaskEnd;	// Return address in case the task exits.

    /* Get a valid state */
    sigsetjmp(env[cpt],1) ;
      /* We don't got back here from the first call to longjmp() */
      /* because PC point to the start of the task  */

    /* Setup the exent's start address and stack end address */

#ifdef __CYGWIN__

    env[cpt][JB_BX] = (int) 0;
    env[cpt][JB_SI] = (int) 1;
    env[cpt][JB_DI] = (int) task;
    env[cpt][JB_BP] = (int) stk;
    env[cpt][JB_SP] = (int) stk;
    env[cpt][JB_PC] = (int) task;
    sigemptyset((sigset_t *)&env[cpt][JB_FLAGS]);

#elif WIN32

    env[cpt][0].__jmpbuf[JB_BX] = (int) 0;
    env[cpt][0].__jmpbuf[JB_SI] = (int) 1;
    env[cpt][0].__jmpbuf[JB_DI] = (int) task;
    env[cpt][0].__jmpbuf[JB_BP] = (int) stk;
    env[cpt][0].__jmpbuf[JB_SP] = (int) stk;
    env[cpt][0].__jmpbuf[JB_PC] = (int) task;
    env[cpt][0].__saved_mask = 0;

#else //Linux

    env[cpt][0].__jmpbuf[JB_BX] = (int) 0;
    env[cpt][0].__jmpbuf[JB_SI] = (int) 1;
    env[cpt][0].__jmpbuf[JB_DI] = (int) task;
    env[cpt][0].__jmpbuf[JB_BP] = (int) stk;
    env[cpt][0].__jmpbuf[JB_SP] = (int) stk;
    env[cpt][0].__jmpbuf[JB_PC] = (int) task;
    sigemptyset(&env[cpt][0].__saved_mask);

#endif

    return (OS_STK *)&env[cpt++];
}

/*
**********************************************************************************************************
*                                          START MULTITASKING
*                                       void OSStartHighRdy(void)
*
* Description: This function is called by OSstart() at the end of OSStart()
*               and start the simulation and call the highest priority task.
*
* Arguments  : none
*
* Note :   1) Interrupts are disabled during this call.
*.
**********************************************************************************************************
*/
void OSStartHighRdy(void)
{
	OS_ENTER_CRITICAL();

	if(sigsetjmp(envret,1)){
		printf("\n");
		printf("************************\n");
		printf("*        uC END        *\n");
		printf("************************\n");
		return;
	}

#ifdef WIN32

	OSWindowsTimer();

#else //Linux & Cygwin

	OSLinuxTimer();

#endif

	OSTaskSwHook();

	OSRunning++;

	printf("\n");
	printf("************************\n");
	printf("*       uC START       *\n");
	printf("************************\n");

	siglongjmp(*(sigjmp_buf*)(OSTCBHighRdy->OSTCBStkPtr),1);	
}

/*
**********************************************************************************************************
*                         PERFORM A CONTEXT SWITCH (From task level or an ISR )
*                                            void OSIntCtxSw(void)
*
* Description: This function is switch to highest priority task.
*
* Arguments  : none
*
* Note :   1) Interrupts are disabled during this call.
*.
**********************************************************************************************************
*/
void OSIntCtxSw(void)
{
	if(sigsetjmp(*(sigjmp_buf*)(OSTCBCur->OSTCBStkPtr),1))
		return;

	OSPrioCur = OSPrioHighRdy;

	OSTCBCur = OSTCBHighRdy;

	siglongjmp(*(sigjmp_buf*)(OSTCBHighRdy->OSTCBStkPtr),1);
}

/*
*********************************************************************************************************
*
*                                                      HOOK FUNCTIONS
*
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
void OSInitHookBegin (void)
{
}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
void OSInitHookEnd (void)
{
}

/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void OSTaskCreateHook(OS_TCB * pTcb)
{
}

/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void OSTaskDelHook(OS_TCB * pTcb)
{
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/
void OSTaskSwHook()
{
}

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/
void OSTaskStatHook()
{
}

/*
*********************************************************************************************************
*                                           OSTCBInit() HOOK
*
* Description: This function is called by OSTCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
void OSTCBInitHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                                           /* Prevent Compiler warning                 */
}


/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
void OSTimeTickHook()
{
}

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/
void OSTaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*
*                                                OSTASKEND
*
*  Description: This is the function that is executed if a task inadvertedly
*               returns from its main function. This is specified in OSTaskStkInit(),
*               when a task's stack is being intialized.
*
* Arguments  : none
*
*********************************************************************************************************
*/
void OSTaskEnd()
{

	OSTaskDel(OS_PRIO_SELF);

}

/*
*********************************************************************************************************
*                                                 uC/OS-II
*                                          The Real-Time Kernel
*                                      PORT LINUX CYGWIN and WINDOWS
*                                       DEFINITIONS AND DECLARATIONS
*
*						Jerome CHEVALIER, Ecole Polytechnique de Montreal, Qc, CANADA
*                                                  09/2001
*
* File : OS_CPU.H
* By   : Jerome CHEVALIER	        jerome.chevalier@polymtl.ca
* And the participation of : Karim YAGHMOUR         yaghmourkarym@opersys.com
* Update :	20/08/2002,  Jerome CHEVALIER	jerome.chevalier@polymtl.ca   Bugs fix and new features
*			09/08/2003,  Jerome CHEVALIER	jerome.chevalier@polymtl.ca	  Cygwin and Windows port
*********************************************************************************************************
*/

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT				extern
#endif

/*
*********************************************************************************************************
*
*                                  PORT SPECIFIC TYPE DEFINITIONS
*
*********************************************************************************************************
*/

typedef unsigned char			BOOLEAN;
typedef unsigned char			INT8U;
typedef char				INT8S;
typedef unsigned short			INT16U;
typedef short				INT16S;
typedef unsigned long			INT32U;
typedef long				INT32S;
typedef float				FP32;
typedef double				FP64;

typedef INT32U				OS_STK;
typedef void *				PTR;


/*
*********************************************************************************************************
*
*                                  PORT SPECIFIC MACRO DEFINITIONS
*
*********************************************************************************************************
*/

#ifdef WIN32
	#define  OS_ENTER_CRITICAL()	flag_reg = 1	
	#define  OS_EXIT_CRITICAL()		flag_reg = 0
#else
	#define  OS_ENTER_CRITICAL()	sigprocmask(SIG_BLOCK, &set, NULL)
	#define  OS_EXIT_CRITICAL()		sigprocmask(SIG_UNBLOCK, &set, NULL)
#endif

#define  OS_STK_GROWTH			1
#define  OS_TASK_SW()		OSIntCtxSw();

/*
*********************************************************************************************************
*
*                                  PORT SPECIFIC FONCTION DECLARATION
*
*********************************************************************************************************
*/



extern void				OSuCStop();



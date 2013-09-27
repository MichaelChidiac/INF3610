/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                        MESSAGE QUEUE MANAGEMENT
*
*                          (c) Copyright 1992-2001, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_Q.C
* By   : Jean J. Labrosse
* Update : Jerome CHEVALIER   jerome.chevalier@polymtl.ca   09/2001
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
/*
*********************************************************************************************************
*                                      ACCEPT MESSAGE FROM QUEUE
*
* Description: This function checks the queue to see if a message is available.  Unlike OSQPend(),
*              OSQAccept() does not suspend the calling task if a message is not available.
*
* Arguments  : pevent	un pointeur sur une queue de message (idem ancienne d�finition).
*
*			   msg		un pointeur sur une variable dans laquelle sera stock� le message re�u.
*						S�il n�y a pas de message, la valeur point� par msg reste inchang� et 
*						la fonction retourne OS_NO_MSG.
* Returns :		OS_NO_ERR, OS_NO_MSG, OS_ERR_PEVENT_NULL, OS_ERR_EVENT_TYPE, OS_NO_ERR, OS_NO_MSG
*********************************************************************************************************
*/

#if OS_Q_ACCEPT_EN > 0
INT8U  OSQAccept (OS_EVENT *pevent ,void  *msg)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    

    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {               /* Validate 'pevent'                                  */
        return (OS_ERR_PEVENT_NULL);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {/* Validate event block type                          */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
    pq = (OS_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
    if (pq->OSQEntries != 0) {                   /* See if any messages in the queue                   */
        OSMemCopy(msg, pq->OSQOut, pq->OSQDSize);/* Yes, extract oldest message from the queue         */
        ((char*)pq->OSQOut) += pq->OSQDSize;
		pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
    } else {
            OS_EXIT_CRITICAL();
            return (OS_NO_MSG);                         /* Queue is empty                                     */
    }
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);                                /* Return message received (or NULL)                  */
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : start         is a pointer to the base address of the message queue storage area.
*
*              size          is the number of elements in the storage area
*
*			   dsize		 la taille d�un emplacement de l�espace de stockage (taille d�un slot).
*
* Returns    : != (OS_EVENT *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                                created queue
*              == (OS_EVENT *)0  if no event control blocks were available or an error was detected
*********************************************************************************************************
*/

OS_EVENT  *OSQCreate (void *start, INT16U size, INT32U dsize)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_EVENT  *pevent;
    OS_Q      *pq;


    if (OSIntNesting > 0) {                      /* See if called from ISR ...                         */
        return ((OS_EVENT *)0);                  /* ... can't CREATE from an ISR                       */
    }
    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;                    /* Get next free event control block                  */
    if (OSEventFreeList != (OS_EVENT *)0) {      /* See if pool of free ECB pool was empty             */
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;
    }
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0) {               /* See if we have an event control block              */
        OS_ENTER_CRITICAL();                     /* Get a free queue control block                     */
        pq = OSQFreeList;
        if (OSQFreeList != (OS_Q *)0) {
            OSQFreeList = OSQFreeList->OSQPtr;
        }
        OS_EXIT_CRITICAL();
        if (pq != (OS_Q *)0) {                   /* See if we were able to get a queue control block   */
            pq->OSQStart        = start;         /* Yes, initialize the queue                          */
            pq->OSQEnd          = (void*)(((char*)start) + (size*dsize));
            pq->OSQIn           = start;
            pq->OSQOut          = start;
            pq->OSQSize         = size;
            pq->OSQDSize         = dsize;
            pq->OSQEntries      = 0;
            pevent->OSEventType = OS_EVENT_TYPE_Q;
            pevent->OSEventPtr  = pq;
            OS_EventWaitListInit(pevent);
        } else {                                 /* No,  since we couldn't get a queue control block   */
            OS_ENTER_CRITICAL();                 /* Return event control block on error                */
            pevent->OSEventPtr = (void *)OSEventFreeList;
            OSEventFreeList    = pevent;
            OS_EXIT_CRITICAL();
            pevent = (OS_EVENT *)0;
        }
    }
    return (pevent);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            queue.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete the queue ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the queue even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              err           is a pointer to an error code that can contain one of the following values:
*                            OS_NO_ERR               The call was successful and the queue was deleted
*                            OS_ERR_DEL_ISR          If you tried to delete the queue from an ISR
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the queue
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a queue
*                            OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the queue was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the queue MUST check the return code of OSQPend().
*              2) OSQAccept() callers will not know that the intended queue has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the queue.
*              4) Because ALL tasks pending on the queue will be readied, you MUST be careful in
*                 applications where the queue is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the queue.
*              5) If the storage for the message queue was allocated dynamically (i.e. using a malloc()
*                 type call) then your application MUST release the memory storage by call the counterpart
*                 call of the dynamic allocation scheme used.  If the queue storage was created statically
*                 then, the storage can be reused.
*********************************************************************************************************
*/

#if OS_Q_DEL_EN > 0
OS_EVENT  *OSQDel (OS_EVENT *pevent, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    BOOLEAN    tasks_waiting;
    OS_Q      *pq;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        *err = OS_ERR_DEL_ISR;                             /* ... can't DELETE from an ISR             */
        return ((OS_EVENT *)0);
    }
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                         /* Validate 'pevent'                        */
        *err = OS_ERR_PEVENT_NULL;
        return (pevent);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {          /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return (pevent);
    }
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                      /* See if any tasks waiting on queue        */
        tasks_waiting = TRUE;                              /* Yes                                      */
    } else {
        tasks_waiting = FALSE;                             /* No                                       */
    }
    switch (opt) {
        case OS_DEL_NO_PEND:                               /* Delete queue only if no task waiting     */
             if (tasks_waiting == FALSE) {
                 pq                  = pevent->OSEventPtr; /* Return OS_Q to free list                 */ 
                 pq->OSQPtr          = OSQFreeList;
                 OSQFreeList         = pq;
                 pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr  = OSEventFreeList;    /* Return Event Control Block to free list  */
                 OSEventFreeList     = pevent;             /* Get next free event control block        */
                 OS_EXIT_CRITICAL();
                 *err = OS_NO_ERR;
                 return ((OS_EVENT *)0);                   /* Queue has been deleted                   */
             } else {
                 OS_EXIT_CRITICAL();
                 *err = OS_ERR_TASK_WAITING;
                 return (pevent);
             }

        case OS_DEL_ALWAYS:                                /* Always delete the queue                  */
             while (pevent->OSEventGrp != 0x00) {          /* Ready ALL tasks waiting for queue        */
                 OS_EventTaskRdy(pevent, (void *)0, OS_STAT_Q);
             }
             pq                  = pevent->OSEventPtr;     /* Return OS_Q to free list                 */ 
             pq->OSQPtr          = OSQFreeList;
             OSQFreeList         = pq;
             pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr  = OSEventFreeList;        /* Return Event Control Block to free list  */
             OSEventFreeList     = pevent;                 /* Get next free event control block        */
             OS_EXIT_CRITICAL();
             if (tasks_waiting == TRUE) {                  /* Reschedule only if task(s) were waiting  */
                 OS_Sched();                               /* Find highest priority task ready to run  */
             }
             *err = OS_NO_ERR;
             return ((OS_EVENT *)0);                       /* Queue has been deleted                   */

        default:
             OS_EXIT_CRITICAL();
             *err = OS_ERR_INVALID_OPT;
             return (pevent);
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : none
*
* Returns     : OS_NO_ERR           upon success
*               OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a queue
*               OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*********************************************************************************************************
*/

#if OS_Q_FLUSH_EN > 0
INT8U  OSQFlush (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
    pq             = (OS_Q *)pevent->OSEventPtr;      /* Point to queue storage structure              */
    pq->OSQIn      = pq->OSQStart;
    pq->OSQOut     = pq->OSQStart;
    pq->OSQEntries = 0;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                     PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time
*                            specified by this argument.  If you specify 0, however, your task will wait
*                            forever at the specified queue or, until a message arrives.
*
*			   msg			 un pointeur sur une variable dans laquelle sera stock� le message re�u.
*
*
* Returns    :				 OS_NO_ERR           The call was successful and your task received a message.
*                            OS_TIMEOUT          A message was not received within the specified timeout
*                            OS_ERR_EVENT_TYPE   You didn't pass a pointer to a queue
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension..
*********************************************************************************************************
*/

INT8U OSQPend (OS_EVENT *pevent,void *msg, INT16U timeout)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
      OS_Q      *pq;


    if (OSIntNesting > 0) {                      /* See if called from ISR ...                         */
                          /* ... can't PEND from an ISR                         */
        return (OS_ERR_PEND_ISR);
    }
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {               /* Validate 'pevent'                                  */

        return (OS_ERR_PEVENT_NULL);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {/* Validate event block type                          */

        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
    pq = (OS_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
    if (pq->OSQEntries != 0) {                   /* See if any messages in the queue                   */
         OSMemCopy(msg, pq->OSQOut, pq->OSQDSize);               /* Yes, extract oldest message from the queue         */
        ((char*)pq->OSQOut) += pq->OSQDSize;
        pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
        OS_EXIT_CRITICAL();

        return (OS_NO_ERR);                            /* Return message received                            */
    }
    OSTCBCur->OSTCBStat |= OS_STAT_Q;            /* Task will have to pend for a message to be posted  */
    OSTCBCur->OSTCBDly   = timeout;              /* Load timeout into TCB                              */
    OS_EventTaskWait(pevent);                    /* Suspend task until event or timeout occurs         */
    OS_EXIT_CRITICAL();
    OS_Sched();                                  /* Find next highest priority task ready to run       */
    OS_ENTER_CRITICAL();
    if (OSTCBCur->OSTCBMsg != (void *)0) {                      /* Did we get a message?                              */
        OSMemCopy(msg,OSTCBCur->OSTCBMsg, pq->OSQDSize);
		OSTCBCur->OSTCBMsg      = (void *)0;     /* Extract message from TCB (Put there by QPost)      */
        OSTCBCur->OSTCBStat     = OS_STAT_RDY;
        OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0; /* No longer waiting for event                        */
        OS_EXIT_CRITICAL();

        return (OS_NO_ERR);                            /* Return message received                            */
    }
    OS_EventTO(pevent);                          /* Timed out                                          */
    OS_EXIT_CRITICAL();
                  /* Indicate a timeout occured                         */
    return (OS_TIMEOUT);                          /* No message received                                */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post a NULL pointer
*********************************************************************************************************
*/

#if OS_Q_POST_EN > 0
INT8U  OSQPost (OS_EVENT *pevent, void *msg)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (msg == (void *)0) {                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                 /* See if any task pending on queue              */
        OS_EventTaskRdy(pevent, msg, OS_STAT_Q);      /* Ready highest priority task waiting on event  */
        OS_EXIT_CRITICAL();
        OS_Sched();                                   /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    }
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
    if (pq->OSQEntries >= pq->OSQSize) {              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }

    OSMemCopy(pq->OSQIn, msg, pq->OSQDSize);              /* Insert message into queue                     */
    ((char*)pq->OSQIn) += pq->OSQDSize;
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    if (pq->OSQIn == pq->OSQEnd) {                    /* Wrap IN ptr if we are at end of queue         */
        pq->OSQIn = pq->OSQStart;
    }
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                   POST MESSAGE TO THE FRONT OF A QUEUE
*
* Description: This function sends a message to a queue but unlike OSQPost(), the message is posted at
*              the front instead of the end of the queue.  Using OSQPostFront() allows you to send
*              'priority' messages.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post to a non queue.
*********************************************************************************************************
*/

#if OS_Q_POST_FRONT_EN > 0
INT8U  OSQPostFront (OS_EVENT *pevent, void *msg)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (msg == (void *)0) {                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                 /* See if any task pending on queue              */
        OS_EventTaskRdy(pevent, msg, OS_STAT_Q);      /* Ready highest priority task waiting on event  */
        OS_EXIT_CRITICAL();
        OS_Sched();                                   /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    }
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
    if (pq->OSQEntries >= pq->OSQSize) {              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }
    if (pq->OSQOut == pq->OSQStart) {                 /* Wrap OUT ptr if we are at the 1st queue entry */
        pq->OSQOut = pq->OSQEnd;
    }
    ((char*)pq->OSQOut) -= pq->OSQDSize;
    OSMemCopy(pq->OSQOut, msg, pq->OSQDSize);                                /* Insert message into queue                     */
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  This call has been added to reduce code size
*              since it can replace both OSQPost() and OSQPostFront().  Also, this function adds the 
*              capability to broadcast a message to ALL tasks waiting on the message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
*              opt           determines the type of POST performed:
*                            OS_POST_OPT_NONE         POST to a single waiting task 
*                                                     (Identical to OSQPost())
*                            OS_POST_OPT_BROADCAST    POST to ALL tasks that are waiting on the queue
*                            OS_POST_OPT_FRONT        POST as LIFO (Simulates OSQPostFront())
*
*                            Below is a list of ALL the possible combination of these flags:
*
*                                 1) OS_POST_OPT_NONE
*                                    identical to OSQPost()
*
*                                 2) OS_POST_OPT_FRONT  
*                                    identical to OSQPostFront()
*
*                                 3) OS_POST_OPT_BROADCAST
*                                    identical to OSQPost() but will broadcast 'msg' to ALL waiting tasks
*
*                                 4) OS_POST_OPT_FRONT + OS_POST_OPT_BROADCAST  is identical to
*                                    OSQPostFront() except that will broadcast 'msg' to ALL waiting tasks
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post a NULL pointer
*
* Warning    : Interrupts can be disabled for a long time if you do a 'broadcast'.  In fact, the 
*              interrupt disable time is proportional to the number of tasks waiting on the queue.
*********************************************************************************************************
*/

#if OS_Q_POST_OPT_EN > 0
INT8U  OSQPostOpt (OS_EVENT *pevent, void *msg, INT8U opt)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (msg == (void *)0) {                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                 /* See if any task pending on queue              */
        if ((opt & OS_POST_OPT_BROADCAST) != 0x00) {  /* Do we need to post msg to ALL waiting tasks ? */
            while (pevent->OSEventGrp != 0x00) {      /* Yes, Post to ALL tasks waiting on queue       */           
                OS_EventTaskRdy(pevent, msg, OS_STAT_Q);    
            }
        } else {
            OS_EventTaskRdy(pevent, msg, OS_STAT_Q);  /* No,  Post to HPT waiting on queue             */
        }
        OS_EXIT_CRITICAL();
        OS_Sched();                                   /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    }
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
    if (pq->OSQEntries >= pq->OSQSize) {              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }
    if ((opt & OS_POST_OPT_FRONT) != 0x00) {          /* Do we post to the FRONT of the queue?         */
        if (pq->OSQOut == pq->OSQStart) {             /* Yes, Post as LIFO, Wrap OUT pointer if we ... */
            pq->OSQOut = pq->OSQEnd;                  /*      ... are at the 1st queue entry           */
        }
        ((char*)pq->OSQOut) -= pq->OSQDSize;
      OSMemCopy(pq->OSQOut, msg, pq->OSQDSize);                            /*      Insert message into queue                */
    } else {                                          /* No,  Post as FIFO                             */
        OSMemCopy(pq->OSQIn, msg, pq->OSQDSize);                           /*      Insert message into queue                */
        ((char*)pq->OSQIn) += pq->OSQDSize;
        if (pq->OSQIn == pq->OSQEnd) {                /*      Wrap IN ptr if we are at end of queue    */
            pq->OSQIn = pq->OSQStart;
        }
    }
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        QUERY A MESSAGE QUEUE
*
* Description: This function obtains information about a message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              pdata         is a pointer to a structure that will contain information about the message
*                            queue.
*
* Returns    : OS_NO_ERR           The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE   If you are attempting to obtain data from a non queue.
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*********************************************************************************************************
*/

#if OS_Q_QUERY_EN > 0
INT8U  OSQQuery (OS_EVENT *pevent, OS_Q_DATA *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_Q      *pq;
    INT8U     *psrc;
    INT8U     *pdest;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                         /* Validate 'pevent'                        */
        return (OS_ERR_PEVENT_NULL);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {          /* Validate event block type                */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
    pdata->OSEventGrp = pevent->OSEventGrp;                /* Copy message queue wait list           */
    psrc              = &pevent->OSEventTbl[0];
    pdest             = &pdata->OSEventTbl[0];
#if OS_EVENT_TBL_SIZE > 0
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 1
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 2
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 3
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 4
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 5
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 6
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 7
    *pdest            = *psrc;
#endif
    pq = (OS_Q *)pevent->OSEventPtr;
    if (pq->OSQEntries > 0) {
        pdata->OSMsg = pq->OSQOut;                         /* Get next message to return if available  */
    } else {
        pdata->OSMsg = (void *)0;
    }
    pdata->OSNMsgs = pq->OSQEntries;
    pdata->OSQSize = pq->OSQSize;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif                                                     /* OS_Q_QUERY_EN                            */

/*$PAGE*/
/*
*********************************************************************************************************
*                                      QUEUE MODULE INITIALIZATION
*
* Description : This function is called by uC/OS-II to initialize the message queue module.  Your
*               application MUST NOT call this function.
*
* Arguments   :  none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

void  OS_QInit (void)
{
#if OS_MAX_QS == 1
    OSQFreeList         = &OSQTbl[0];            /* Only ONE queue!                                    */
    OSQFreeList->OSQPtr = (OS_Q *)0;
#endif

#if OS_MAX_QS >= 2
    INT16U  i;
    OS_Q   *pq1;
    OS_Q   *pq2;


    pq1 = &OSQTbl[0];
    pq2 = &OSQTbl[1];
    for (i = 0; i < (OS_MAX_QS - 1); i++) {      /* Init. list of free QUEUE control blocks            */
        pq1->OSQPtr = pq2;
        pq1++;
        pq2++;
    }
    pq1->OSQPtr = (OS_Q *)0;
    OSQFreeList = &OSQTbl[0];
#endif
}
#endif                                                     /* OS_Q_EN                                  */

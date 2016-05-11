#include "sst.h"
#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define PRINT(...)    printf(...)
#else
#define PRINT(...)
#endif

/* Public-scope objects ----------------------------------------------------*/
sst_uint8_t sst_curr_prio = (sst_uint8_t)0xFF;          /* current SST priority */
sst_uint8_t sst_ready_set = (sst_uint8_t)0;             /* SST ready-set */

typedef struct task_cb_tag 
{
    sst_task task;
    sst_event *queue;
    sst_uint8_t end;
    sst_uint8_t head;
    sst_uint8_t tail;
    sst_uint8_t nUsed;
    sst_uint8_t mask;
} task_cb;

/* Local-scope objects -----------------------------------------------------*/
static task_cb g_task_cb[SST_MAX_PRIO];
static sst_idle sst_onidle = 0;

static sst_uint8_t const CODE log2lkup[] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

sst_bool_t sst_idle_register(sst_idle onidle)
{
    if (sst_onidle == 0)
    {
        sst_onidle = onidle;
        return (sst_bool_t)1;
    }
    else
    {
        return (sst_bool_t)0;
    }
}

/*..........................................................................*/
void sst_task_create(
                     sst_task task, 
                     sst_uint8_t prio, 
                     sst_event *queue, 
                     sst_uint8_t qlen,
                     sst_signal sig, 
                     sst_param par)
{		
    sst_event ie;                                    /* initialization event */
    
	if(prio == 0)
    {
        printf("ERROR:priolity 0 is always set for idle task \n");
        return;
    }
    task_cb *tcb  = &g_task_cb[prio - 1];
    tcb->task  = task;
    tcb->queue = queue;
    tcb->end   = qlen;
    tcb->head  = (sst_uint8_t)0;		//write
    tcb->tail  = (sst_uint8_t)0;		//read
    tcb->nUsed = (sst_uint8_t)0;
    tcb->mask  = (1 << (prio - 1));
    ie.sig = sig;
    ie.par = par;
    tcb->task(ie);                                 /* initialize the task */
}
/*..........................................................................*/
void sst_run(void) 
{
    SST_INT_LOCK();
    sst_curr_prio = (sst_uint8_t)0; /* set the priority for the SST idle loop */
    //sst_schedule();                     /* process all events produced so far */
    SST_INT_UNLOCK();

    while(1) {                                         /* the SST idle loop */
        /* invoke the on-idle callback */
        if (sst_onidle != 0) 
        {    
            sst_onidle();
        }
                     
    }
}

/*..........................................................................*/
sst_bool_t sst_post(sst_uint8_t prio, sst_signal sig, sst_param par) 
{
    task_cb *tcb = &g_task_cb[prio - 1];
    SST_INT_LOCK();
    printf("%d |%d\n", tcb->nUsed, tcb->end);
    if (tcb->nUsed < tcb->end) {
        tcb->queue[tcb->head].sig = sig;	/* insert the event at the head */
        tcb->queue[tcb->head].par = par;
        if ((++tcb->head) == tcb->end) {
            tcb->head = (sst_uint8_t)0;             		/* wrap the head */
        }
        if ((++tcb->nUsed) == (sst_uint8_t)1) {       	/* the first event? */
            sst_ready_set |= tcb->mask;   	/* insert task to the ready set */
        printf("1111\n");
            sst_schedule();            	/* check for synchronous preemption */
        }
        SST_INT_UNLOCK();
        return (sst_bool_t)1;                  /* event successfully posted */
    }
    else {
        printf("sdfasdfs");
        SST_INT_UNLOCK();
        return (sst_bool_t)0;           /* queue full, event posting failed */
    }
}
/*..........................................................................*/
sst_uint8_t sst_mutex_lock(sst_uint8_t prio_ceiling) 
{
    sst_uint8_t p;
    SST_INT_LOCK();
    p = sst_curr_prio;               /* the original SST priority to return */
    if (prio_ceiling > sst_curr_prio) {
        sst_curr_prio = prio_ceiling;              /* raise the SST priority */
    }
    SST_INT_UNLOCK();
    return p;
}
/*..........................................................................*/
void sst_mutex_unlock(sst_uint8_t org_prio) {
    SST_INT_LOCK();
    if (org_prio < sst_curr_prio) {
        sst_curr_prio = org_prio;    /* restore the saved priority to unlock */
        sst_schedule();    /* invoke scheduler after lowering the priority */
    }
    SST_INT_UNLOCK();
}
/*..........................................................................*/
/* NOTE: the SST scheduler is entered and exited with interrupts LOCKED */
void sst_schedule(void) 
{
    sst_uint8_t pin = sst_curr_prio;     /* save the initial priority */
    sst_uint8_t p;                       /* the new priority */
                     
    /* is the new priority higher than the initial? */
    while ((p = log2lkup[sst_ready_set]) > pin) 
	{
        printf("G ");
        fflush(stdout); 
        task_cb *tcb  = &g_task_cb[p - 1];
                                          			
        sst_event e = tcb->queue[tcb->tail];/* get the event out of the queue */
        if ((++tcb->tail) == tcb->end) 
		{
            tcb->tail = (sst_uint8_t)0;
        }
        if ((--tcb->nUsed) == (sst_uint8_t)0) /* is the queue becoming empty?*/
		{				
            sst_ready_set &= ~tcb->mask;     /* remove from the ready set */
        }
        sst_curr_prio = p;        	/* this becomes the current task priority */
		
        SST_INT_UNLOCK();                          /* unlock the interrupts */
        (*tcb->task)(e);                            /* call the SST task */
        SST_INT_LOCK();              /* lock the interrupts for the next pass */
    }
    sst_curr_prio = pin;                  /* restore the initial priority */
}

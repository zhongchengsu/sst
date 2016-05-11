#include "core/sst.h"
#include <stdio.h>

#define TASK_Q_LEN 128
sst_event inter_task_q[TASK_Q_LEN];
sst_event io1_task_q[TASK_Q_LEN];
sst_event io2_task_q[TASK_Q_LEN];

void inter_task(sst_event event)
{
    //if (event.par != NULL)
    {
        //printf("\033[1;31;40m%s \033[0m", (char *)event.par);
        printf("\033[1;31;40mTASK_%s \033[0m", (char *)event.par);
        fflush(stdout); 
    }
    sst_post(IO2_PRIO, 0, (sst_param)event.par);
}

void io1_task(sst_event event)
{
    int i;
    for (i = 0; i < 5; ++i)
    {
    	/* code */
    	printf("\033[1;33;40mIO1_%s \033[0m", (char *)event.par);
    	fflush(stdout);
    	sleep(1);
    }    
}

void io2_task(sst_event event)
{
    int i;
    for (i = 0; i < 5; ++i)
    {
    	/* code */
    	printf("\033[1;34;40mIO2_%s \033[0m", (char *)event.par);
    	fflush(stdout);
    	sleep(1);
    }    
    sst_post(IO1_PRIO, 0, (sst_param)event.par);
}


void task_init()
{
    /*add task here*/
    sst_task_create(inter_task, INTER_PRIO,
                    inter_task_q, 
                    TASK_Q_LEN,
                    0,
                    (sst_param)"I");

    sst_task_create(io2_task, IO2_PRIO,
                    inter_task_q, 
                    TASK_Q_LEN,
                    0,
                    (sst_param)"I");

    sst_task_create(io1_task, IO1_PRIO,
                    inter_task_q, 
                    TASK_Q_LEN,
                    0,
                    (sst_param)"I");    

}
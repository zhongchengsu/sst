#include "core/sst.h"
#include <stdio.h>

#define INTER_TASK_Q_LEN 128
sst_event inter_task_q[INTER_TASK_Q_LEN];

void idle_task()
{
    printf("\033[1;32;40mT \033[0m");
    fflush(stdout); 
    usleep(1000000);
}

void inter_task(sst_event event)
{
    //printf("\033[1;13;40mP \033[0m");
    //fflush(stdout); 
    //usleep(1000000);

    if (event.par != NULL)
    {
        printf("\033[1;31;40m%s \033[0m", (char *)event.par);
        fflush(stdout); 
    }
}


void task_init()
{
    /*add task here*/
    sst_idle_register(idle_task);

    sst_task_create(inter_task, INTER_PRIO,
                    inter_task_q, 
                    INTER_TASK_Q_LEN,
                    0,
                    (sst_param)NULL);
}
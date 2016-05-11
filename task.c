#include "core/sst.h"
#include <stdio.h>
#include <unistd.h>

void idle_task()
{
    printf("T ");
    fflush(stdout); 
    usleep(1000000);
}

void task_init()
{
    /*add task here*/
    sst_idle_register(idle_task);
}
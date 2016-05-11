#include "sst.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#define FIFO "/tmp/sstfifo2"

char buf_r[100];
int fd;
int nread;

void virtual_interupt()
{

  pid_t fpid;
  int count=0;
  
  if((mkfifo(FIFO,O_CREAT|O_EXCL)<0)&&(errno!=EEXIST))
    printf("cannot create fifoserver\n");

  memset(buf_r,0,sizeof(buf_r));
  fd=open(FIFO,O_RDONLY|O_NONBLOCK,0);
  if(fd==-1)
  {
    perror("open");
    exit(1);
  }
}


void idle_task()
{
    int nread;
    printf("\033[1;32;40mSST  \033[0m");

    memset(buf_r,0,sizeof(buf_r));
    if((nread=read(fd,buf_r,100))>0)
    {
        if(errno==EAGAIN)
        {
        	 printf("no data yet\n");
        }  
        //printf("sst_post %s ", buf_r); 
        sst_post(INTER_PRIO, 0, (sst_param)"F");  
    }
    fflush(stdout);
    usleep(1000000);
}

void sigroutine(int dunno)

{
  switch (dunno)

  {
    case 1:
      printf("Get a signal -- SIGHUP ");
      break;
    case 2:
      //printf("Get a signal -- SIGINT ");
      sst_post(INTER_PRIO, 0, (sst_param)"C");
      break;
    case 3:
      printf("Get a signal -- SIGQU99v ");
      break;
    default:
      printf("%d  ", dunno);
  }
  return;
}

void main(void)
{   
    signal(SIGHUP, sigroutine); //* 下面设置三个信号的处理方法
    signal(SIGINT, sigroutine);
    signal(SIGQUIT, sigroutine);
    virtual_interupt();
    sst_idle_register(idle_task);
    task_init();
    sst_run();  
    pause();
    unlink(FIFO);   
}


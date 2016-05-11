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
#define FIFO "/tmp/sstfifo"

void virtual_interupt_thread()
{
  char buf_r[100];
  int fd;
  int nread;
  pid_t fpid;
  int count=0;
  
  fpid = fork();

  if (fpid < 0)
  {
     printf("Error init interupt!\n");
  }
  else if (fpid == 0)
  {
    if((mkfifo(FIFO,O_CREAT|O_EXCL)<0)&&(errno!=EEXIST))
      printf("cannot create fifoserver\n");

    memset(buf_r,0,sizeof(buf_r));
    fd=open(FIFO,O_RDONLY|O_NONBLOCK,0);
    if(fd==-1)
    {
      perror("open");
      exit(1);
    }

    while(1)
    {
      memset(buf_r,0,sizeof(buf_r));
      if((nread=read(fd,buf_r,100))>0){
        if(errno==EAGAIN)
        {
        	 printf("no data yet\n");
        }  
        printf("sst_post %s ", buf_r); 
        sst_post(INTER_PRIO, 0, (sst_param)buf_r);  
      }
      sleep(1);
      fflush(stdout);
    }
    pause();
    unlink(FIFO);
  }
}

void main(void)
{    
    virtual_interupt_thread();
    task_init();
    sst_run();     
}


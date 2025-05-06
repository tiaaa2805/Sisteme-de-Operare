#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<string.h>
#include <errno.h>
#include<time.h>
#include<dirent.h>
#include<signal.h>
#include<stdbool.h>
#include"treasur.h"
#define Max 235
volatile sig_atomic_t monitor_execution=0;
volatile sig_atomic_t monitor_stop=0;

void handler_view_treasure()
{
  if(monitor_stop==0)
    view_treasure();
  else
    
  
}
void handler_stop_monitor()
{
    usleep(5000000);
    exit(-1);
}
void handler_sigc()
{
  int status;
  waitpid(getpid(),&status,0);
  printf("Monitorul are acest status %d\n", WEXISTATUS(status));
  monitor_stop=-1;
}
void setup_for_handler()
{
   struct sigaction sa;
  sa.sa_handler=handler_list_hunt;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if(sigaction(SIGUSR1,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  sa.sa_handler=handler_list_treasures;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if(sigaction(SIGUSR2,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  sa.sa_handler=handler_stop_monitor;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if(sigaction(SIGTERM,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  sa.sa_handler=handler_view_treasure;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if(sigaction(SIGINT,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }

  sa.sa_handler=handler_sigc;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if(sigaction(SIGCHLD,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
}
int main()
{
  return 0;
}

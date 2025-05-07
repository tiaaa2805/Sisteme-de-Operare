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
#define Max 500
volatile sig_atomic_t monitor_execution=0;
volatile sig_atomic_t monitor_stop=0;
void readdetails()
{
  int fd=open("comenzi.txt",O_RDONLY,S_IRUSR);
  if(fd==-1)
    {
      perror("Eroare la deschiderea firierului pentru citirea comenzilor\n");
      return;
    }
   lseek(fd,0,SEEK_SET);
   char linie[256];
   int i=0;
  while(read(fd,linie,1)==1)
    {
      if(linie[i]=='\n')
	{
	  linie[i+1]='\0';
	  break;
	}
      i++;
      if(i>255)
	{
	  linie[i]='\0';
	  break;
	}
    }
  printf("Comanda preluata din text este %s\n");
  close(fd);
}
void handler_view_treasure()
{
  if(monitor_stop==0)
    view_treasure();
  else
    printf("Eroare, monitor posibil inchis!\n");
}
void handler_list_treasure()
{
  if(monitor_stop==0)
    list_treasures();
  else
    printf("Eroare, monitor posibil inchis!\n");
}

void handler_stop_monitor()
{
    usleep(5000000);
    monitor_stop=-1;
    exit(0);
}
void handler_sigc()
{
  int status;
  pid_t v=waitpid(getpid(),&status,0);
  if(v==-1)
    {
      printf("Eroare la waitpid\n");
    }
  printf("Monitorul are acest status %d\n", WEXISTATUS(status));
}
void listaredirector()
{
  char cale[Max];
  DIR *fd=opendir(".");
  struct dirent *citim;
  if(fd)
    {
      while((citim==readdir(fd))!=NULL)
	{
	  if(strcmp(citim->d_name,".")==0 || strcmp(citim->d_name,"..")==0)
	    continue;
	  if(strcmp(citim->d_type, DT_DIR)==0)
	    {
	      strcpy(director,citim);
	      DIR *fdd=opendir(".");
	      struct dirent *citim2;
	      if(fdd)
		{
		  while((citim2==readdir(fdd))!=NULL)
		    {
		      if(strcmp(citim2->d_nume,".")==0 || strcmp(citim2->d_nume,"..")==0)
			continue;
		      if(strstr(citim2->d_nume,".dat")!=NULL)
			{
			   snprintf(cale,Max,"%s/%s",director,citim2->d_nume);
		           int fisier=open(cale, O_RDONLY,S_IRUSR);
		           int nr=nrtreasure(fisier);
		           printf("Numarul treasurilor este %d \n\n", nr);
		           close(fisier);
			   break;
			}
		    }
		  
		}
	      if(closedir(fdd)==0)
		printf("Hunt-ul a fost inchis cu succes!!\n");
	      else
		perror("Exista o rpoblema la inchiderea Huntului\n");
		      
	    }
	}
    }
  if(closedir(fd)==0)
    printf("Directorul a fost inchis cu succes \n");
  else
    perror("INCHIDEREA NU A FUNCTIONAT\n");
}
void setup_for_handler(int sig)
{
  switch(sig)
    {
    case SIGUSR1:listaredirector();
      break;
    case SIGUSR2:
      break;
    case SIGTERM :
      break;
    case SIGINT:
      break;
    case SIGCHLD:
      break;
    case SIGHUP:///pt remove
      
    }
  
}
int main()
{ struct sigaction sa;
  sa.sa_handler=setup_for_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if(sigaction(SIGUSR1,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  if(sigaction(SIGUSR2,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  if(sigaction(SIGTERM,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  if(sigaction(SIGINT,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  if(sigaction(SIGCHLD,&sa,NULL)==-1)
    {
      perror("Eroare la sigaction\n");
      exit(1);
    }
  return 0;
}

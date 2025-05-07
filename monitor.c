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
#include<limits.h>
#include<sys/wait.h>
#include<stdbool.h>
#include"treasur.h"
#define Max 500
int ls=0;
volatile sig_atomic_t monitor_execution=0;
volatile sig_atomic_t monitor_stop=0;
char  *readdetails()
{
  int fd=open("comenzi.txt",O_RDONLY,S_IRUSR);
  if(fd==-1)
    {
      perror("Eroare la deschiderea firierului pentru citirea comenzilor\n");
      return NULL;
    }
  lseek(fd,ls*sizeof(char),SEEK_SET);
  char *linie=malloc(Max);
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
  ls++;
  printf("Comanda preluata din text este %s\n",linie);
  printf("\nNe ocupam de transmiterea detaliilor\n");
  close(fd);
  return linie;
}
void handler_view_treasure()
{
  char op[Max], hunt[Max], tres[Max];
  strcpy( op,readdetails());
  int len=strcspn(op," ");
  int len2=strcspn(op+len+1," ");
  strncpy(hunt,op+len+1,strlen(op)-len2);
  strcpy(tres,op+len2+1);
  if(monitor_stop==0)
    view(hunt,tres);
  else
    printf("Eroare, monitor posibil inchis!\n");
}
void handler_list_treasures()
{
  char op[Max];
  snprintf(op,Max,"%s",readdetails());
  int len=strcspn(op," ");
  if(monitor_stop==0)
    list(op+len+1);
  else
    printf("Eroare, monitor posibil inchis!\n");
}

void handler_stop_monitor()
{printf("Monitorul se va inchide in 5 secunde\n");
  
    usleep(5000000);
    printf("Monitorul se afla in procesul de inchidere\n");
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
  printf("Monitorul are acest status %d\n", WEXITSTATUS(status));
}
int este_director(const char *oo)
{
  struct stat st;
  return ((stat(oo,&st)==0) && S_ISDIR(st.st_mode));
}
void handler_list_directory()
{
  char cale[PATH_MAX], director[Max];
  DIR *fd=opendir(".");
  struct dirent *citim;
  if(fd)
    {
      while((citim=readdir(fd))!=NULL)
	{
	  if(strcmp(citim->d_name,".")==0 || strcmp(citim->d_name,"..")==0)
	    continue;
	  
	  if(este_director(citim->d_name))
	    {
	      snprintf(director,Max,"%s",citim->d_name);
	      DIR *fdd=opendir(".");
	      struct dirent *citim2;
	      if(fdd)
		{
		  while((citim2=readdir(fdd))!=NULL)
		    {
		      if(strcmp(citim2->d_name,".")==0 || strcmp(citim2->d_name,"..")==0)
			continue;
		      printf("Numele huntului este %s\n",citim2->d_name);
		      
			  snprintf(cale,sizeof(cale),"%s/%s",citim2->d_name,"treasure.dat");
		           int fisier=open(cale, O_RDONLY,S_IRUSR);
		           int nr=nrtreasure(fisier);
		           printf("Numarul treasurilor este %d \n\n", nr);
		           close(fisier);
			   break;
			
		      
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
    case SIGUSR1:printf("Ne ocupam de listarea directoarelor si numarului de treasururi in fiecare hunt\n");
      handler_list_directory();
        printf("\n\n--------------------------------------------------\n\n");
      break;
    case SIGUSR2:
      printf("Ne ocupam de listarea treasure_urilor dintr-un hunt\n");
      handler_list_treasures();
       printf("\n\n--------------------------------------------------\n\n");
      break;
    case SIGTERM :
      printf("Ne ocupam de inchiderea monitorului\n");
      handler_stop_monitor();
       printf("\n\n--------------------------------------------------\n\n");
      break;
    case SIGINT:
      printf("Ne ocupam de deschiderea unui HUNT, cu scopul analizei unui treasure\n");
      handler_view_treasure();
       printf("\n\n--------------------------------------------------\n\n");
      /*break;
    case SIGHUP:///pt remove
      */
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
  printf("Pidul monitorului nostru este %d\n\n",getpid());
  while(1)
    {
      pause();
    }
  return 0;
}

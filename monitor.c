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
      perror("Eroare la deschiderea fisierului pentru citirea comenzilor\n");
      return NULL;
    }
  lseek(fd,ls*sizeof(char),SEEK_SET);
  char *linie=malloc(Max);
  char ch;
   int i=0;
  while(read(fd,&ch,1)==1)
    {
      if(ch=='\n' || i>255)
	{
	  linie[i]='\0';
	  break;
	}
      linie[i++]=ch;
    }
  ls++;
  size_t len=strlen(linie);
  if(linie[len-1]=='\n')
    linie[len-1]='\0';
  printf("Comanda preluata din text este %s\n",linie);
  printf("\nNe ocupam de transmiterea detaliilor\n");
  close(fd);
  return linie;
}
void handler_view_treasure()
{
  char op[Max];
  char *hunt, *tres;
   snprintf(op,Max,"%s",readdetails());
   char *len=strchr(op,' ');
   len++;
   char *len2=strchr(len,' ');
   size_t p=len2-len;
  hunt=strndup(len,p);
   len2++;
   tres=len2;
  printf("%s %s\n",hunt,tres);
  if(monitor_stop==0)
    view(hunt,tres);
  else
    printf("Eroare, monitor posibil inchis!\n");
}
void handler_list_treasures()
{
  char op[Max];
  snprintf(op,Max,"%s",readdetails());
  op[strcspn(op,"\n")]='\0';
  int len=strcspn(op," ");
  printf("%s\n",op+len+1);
  if(monitor_stop==0)
    {
      list(op+len+1);
    }
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
	  if(strcmp(citim->d_name,".")==0 || strcmp(citim->d_name,"..")==0 || strcmp(citim->d_name,".git")==0)
	    continue;
	  if(este_director(citim->d_name))
	    {
	      snprintf(director,Max,"%s",citim->d_name);
	      DIR *fdd=opendir(director);
	      struct dirent *citim2;
	      if(fdd)
		{
		  while((citim2=readdir(fdd))!=NULL)
		    {
		      if(strcmp(citim2->d_name,".")==0 || strcmp(citim2->d_name,"..")==0 )
			continue;
		         printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		      printf("\tNumele huntului este: %s\n",director);
		      
			  snprintf(cale,sizeof(cale),"%s/%s",director,"treasure.dat");
		           int fisier=open(cale, O_RDONLY,S_IRUSR);
		           int nr=nrtreasure(fisier);
		           printf("\tNumarul treasurilor este %d \n\n", nr);
		           close(fisier);
			   break;
		    }
		  
		}
	      if(closedir(fdd)==0)
		printf("Hunt-ul a fost inchis cu succes!!\n");
	      else
		perror("Exista o problema la inchiderea Huntului\n");
	      printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	      
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
        printf("\n--------------------------------------------------\n\n");
      break;
    case SIGUSR2:
      printf("Ne ocupam de listarea treasure_urilor dintr-un hunt\n");
      handler_list_treasures();
       printf("\n--------------------------------------------------\n\n");
      break;
    case SIGTERM :
      printf("Ne ocupam de inchiderea monitorului\n");
      if(monitor_stop==0)
         handler_stop_monitor();
      else
	{printf("Minitorul a fost deja inchis!\n");
	  exit(0);
	}
       printf("\n--------------------------------------------------\n\n");
      break;
    case SIGINT:
      printf("Ne ocupam de deschiderea unui HUNT, cu scopul analizei unui treasure\n");
      handler_view_treasure();
       printf("\n--------------------------------------------------\n\n");
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

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

struct stringtoValue{
  const char *key;
  int op;
};

struct stringtoValue words[]={
  {"start_monitor",1},
  {"list_hunts",2},
  {"list_treasures",3},
  {"view_treasure",4},
  {"stop_monitor",5},
  {"exit",6}
};
pid_t monitor_pid=-1;
volatile sig_atomic_t monitor_execution=0;
volatile sig_atomic_t monitor_stop=0;
void write_intxt(const char *c)
{
  int fd=open("comenzi.txt", O_WRONLY|O_APPEND|O_CREAT, 0666);
  if(fd==-1)
    {
      perror("eroare la deschiderea fisierului de scris \n");
      return;
    }
  size_t len=sizeof(char)*strlen(c);
  if(write(fd,c,len)!=len)
    {
      perror("eroare la scriere \n");
      close(fd);
      return ;
    }
  close(fd);
}
int command(const char *buff)
{
  int i;
  for(i=0; i<sizeof(words)/sizeof(words[0]);i++)
    if(strcmp(words[i].key,buff)==0)
      return words[i].op;
  return 0;
}
void handler_view_treasure()
{
  char op[Max];
  char *hunt, *tres;
   snprintf(op,Max,"%s",readdetails());
   char *len=strchr(op,' ');
   if(len==NULL)
     {
       printf("Comanda invalida ca nu am gasit spatiu 1\n");
       return;
     }
   len++;
   char *len2=strchr(len,' ');
    if(len2==NULL)
     {
       printf("Comanda invalida ca nu am gasit spatiu 2\n");
       return;
     }
    *len2='\0';
    hunt=len;
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
   
    }
  
}
int main()
{ struct sigaction sa;
  sa.sa_handler=setup_for_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=0;
  sigaction(SIGCHLD,sa,NULL);
 
  return 0;
}

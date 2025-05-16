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
#define Max2 200

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
  
  char buff2[50],buf3[50],comprim[Max2];
    printf("Introduceti pentru inceput hunt-ul\n");
    fgets(buff2,50,stdin);
    buff2[strcspn(buff2,"\n")]='\0';
    printf("Introduceti acum treasure-ul cautat \n");
    fgets(buf3,50,stdin);
    buf3[strcspn(buf3,"\n")]='\0';
    snprintf(comprim, Max2,"--view_treasure %s %s\n",buff2,buf3);
    write_intxt(comprim);
  printf("%s %s\n",buff2,buf3);
  if(monitor_stop==0)
    view(buff2,buf3);
  else
    printf("Eroare, monitor posibil inchis!\n");
}
void handler_list_treasures()
{
  char buff2[50], comprim[Max2];
    printf("Introduceti numele huntului \n");
    fgets(buff2,50,stdin);
    buff2[strcspn(buff2,"\n")]='\0';
  if(monitor_stop==0)
    {
      snprintf(comprim,Max2,"list_treasures %s",buff2);
      write_intxt(comprim);
      list(buff2);
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
  char cale[POSIX1_], director[Max2];
  DIR *fd=opendir(".");
  write_intxt("list_hunts");
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
void setup_for_sigaction(int sig)
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
void setup(int sig)
{
  struct sigaction sa;
  sa.sa_handler=setup_for_sigaction;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  sigaction(sig,&sa,NULL);
}
int main()
{ 
  printf("Urmatoarele comenzi disponibile pentru aceasta interfata sunt:\n\t start_monitor---pornirea monitorului,care este obligatorie:\n\t list_hunts---afiseaaza hunturile si numarul loc\n\t list_treasures---afiseaza toate treasure-urile din hunt-ul respectiv\n\t view_treasure---afiseaza detaliile legate de un treasure, dintr-un hunt\n\t stop_monitor---o actiune necesara la fial, de oprire a monitorului\n\t exit---o actiune care verifica daca monitorul inca ruleaza, in caz afirmativ printeaza o eroare, altfel il opreste\n\nIntroduceti comanda dorita:\n");
  char buff[Max2];
  while(1)
 {
  
    fgets(buff,Max2, stdin);
    buff[strcspn(buff, "\n")]='\0';
    if(command(buff)==1)
      {printf("Se verificam daca monitorul este pornit pentru inceput\n");

	if(monitor_execution==1 &&  monitor_pid>0 )
        	{
		  printf("Monitorul functioneaza cu acest pid %d\n",monitor_pid);		  
		}
	 else
	   {  printf("Monitorul era/este pornit!\n");
		  monitor_pid=fork();
		  if(monitor_pid<0)
		    {
		      perror("Eroare la fork : ");
		      exit(-1); 
		    }
		  monitor_execution=1;
		  if(monitor_pid==0)
		    {
		      setup(SIGUSR1);
		      setup(SIGUSR2);
		      setup(SIGTERM);
		      setup(SIGINT);
		      printf("Monitorul este pornit cu pidul urmator %d ",getpid());
		    }
	   }
	 printf("\n\n--------------------------------------------------\n\n");
      }
    else
      {
          if(monitor_pid==-1)
              {
        	printf("Porneste monitorul pentru inceput!\n");
        	exit(1);
              }
	   printf("Introduceti comenzile corespunzatoare:\n\n");
          switch(command(buff))
           {
            case 2:printf("S-a optat pentru listarea hunts\n");
               kill(monitor_pid,SIGUSR1);
	       printf("\n\n--------------------------------------------------\n\n");
              break;
	   case 3:printf("S-a optat pentru listarea treasure-urilor\n");
              kill(monitor_pid,SIGUSR2);
	       printf("\n\n--------------------------------------------------\n\n");
              break;
            case 4:printf("S-a optat pentru vizualizarea treasure-ului specificat \n");
              kill(monitor_pid,SIGINT);
	       printf("\n\n--------------------------------------------------\n\n");
           break;
           case 5:printf("S-a optat pentru oprirea monitorului \n");
	     write_intxt("stop_monitor");
	     monitor_stop=1;
	     kill(monitor_pid,SIGTERM);
	      printf("\n\n--------------------------------------------------\n\n");
           break;
           case 6:printf("S-a optat pentru oprirea fortata a procesului\n");
	     if(monitor_pid!=0 && monitor_stop!=1)
	       {
		 write_intxt("exit");
		 if(monitor_stop!=1)
		   { printf("Mai intai trebuie oprirea monitorului !!");
		   }
		 kill(monitor_pid,SIGTERM);
		  
	       }
	     else
	       {
		  printf("\n\n--------------------------------------------------\n\n");
		 exit(0);
	       }
	     break;
	   default:printf("Comanda necunoscuta \n");
          }
       }
   
       }
 
  return 0;
}

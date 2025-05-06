#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>
#include<string.h>
#include<error.h>
#include<sys/wait.h>
#include<unistd.h>
#define Max 256
#define MAX 356
struct stringtoValue{
  const char *key;
  int op;
};

struct stringtoValue words[]={
  {"start_monitor",1},
  {"list_hunts",2},
  {"list_treasures",3},
  {"view_treasures",4},
  {"stop_monitor",5},
  {"exit",6}
};
pid_t monitor_pid=-1;
volatile sig_atomic_t monitor_execution=0;
volatile sig_atomic_t monitor_stop=0;
void write_intxt(const char *c)
{
  int fd=open("comenzi.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
  if(fd==-1)
    {
      perror("eroare la deschiderea fisierului de scris \n");
      return;
    }
  size_t len=sizeof(char)*strlen(c);
  if(write(fd,c,len)!=len)
    {
      perror("eroare ;a citire \n");
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
void verificare(int sig)
{int status;
  pid_t oo=waitpid(monitor_pid,&status,0);
  if(oo==-1)
    {
      perror("Eroare la waitpid : ");
      return;
    }
  
  monitor_execution=-1;
  if(WIFEXITED(status))
    {
      printf("Monitorul s-a inchis cu urmatorul cod  %d  (0---inseamna succes, altceva----probleme)\n", WEXITSTATUS(status));
    }
  else
    printf("Monitorul nu s-a inchis normal\n");
}
void sendcommand(const char *cc)
{
  if(strcmp(cc, "--list_hunt")==0)
    {
      write_intxt(cc);
      if(kill(monitor_pid, SIGUSR1)==-1)
	{ perror("Trimiterea semnalului a dat o eroare ");
	}
      else
	{ printf("S-a trimis semnalul pentru list_hunt \n"); 
	}
    }
  if(strncmp(cc,"--list_treasures ",17)==0)
    {
      write_intxt(cc);
      if(kill(monitor_pid,SIGUSR2)==-1)
	{ perror("Trimiterea semnalului a dat o eroare ");
	}
      else
	{ printf("S-a trimis semnalul pentru list_treasures\n"); 
	}
    }
  if(strncmp(cc,"--view_treasure ",16)==0)
    {
      write_intxt(cc);
      if(kill(monitor_pid,SIGINT)==-1)
	{
	  perror("Trimiterea semnalului a dat o eroare ");
	}
      else
	{
	  printf("S-a trimis semnalul pentru view_treasure\n"); 
	}
    }
  if(strcmp(cc, "--stop_monitor")==0)
    {
     
       if(kill(monitor_pid,SIGTERM)==-1)
	{
	  perror("Trimiterea semnalului a dat o eroare ");
	}
      else
	{
	  printf("S-a trimis semnalul pentru stop_monitor\n"); 
	}
    }
  
}

int main()
{
  
  struct sigaction sa;
  sa.sa_handler=verificare;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  sigaction(SIGCHLD,&sa,NULL);
  printf("Urmatoarele comenzi disponibile pentru aceasta interfata sunt:\n\t start_monitor---pornirea monitorului,care este obligatorie:\n\t list_hunts---afiseaaza hunturile si numarul loc\n\t list_treasures---afiseaza toate treasure-urile din hunt-ul respectiv\n\t view_treasure---afiseaza detaliile legate de un treasure, dintr-un hunt\n\t stop_monitor---o actiune necesara la fial, de oprire a monitorului\n\t exit---o actiune care verifica daca monitorul inca ruleaza, in caz afirmativ printeaza o eroare, altfel il opreste\n\nIntroduceti comanda dorita\n");
  char buff[Max],buff2[Max], comprim[MAX], buf3[70];
  while(1)
 {
    fgets(buff,Max, stdin);
    buff[strcspn(buff, "\n")]='\0';
    if(command(buff)==1)
      {printf("Se verificam daca monitorul este pornit pentru inceput\n");
         if(monitor_execution==0 &&  monitor_pid==-1 )
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
	           execl("./monitor", "./monitor",NULL);
		   /// perror("eroare pentru execl\n");
	           exit(0);
        	}
	   printf("Monitorul functioneaza cu acest pid %d\n",monitor_pid);
         }
	 else
	   {
	           printf("Monitorul s a oprit!\n");
	            exit(1);
	   }
      }
    else
      {
          if(monitor_pid==-1)
              {
        	printf("Porneste monitorul pentru inceput!\n");
        	exit(1);
              }
          switch(command(buff))
           {
            case 2:printf("S-a optat pentru listarea hunts\n");
              sendcommand("--list_hunts" );
              break;
            case 3:printf("S-a optat pentru listarea treasure-urilor\n");
	      printf("Introduceti numele huntului \n");
              fgets(buff2,Max,stdin);
              buff2[strcspn(buff2,"\n")]='\0';
              snprintf(comprim, MAX,"--list_treasures");
              sendcommand(comprim);
              break;
            case 4:printf("S-a optat pentru vizualizarea treasure-ului specificat \n");
              printf("Introduceti pentru inceput hunt-ul\n");
              fgets(buff2,Max,stdin);
              buff2[strcspn(buff2,"\n")]='\0';
              printf("Introduceti aum treasure-ul cautat \n");
              fgets(buf3,70,stdin);
              buf3[strcspn(buf3,"\n")]='\0';
              snprintf(comprim, MAX,"--view_treasure");
              sendcommand(comprim);
           break;
           case 5:printf("S-a optat pentru oprirea monitorului \n");
	     snprintf(comprim,Max,"--stop_monitor");
	     sendcommand(comprim);
	     monitor_stop=1;
           break;
           case 6:printf("S-a optat pentru oprirea fortata a procesului\n");
	     if(monitor_pid!=0 && monitor_stop!=1)
	       {
		 printf("Mai intai trebuie oprirea monitorului !!");
		  sendcommand("--stop_monitor");
	       }
	     else
	      exit(0);
	     break;
	   default:printf("Comanda necunoscuta \n");
          }
       }
    }
  
  return 0;
}

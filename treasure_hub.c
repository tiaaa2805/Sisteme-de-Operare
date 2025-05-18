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

#define Max2 60
#define Maxx2 260
#define Max22 300
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
  {"exit",6},
  {"calculate_score",7}
};
pid_t monitor_pid=-1;
volatile sig_atomic_t monitor_execution=0;
volatile sig_atomic_t monitor_stop=0;

volatile sig_atomic_t usr1=0;
volatile sig_atomic_t usr2=0;
volatile sig_atomic_t term=0;
volatile sig_atomic_t ing=0;

char hunt[Max2], tres[Max2];
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
  if(monitor_stop==0)
    view(hunt,tres);
  else
    printf("Eroare, monitor posibil inchis!\n");
}
void handler_list_treasures()
{
  printf("hdhfhdf"); 
  if(monitor_stop==0)
    {
      list(hunt);
    }
  else
    printf("Eroare, monitor posibil inchis!\n");
}
void handler_stop_monitor()
{printf("Monitorul se va inchide in 5 secunde\n");
    usleep(5000000);
    printf("Monitorul se afla in procesul de inchidere\n");
    monitor_stop=1;  
}
int este_director(const char *oo)
{
  struct stat st;
  return ((stat(oo,&st)==0) && S_ISDIR(st.st_mode));
}
void handler_list_directory()
{
  char cale[Max22], director[Maxx2];
  DIR *fd=opendir(".");
  write_intxt("list_hunts\n");
  struct dirent *citim;
  if(fd)
    {
      while((citim=readdir(fd))!=NULL){
       	  if(strcmp(citim->d_name,".")==0 || strcmp(citim->d_name,"..")==0 || strcmp(citim->d_name,".git")==0)
	    continue;
	  if(este_director(citim->d_name))
	    {
	      snprintf(director,Maxx2,"%s",citim->d_name);
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
      usr1=1;
      break;
    case SIGUSR2:
      printf("Ne ocupam de listarea treasure_urilor dintr-un hunt\n");
      usr2=1;
      break;
    case SIGTERM :
      printf("Ne ocupam de inchiderea monitorului\n");
      term=1;
      break;
    case SIGINT:
      printf("Ne ocupam de deschiderea unui HUNT, cu scopul analizei unui treasure\n");
      ing=1;
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
  printf("Urmatoarele comenzi disponibile pentru aceasta interfata sunt:\n\t start_monitor---pornirea monitorului,care este obligatorie:\n\t list_hunts---afiseaaza hunturile si numarul loc\n\t list_treasures---afiseaza toate treasure-urile din hunt-ul respectiv\n\t view_treasure---afiseaza detaliile legate de un treasure, dintr-un hunt\n\t stop_monitor---o actiune necesara la fial, de oprire a monitorului\n\t exit---o actiune care verifica daca monitorul inca ruleaza, in caz afirmativ printeaza o eroare, altfel il opreste\n\t calculate_score--- calculeaza scorul per hunt\nIntroduceti comanda dorita:\n");
  char buff[Max2], comprim[Max22], buf2[Maxx2];
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
	     write_intxt("start_monitor\n");
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
		      printf("Monitorul este pornit cu pidul urmator %d \n",getpid());
		      while(1)
			{
			  pause();
			  if(usr1)
			    {
			        handler_list_directory();
				usr1=0;
			    }
			  if(usr2)
			    {
			      int fdd=open("fisier.txt",O_RDONLY,0666);
			      if(fdd==-1)
				{
				  perror("ERoare la deschidere fisier \n");
				}
			      else
				{
				  ssize_t len=read(fdd,hunt,Max2-1);
				  if(len>0)
				    {
				      hunt[len]='\0';
				      hunt[strcspn(hunt,"\n")]='\0';
				    }
				  else
				    {
				      perror("Eroare la citirea din fisierul temporar \n");
				    }
				  close(fdd);
				}
			      handler_list_treasures();
			      usr2=0;
			    }
			  if(term)
			    {
			      handler_stop_monitor(); 
			      break;
			    }
			  if(ing)
			    {
			       int fdd=open("fisier.txt",O_RDONLY,0666);
			      if(fdd==-1)
				{
				  perror("ERoare la deschidere fisier \n");
				}
			      else
				{
				  ssize_t len=read(fdd,buf2,Max2-1);
				  if(len>0)
				    {
				      buf2[strcspn(buf2,"\n")]='\0';
				      int count=sscanf(buf2,"%s %s",hunt,tres);
				      printf("Countul este %d ",count);
				    }
				  else
				    {
				      perror("Eroare la citirea din fisierul temporar \n");
				    }
				  close(fdd);
				}
			      handler_view_treasure();
			      ing=0;
			    }
			}
		      exit(0);
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
          switch(command(buff))
           {
            case 2:printf("S-a optat pentru listarea hunts\n");  
               kill(monitor_pid,SIGUSR1);
	       printf("\n\n--------------------------------------------------\n\n");
              break;
	   case 3:printf("S-a optat pentru listarea treasure-urilor\n");
	      printf("Introduceti pentru inceput hunt-ul\n");
	      fgets(hunt,Max2,stdin);
	      hunt[strcspn(hunt,"\n")]='\0';
	      int fdd=open("fisier.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	      if(fdd==-1)
		{
		  perror("Eroare la deschiderea fisierului \n");
		}
	      else
		{
		  write(fdd,hunt,strlen(hunt));
		  close(fdd);
		}
	      snprintf(comprim, Max22,"list_treasures %s\n",hunt);
	      printf("%s",comprim);
	      write_intxt(comprim);
              kill(monitor_pid,SIGUSR2);
	       printf("\n\n--------------------------------------------------\n\n");
              break;
            case 4:printf("S-a optat pentru vizualizarea treasure-ului specificat \n");
	        printf("Introduceti pentru inceput hunt-ul\n");
		fgets(hunt,Max2,stdin);
       	        hunt[strcspn(hunt,"\n")]='\0';
       	        printf("Introduceti acum treasure-ul cautat \n");
		fgets(tres,Max2,stdin);
		tres[strcspn(tres,"\n")]='\0';
		fdd=open("fisier.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
		 snprintf(buf2,Maxx2,"%s %s",hunt,tres);
	      if(fdd==-1)
		{
		  perror("Eroare la deschiderea fisierului \n");
		}
	      else
		{
		  write(fdd,buf2,strlen(buf2));
		  close(fdd);
		}
		snprintf(comprim, Max22,"view_treasure %s %s\n",hunt,tres);
		printf("%s\n",comprim);
		write_intxt(comprim);
              kill(monitor_pid,SIGINT);
	       printf("\n\n--------------------------------------------------\n\n");
           break;
           case 5:printf("S-a optat pentru oprirea monitorului \n");
	     write_intxt("stop_monitor\n");
	     kill(monitor_pid,SIGTERM);
	     waitpid(monitor_pid,NULL,0);
	     monitor_pid=-1;
	     monitor_execution=0;
	     monitor_stop=1;
	      printf("\n\n--------------------------------------------------\n\n");
	      printf("Inchidem monitorul\n");
	      exit(0);
           break;
           case 6:printf("S-a optat pentru oprirea fortata a procesului\n");
	     if(monitor_pid!=0 && monitor_stop!=1)
	       {
		 write_intxt("exit\n");
		 if(monitor_stop!=1)
		   { printf("Mai intai trebuie oprirea monitorului!!\n");
		   }
	       }
	     printf("Iesim din program\n");
		  exit(0);
	     break;
	   case 7:printf("Introduceti numele hunt-ului asupra caruia se calculeaza scorul \n");
	     fgets(hunt,Max2,stdin);
	     printf("Hunt-ul este %s\n",hunt);
	     hunt[strcspn(hunt,"\n")]='\0';
	     printf("Introduceti treasure-ul\n");
	     fgets(tres,Max2,stdin);
	     printf("treasure-ul este %s \n",tres);
	     tres[strcspn(tres,"\n")]='\0';
	     snprintf(comprim,Maxx2,"calculate_score %s %s",hunt,tres);
	     write_intxt(comprim);
	     int ff[2];
	     if(pipe(ff)==-1)
	       {
		 perror("eroare la pipe ");
		 break;
	       }
	     pid_t pp=fork();
	     if(pp<0)
	       {
		 perror("Eroare la fork ");
		 break;
	       }
	     if(pp==0)
	       {
		 close(ff[0]);
		 dup2(ff[1],STDOUT_FILENO);
		 close(ff[1]);
		 execl("./calculator", "calculator", hunt,tres,NULL);
		 perror("Eroare la trimitere\n");
		 exit(1);
	       }
	     else
	       {
		 close(ff[1]);
		 char bufi[256];
		 ssize_t n;
		 while((n=read(ff[0],bufi,sizeof(bufi)-1))>0)
		   {
		     bufi[n]='\0';
		     printf("%s ",bufi);
		   }
		 close(ff[0]);
		 waitpid(pp,NULL,0);
	       }
	     break;
	   default:printf("Comanda necunoscuta \n");
          }
       }
       }
  return 0;
}

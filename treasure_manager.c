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
void functie(int sig)
{
  printf("Am primit semnalele !!\n");
  fflush(stdout);
  char operatia[Max], hunt[Max], treas[Max];
  int tre;
  if(fgets(operatia,Max,stdin)==NULL)
    {
      printf("Nu s-a citit nicio operatie !!!!\n");
      return;
    }
  operatia[strcspn(operatia,"\n")]='\0';
   char *use=userr();
   if(strncmp(operatia,"--add ", 6)==0)
    {
      printf("Ati optat pentru introducerea datelor legate treasure \n");
      treasure tr;
      snprintf(hunt,Max,"%s",operatia+6);
      if(adaugarea(&tr)==1 && add(&tr, hunt)==1)
	    {
	      printf("Am creat noul joc/adaugat noul treasure in acest hunt \n");
	       istoric_log(hunt,use,"--add");
	    }
       else
	 {
	   printf("Nu s a putut crea noul joc \n");
	 }
    }
  else{
    if(strncmp(operatia,"--list ", 7)==0)
	    {
	      snprintf(hunt,Max,"%s ", operatia+7);
	      printf("Ati optat pentru listarea datelor \n");
	      list(hunt);
	      istoric_log(hunt,use,"--list");
	    }
	  else{
	    if(strncmp(operatia,"--view ", 7)==0)
		   {
		     printf("Ati optat pentru listarea datelor legate de un treasure \n");
		       snprintf(hunt,Max,"%s ", operatia+7);
		       int localizare=strcspn(hunt," ");
		       snprintf(treas,Max,"%s",hunt+localizare);
		     view(hunt,treas);
		     istoric_log(hunt,use,"--view");
		   }
		  else {
		    if(strncmp(operatia,"--remove_hunt ", 14)==0)
			   {
			     printf("Ati optat pentru stergerea unui hunt \n");
			       snprintf(hunt,Max,"%s ", operatia+14);
			      istoric_log(hunt,use,"--remove_hunt");
			     remove_hunt(hunt);
			   }
			 else {
			   if(strncmp(operatia,"--remove_treasure ", 18)==0)
			     {int idd;
				     printf("Ati optat pentru stergerea unui treasure din huntul %s\n",hunt);
				      snprintf(hunt,Max,"%s ", operatia+18);
				      int localizare=strcspn(hunt," ");
		                      snprintf(treas,Max,"%s",hunt+localizare);
				      idd=atoi(treas);
				      istoric_log(hunt,use,"--remove_treasure");
				     remove_treasure(hunt,idd);
				    
				   }
				 else
				   {
				     if(strcmp(operatia,"--stop_monitor")==0)
				       {
					 printf("Monitorul se v-a opri!\n");
					 usleep(5000000);
					 exit(-1);
				       }
				     else
				       {
					 printf("Nu ati introdus niciuna dintre operatiile propuse! \n");
				       }
				   }
			 }
		  }
	  }
  }
  
 
}
int main()
{
  struct sigaction sa;
  sa.sa_handler=functie;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  sigaction(SIGCHLD,&sa,NULL);
  printf("Acum primim semnalele \n");
  while(1)
    pause();
  return 0;
}

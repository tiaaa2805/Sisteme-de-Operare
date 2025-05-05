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

int main(int argc, char *argv[])
{
  if(argc<2)
    {
      printf("Prea putine argumente \n");
      return 0;
    }
   char operatia[Max], hunt[Max], treas[Max];
   strcpy(operatia,argv[1]);
 
  
   char *use=userr();
   if(strcmp(operatia,"--add")==0)
    {
      printf("Ati optat pentru introducerea datelor legate treasure \n");
      treasure tr;
      strcpy(hunt,argv[2]);
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
    if(strcmp(operatia,"--list")==0)
	    {
	       strcpy(hunt,argv[2]);
	      printf("Ati optat pentru listarea datelor \n");
	      list(hunt);
	      istoric_log(hunt,use,"--list");
	    }
	  else{
	    if(strcmp(operatia,"--view" )==0)
		   {
		     printf("Ati optat pentru listarea datelor legate de un treasure \n");
		       strcpy(hunt,argv[2]);
		        strcpy(treas,argv[3]);
		     view(hunt,treas);
		     istoric_log(hunt,use,"--view");
		   }
		  else {
		    if(strcmp(operatia,"--remove_hunt")==0)
			   {
			     printf("Ati optat pentru stergerea unui hunt \n");
			        strcpy(hunt,argv[2]);
			      istoric_log(hunt,use,"--remove_hunt");
			     remove_hunt(hunt);
			   }
			 else {
			   if(strcmp(operatia,"--remove_treasure")==0)
			     {int idd;
				     printf("Ati optat pentru stergerea unui treasure din huntul %s\n",hunt);
				       strcpy(hunt,argv[2]);
				       strcpy(treas,argv[3]);
				      idd=atoi(treas);
				      istoric_log(hunt,use,"--remove_treasure");
				     remove_treasure(hunt,idd);
				    
				   }
			   /*else
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
				      }*/
			 }
		  }
	  }
  }
  
 
  return 0;
}

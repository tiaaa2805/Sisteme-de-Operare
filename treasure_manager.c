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
   snprintf(hunt,Max,"%s",argv[2]);
   snprintf(treas,Max,"%s",argv[3]);
   char *use=userr();
   if(strcmp(operatia,"--add")==0)
    {
      printf("Ati optat pentru introducerea datelor legate treasure \n");
      treasure tr;
      
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
	       
	      printf("Ati optat pentru listarea datelor \n");
	      list(hunt);
	      istoric_log(hunt,use,"--list");
	    }
	  else{
	    if(strcmp(operatia,"--view" )==0)
		   {
		     printf("Ati optat pentru listarea datelor legate de un treasure \n");
		       
		     view(hunt,treas);
		     istoric_log(hunt,use,"--view");
		   }
		  else {
		    if(strcmp(operatia,"--remove_hunt")==0)
			   {
			     printf("Ati optat pentru stergerea unui hunt \n");
			        
			      istoric_log(hunt,use,"--remove_hunt");
			     remove_hunt(hunt);
			   }
			 else {
			   if(strcmp(operatia,"--remove_treasure")==0)
			     {int idd;
				     printf("Ati optat pentru stergerea unui treasure din huntul %s\n",hunt);
				       
				      idd=atoi(treas);
				      istoric_log(hunt,use,"--remove_treasure");
				     remove_treasure(hunt,idd);
				    
				   }
			 
			 }
		  }
	  }
  }
  
 
  return 0;
}

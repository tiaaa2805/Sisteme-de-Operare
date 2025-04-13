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
#include<stdbool.h>
#include"treasur.h"


int main(int argc, char **argv)
{
  if(argc<3)
    {printf("Va rog introduceti numarul corect de argumente");
      return 1;
    }
  char *operatia=argv[1];
  char *hunt=argv[2];
  char *treas=argv[3];
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
	         if(strcmp(operatia,"--view")==0)
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
				   {int idd=atoi(treas);
				     printf("Ati optat pentru stergerea unui treasure din huntul %s\n",hunt);
				      istoric_log(hunt,use,"--remove_treasure");
				     remove_treasure(hunt,idd);
				    
				   }
				 else
				   {
				     printf("Nu ati introdus niciuna dintre operatiile propuse! \n");
				   }
			 }
		  }
	  }
  }
  
 
  return 0;
}

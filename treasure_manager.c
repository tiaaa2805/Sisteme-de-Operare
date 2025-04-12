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
  
  if(strcmp(operatia,"--add")==0)
    {printf("Ati optat pentru introducerea datelor legate treasure \n");
      treasure tr;
      adaugarea(&tr);
      if(add(&tr, hunt)==1)
	printf("Am creat noul joc ");
      else
	printf("Nu s a putut crea noul joc ");
    }
  if(strcmp(operatia,"--list")==0)
    {
      list(hunt);
    }
  if(strcmp(operatia,"--view")==0)
    {
      view(hunt,treas);
    }
  if(strcmp(operatia,"--remove_hunt")==0)
    {
    }
  if(strcmp(operatia,"--remove_treasure")==0)
    {
    }
  
  
  return 0;
}

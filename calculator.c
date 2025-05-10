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
int getvalue(char *hunt, char *tres)
{
  int valoare;
   char cale[Max];
  DIR *huntdir=opendir(hunt);
  struct dirent *oo;
  if(huntdir==NULL)
    {
      perror("Eroare la deschiderea huntului pentru cautarea valorii\n");
      exit(-1);
    }
  while((oo=readdir(huntdir)))
    {
      if(strcmp(oo->d_name,".")==0 || strcmp(oo->d_name,"..")==0)
	continue;
      snprintf(cale,Max, "%s/%s",hunt,"treasure.dat");
      int fdfisier=open(cale, O_RDONLY,0666);
      treasure tr;
      if(fdfisier==-1)
	{
	  perror("Eroare la deschiderea fisierului pentru citirea treasure-ului\n");
	  closedir(huntdir);
	  exit(-1);
	}
      while(read(fdfisier,&tr,sizeof(treasure))==sizeof(treasure))
	{
	  if(strcmp(tres,tr.name)==0)
	    {
	      valoare=tr.value;
	      break;
	    }
	}
      close(fdfisier);
      return valoare;
      
    }
  closedir(huntdir);
  return 0;
}
int main(int argc, char *argv[])
{
  if(argc<3)
    {
      printf("Intrebuiau introduse numele hunt-ului, numele utilizatorului si numele treasure-lui\n");
      exit(-1);
    }
  char *hunt=argv[1];
  char *tres=argv[2];
 
  int fd=open("results.txt", O_APPEND|O_CREAT|O_WRONLY, S_IWUSR);
  if(fd==-1)
    {
      perror("Eroare la deschiderea fisierului pentru scrierea comenzilor\n");
      exit(-1);
    }
  int val=getvalue(hunt,tres);
  char *utilizator=userr(), implementare[Max];
  snprintf(implementare,Max,"%s %d\n",utilizator,val);
  if(write(fd,implementare,sizeof(implementare))!=sizeof(implementare))
    {
      perror("eroare la scrierea in fisier\n");
      close(fd);
      exit(1);
    }
  close(fd);
  return 0;
}

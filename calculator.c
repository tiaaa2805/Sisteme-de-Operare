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
#define Max 256
typedef struct{
  char user[Max];
  int points;
}results;
int getvalue(char *hunt, char *tres)
{
  int valoare=0;
   char cale[Max];
  DIR *huntdir=opendir(hunt);
  if(huntdir==NULL)
    {
      perror("Eroare la deschiderea huntului pentru cautarea valorii\n");
      exit(-1);
    }
 
      snprintf(cale,Max, "%s/%s",hunt,"treasure.dat");
    
      int fdfisier=open(cale, O_RDONLY,S_IRUSR);
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
  closedir(huntdir);
  return valoare;
}
void check( int fd, results *implementare)
{
  results o;int ok=0;
  while((read(fd,&o,sizeof(results)))==sizeof(results))
    {
      if(strcmp(o.user,implementare->user)==0)
	{
	  implementare->points+=o.points;
	  lseek(fd,-sizeof(results),SEEK_CUR);
	  ok=1;
	  if(write(fd,implementare,sizeof(results))!=sizeof(results))
	    {
	      perror("eroare la scrierea in fisier\n");
	      
	      exit(1);
	    }
	  break;
	}
    }
  if(ok==0)
    {printf("suntem aici ");
       if(write(fd,implementare,sizeof(results))!=sizeof(results))
	    {
	      perror("eroare la scrierea in fisier\n");
	      
	      exit(1);
	    }
    }
  
}
 
int main(int argc, char *argv[])
{
  if(argc<3)
    {
      printf("Trebuiau introduse numele hunt-ului si numele treasure-lui\n");
      exit(-1);
    }
  char *hunt=argv[1];
  char *tres=argv[2];

  int fd=open("results.txt", O_RDWR|O_CREAT, 0644);
  if(fd==-1)
    {
      perror("Eroare la deschiderea fisierului pentru scrierea comenzilor\n");
      exit(-1);
    }
  int val=getvalue(hunt,tres);
  printf("%d\n",val);
  char *utilizator=userr();
  results implementare;
  snprintf(implementare.user,Max,"%s",utilizator);
  implementare.points=val;
  lseek(fd,0,SEEK_SET);
  check(fd,&implementare);
  results o;
    lseek(fd,0,SEEK_SET);
   while((read(fd,&o,sizeof(results)))==sizeof(results))
    {
      printf("Utilizatorul este : %s, iar numarul sau de puncte este %d\n",o.user,o.points);
    }
  /// printf("Scriu:%s \n",implementare);
  close(fd);
  return 0;
}

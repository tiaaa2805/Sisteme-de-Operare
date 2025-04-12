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

#define MAX 50
#define MAXCLUE 200
#define MAXSTR 200


void adaugarea(treasure *tr)
{
   printf("Introduceti id ul\n");
      scanf("%d",&tr->id);
      printf("Introduceti numele \n");
      scanf("%s",tr->name);
      printf("coordonatele corecte \n");
      scanf("%f %f", &tr->coordonate.longitudine, &tr->coordonate.latitudine);
      printf("introduceti indiciul\n ");
      getchar();
      fgets(tr->clue,200,stdin);
      tr->clue[strspn(tr->clue,"\n")]='\0';
      printf("introduceti value \n");
      scanf("%d", &tr->value);
}
int createdir(const char *hunt)
{char sir[MAXSTR];
  snprintf(sir,MAXSTR,"%s", hunt);
  int pp=mkdir(sir,0777);
    if(pp==-1){
      if(errno==EEXIST)
      {perror(" Acest hunt exista deja \n");
       return 1;
      }
    }

  printf(" S-a creat cu succes noul hunt \n");
  return 0;
}
int add(treasure *tr, const char *hunt)
{
  char sir[MAXSTR];
    snprintf(sir,MAXSTR,"%s/treasure.dat", hunt);
    if(createdir(hunt)==0)
    { int fd=open(sir, O_CREAT| O_RDWR| O_APPEND,0666);
      if(fd==-1)
	{
	  perror("Eroare la deschidere fisier treasure");
	  return 0;
	}
    
      lseek(fd,0,SEEK_END);
      if(write(fd,tr,sizeof(treasure))!=sizeof(treasure))
	{
	  perror("Eroare la scriere fisier treasure");
	  close(fd);
	  return 0;
	}
    }
      return 1;
}
void list(const char *hunt)
{
  char sir[MAXSTR],sir1[MAXSTR];
  struct stat st1;
    snprintf(sir1,MAXSTR,"%s",hunt);
    if(stat(sir1,&st1)!=0){
      perror("Eroare la director ");
      return;
    }
   snprintf(sir,MAXSTR,"%s/treasure.dat", hunt);
    struct stat st;
    if(stat(sir,&st)<0){
      perror("Nu s au adaugat informatiile fisierului\n");
      return;
    }
  printf("Numele hunt-ului este : %s\n", hunt);
  printf("Dimensiunea lui : %ld bytes \n", st.st_size);
  printf("Ultima modificare : %s ", ctime(&st.st_mtime));
  int fd=open(sir,O_RDONLY);
  if(fd==-1)
    {
      perror("Eroare ");
      return;
    }
  treasure t;
  lseek(fd,0,SEEK_SET);
  while(read(fd,&t,sizeof(treasure))==sizeof(treasure))
    {
      printf("Treasure id : %d \n Numele : %s \n Clue : %s\n Value : %d \n Coordonatele : %f,%f \n\n", t.id, t.name,t.clue,t.value,t.coordonate.longitudine, t.coordonate.latitudine);
    }
  close(fd);
}
void view(const char *hunt, const char *trea)
{
char sir[MAXSTR],sir1[MAXSTR];
 struct stat s1;
    snprintf(sir1,MAXSTR,"%s",hunt);
    if(stat(sir1,&s1)!=0)
      {perror("Eroare la director ");
	return;
      }
   snprintf(sir,MAXSTR,"%s/treasure.dat", hunt);
    struct stat st;
    if(stat(sir,&st)<0){
      perror("Nu s au adaugat informatiile fisierului");
      return;
    }
 printf("Numele hunt-ului este : %s\n", hunt);
  printf("Dimensiunea lui : %ld bytes \n", st.st_size);
  printf("Ultima modificare : %s ", ctime(&st.st_mtime));
  int fd=open(sir,O_RDONLY);
  if(fd==-1)
    {
      perror("Eroare ");
      return;
    }
  treasure t;
  lseek(fd,0,SEEK_SET);
  int ok=0;
  while(read(fd,&t,sizeof(treasure))==sizeof(treasure))
    {
      if(strcmp(t.name,trea)==0)
	{printf("Treasure id : %d \n Numele : %s \n Clue : %s\n Value : %d \n Coordonatele : %f,%f \n\n", t.id, t.name,t.clue,t.value,t.coordonate.longitudine, t.coordonate.latitudine);
	  
	  ok=1;
	  break;
	}
    }
  if(ok==1)
    { printf("Am gasit acest treasure hunt\n");}
  else
    { printf("Nu am gasit acest treasure hunt \n");}
   close(fd);
}
void remove_hunt(const char *hunt, int idd)
{
  
}
void remove_treasure(const char *hunt, const char *treasu)
{
}

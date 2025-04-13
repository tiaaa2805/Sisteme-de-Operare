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

int nrtreasure(int tot)
{
  treasure tr;
  int nr=0;
  lseek(tot,0,SEEK_SET);
  while(read(tot,&tr,sizeof(treasure))==sizeof(treasure))
    {
      nr++;
    }
  return nr;
  
}
void adaugarea(treasure *tr)
{
   printf("Introduceti id ul\n");
      scanf("%d",&tr->id);
      printf("Introduceti numele \n");
      scanf("%s",tr->name);
      printf("coordonatele corecte \n");
      scanf("%f %f", &tr->coordonate.longitudine, &tr->coordonate.latitudine);
      while(getchar()!='\n');
      printf("introduceti indiciul\n");
      if(fgets(tr->clue,200,stdin)==NULL)
	{
	  perror("Citire string  ");
	  return;
	}
      tr->clue[strcspn(tr->clue,"\n")]='\0';
      
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
       printf("Daca doriti sa adaugati alt treasure hunt apasati tasta y-YES, daca nu, apasati tasta n-NO\n");
       getchar();
      char o;
      scanf("%c",&o);
      
      if(o=='y')
	return 0;
      
       return 1;
      }
    }

  printf(" S-a creat cu succes noul hunt \n");
  return 0;
}
int add(treasure *tr, const char *hunt)
{printf("--------------------------------------------------\n");
  char sir[MAXSTR];
    snprintf(sir,MAXSTR,"%s/treasure.dat", hunt);
    if(createdir(hunt)==0)
    {
      int fd=open(sir, O_CREAT| O_RDWR| O_APPEND,0666);
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
        close(fd);
    }
    printf("--------------------------------------------------\n");
    
      return 1;
}
void list(const char *hunt)
{printf("--------------------------------------------------\n");
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
   printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
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
      printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }
  int nr=nrtreasure(fd);
  printf("Numarul treasurilor este %d \n\n", nr);
  printf("--------------------------------------------------\n");
  close(fd);
}
void view(const char *hunt, const char *trea)
{printf("--------------------------------------------------\n");
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
  printf("--------------------------------------------------\n");
   close(fd);
}
void remove_hunt(const char *hunt)
{printf("--------------------------------------------------\n");
  char s[MAXSTR], s1[MAXSTR];
  struct stat ss;
  snprintf(s,MAXSTR,"%s",hunt);
  if(stat(s,&ss)!=0)
    {
      perror("Eroare la director ");
      return;
    }
  struct stat str;
  snprintf(s1,MAXSTR,"%s/treasure.dat",hunt);
  if(stat(s1,&str)!=0)
    {
      perror("Eroare la fisier ");
      return;
    }
  if(remove(s1)!=0)
    {
      perror("Eroare la stergerea continutului fisierului \n");
      return;
    }
  printf("Continutul acestui hunt a fost sters complet!\n\n");
  if(rmdir(s)==-1)
    {
      perror("Eroare la stergere director");
      return ;
    }
  printf("Hunt ul a fost sters cu succes!\n\n");
  printf("--------------------------------------------------\n");
  
}
void remove_treasure(const char *hunt, int id)
{
  printf("--------------------------------------------------\n");
  char ss[MAXSTR];
  snprintf(ss,MAXSTR,"%s",hunt);
  struct stat str;
  if(stat(ss,&str)==-1)
    {
      perror("Eroare la nivelul directorului \n");
      return;
    }
  char ss1[MAXSTR];
  snprintf(ss1,MAXSTR,"%s/treasure.dat",hunt);
  struct stat stt;
  if(stat(ss1,&stt)==-1)
    {
      perror("Eroare la nivelul  fisierului \n");
      return;
    }
  int fd=open(ss1,O_RDWR, S_IXUSR);
  if(fd==-1)
    {
      perror("eroare la deschiderea fisierului \n");
      return;
    }
  lseek(fd,0,SEEK_SET);
  treasure tr;
  int n=0;
  int nr=nrtreasure(fd);
  while(read(fd,&tr,sizeof(treasure))==sizeof(treasure))
    {
      n++;
      if(tr.id==id)
	{
	  treasure tt,ttt;
	  lseek(fd,n*sizeof(treasure),SEEK_CUR);
	  while(read(fd,&tt,sizeof(treasure))==sizeof(treasure))
	    {
	      lseek(fd,(n+1)*sizeof(treasure),SEEK_CUR);
	      while(read(fd,&ttt,sizeof(treasure))==sizeof(treasure))
		{
		  tt=ttt;
		}
	    }
	    break;
	}
    }
      if(n==nr)
	printf("Nu am reusit sa gasim treasure-ul cautat\n");
      else
	printf("Am sters cu succes treasure-ul cu id ul %d \n",id);
  close(fd);
  printf("--------------------------------------------------\n");
}

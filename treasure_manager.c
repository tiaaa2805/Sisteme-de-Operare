#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<string.h>
#include <errno.h>
#include<time.h>


#define MAX 50
#define MAXCLUE 200
#define MAXSTR 200
typedef struct{
  float longitudine;
  float latitudine;
}gps;
typedef struct{
  int id;
  char name[MAX];
  gps coordonate;
  char clue[MAXCLUE];
  int value;
}treasure;
int createdir(const char *hunt)
{char sir[MAXSTR];
  snprintf(sir,MAXSTR,"%s%s","treasure_hunts/", hunt);
  int pp=mkdir(sir,0777);
    if(pp==-1){
      if(errno==EEXIST)
      {printf(" Acest hunt exista deja \n");
       return 1;
      }
    }

  printf(" S-a creat cu succes noul hunt \n");
  return 0;
}
int add(treasure *tr, const char *hunt)
{
  char sir[MAXSTR];
    snprintf(sir,MAXSTR,"%s%s","treasure_hunts/", hunt);
  
     
    if(createdir(hunt)==1)
    { int fd=open(sir, O_CREAT| O_RDWR,0666);
      if(fd==-1)
	{
	  printf("Eroare la deschidere fisier treasure");
	  return 0;
	}
    
      lseek(fd,0,SEEK_END);
      if(write(fd,tr,sizeof(treasure))!=sizeof(treasure))
	{
	  printf("Eroare la scriere fisier treasure");
	  close(fd);
	  return 0;
	}
    }
 
      return 1;
}
void list(const char *hunt)
{
   char sir[MAXSTR];
    snprintf(sir,MAXSTR,"%s%s","treasure_hunts/", hunt);
    if(strlen(sir)<0)
      {
	printf("Probleme la copierea cailor");
	return;
      }
    struct stat st;
    if(stat(sir,&st)<0){
      printf("Nu s ai adaugat informatiile fisierului");
      return;
    }
  printf("Numele hunt-ului este : %s\n", hunt);
  printf("Dimensiunea lui : %ld bytes \n", st.st_size);
  printf("Ultima modificare : %s ", ctime(&st.st_mtime));
  int fd=open(sir,O_RDONLY);
  if(fd==-1)
    {
      printf("Eroare ");
      return;
    }
  treasure t;
  lseek(fd,0,SEEK_SET);
  while(read(fd,&t,sizeof(treasure))==sizeof(treasure))
    {
      printf("Treasure id : %d \n Numele : %s \n Clue : %s \n Value : %d \n Coordonatele : %f,%f \n\n", t.id, t.name,t.clue,t.value,t.coordonate.longitudine, t.coordonate.latitudine);
    }
  close(fd);
}

int main(int argc, char **argv)
{
  if(argc<3)
    {printf("Va rog introduceti numarul corect de argumente");
      return 1;
    }
  char *operatia=argv[1];
  char *hunt=argv[2];
  if(strcmp(operatia,"--add")==0)
    {printf("Ati optat pentru introducerea datelor legate treasure \n");
      treasure tr;
      printf("Introduceti id ul\n");
      scanf("%d",&tr.id);
      printf("Introduceti numele \n");
      scanf("%s",tr.name);
      printf("coordonatele corecte \n");
      scanf("%f %f", &tr.coordonate.longitudine, &tr.coordonate.latitudine);
      printf("introduceti indiciul\n ");
      getchar();
      fgets(tr.clue,200,stdin);
      printf("introduceti value \n");
      scanf("%d", &tr.value);
      if(add(&tr, hunt)==1)
	printf("Am creat noul joc ");
      else
	printf("Nu s a putut crea noul joc ");
    }
  if(strcmp(operatia,"--list")==0)
    {
    }
  if(strcmp(operatia,"--view")==0)
    {
    }
  if(strcmp(operatia,"--remove_hunt")==0)
    {
    }
  if(strcmp(operatia,"--remove_treasure")==0)
    {
    }
  
  
  return 0;
}

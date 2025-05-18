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
#include<pwd.h>
#include"treasur.h"
#define _POSIX1_SOURCE 2
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
  lseek(tot,0,SEEK_END);
  return nr;
  
}
char *userr()
{
  uid_t id=getuid();
  struct passwd *pw=getpwuid(id);
  if(pw)
    return pw->pw_name;
  return "unknown";
}
void istoric_log(const char *hunt,const char *user, const char *operatie )
{
  char s[MAXSTR];
  snprintf(s,MAXSTR,"%s/logged_hunt.txt",hunt);
  int fd=open(s,O_CREAT|O_RDWR|O_APPEND, 0644);
  if(fd==-1)
    {
      perror("Eroare la deschidere logged_hunt ");
      return;
    }
  lseek(fd,0,SEEK_END);
  char ss[MAXSTR];
  time_t oo=time(NULL);
  char *timee=ctime(&oo);
  timee[strcspn(timee,"\n")]='\0';
  snprintf(ss,MAXSTR,"Userul:%s a realizat actiunea:%s;\nasupra: %s;\n detalii [%s] \n",user,operatie,hunt,timee);
  ssize_t len=sizeof(char)*strlen(ss);
  if(write(fd, ss,len)!=len)
    {
      perror("Eroare la scrierea in acest fisier logg \n");
      close(fd);
      return;
    }
  char symbolic[MAXSTR];
  snprintf(symbolic,MAXSTR, "logged_hunt-%s",hunt);
  struct stat st;
  if(lstat(symbolic,&st)==-1)
    {
      if(symlink(s,symbolic)==-1)
	{
	  perror("Eroare la crearea legaturii simbolice :");
	  close(fd);
	  return;
	}
    }
 
  close(fd);
}
bool checkcoordonate(double a, double b)
{
  if(a<-90.00 || a>90.00)
    return false;
  if(b<-180.00 || b>180.00)
    return false;
  return true;
}
int adaugarea(treasure *tr)
{
  double a,b;
   printf("Introduceti id ul\n");
      scanf("%49s",tr->id);
      printf("Introduceti numele \n");
      scanf("%49s",tr->name);
      printf("Coordonatele corecte, cuprinse in intervalele latitudinea-(-90.00,90.00) si longitudinea-(-180.00,180.0) \nATENTIE! TREBUIE SA FIE DE FORMA x y, cu spatiu intre ele\n");
      scanf("%lg %lg", &a, &b);
      if(checkcoordonate(a,b)==false)
	{
	  printf("Coordonatele nu satisfac cerintele generale! \nVa rugam sa le modificai sa fie in intervalul dat!\nMisiune abandonata\n");
	  return 0;
	}
      tr->coordonate.latitudine=a;
      tr->coordonate.longitudine=b;
      while(getchar()!='\n');
      printf("Introduceti indiciul\n");
      if(fgets(tr->clue,200,stdin)==NULL)
	{
	  perror("Citire string:");
	  return 0;
	}
      tr->clue[strcspn(tr->clue,"\n")]='\0';
      printf("Introduceti value \n");
      scanf("%d", &tr->value);
      return 1;
}

int add(treasure *tr, const char *hunt)
{printf("--------------------------------------------------\n");
  char sir[MAXSTR];
    snprintf(sir,MAXSTR,"%s/treasure.dat", hunt);
    DIR *d;
   
    if((d=opendir(hunt))==NULL)
      {
	if(errno!=ENOENT)
	  { 
	    perror("Eroare la deschiderea directorului ");
	    return 0;
	  }
	else
	  {
	    if(mkdir(hunt,0777)==-1)
	      {
		perror("Eroare la crearea directorului ");
		return 0;
	      }
	  }
      }
    else
      {
	closedir(d);
      }
    printf("Director deschis/creat\n");
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
      printf("Treasure id : %s \n Numele : %s \n Clue : %s\n Value : %d \n Coordonatele : %f,%f \n\n", t.id, t.name,t.clue,t.value,t.coordonate.longitudine, t.coordonate.latitudine);
      printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }
  int nr=nrtreasure(fd);
  printf("Numarul treasurilor este %d \n\n", nr);
  printf("--------------------------------------------------\n");
  close(fd);
}
void view(const char *hunt, const char *trea)
{
  printf("--------------------------------------------------\n");
char sir[MAXSTR],sir1[MAXSTR];
 struct stat s1;
    snprintf(sir1,MAXSTR,"%s",hunt);
    if(stat(sir1,&s1)!=0)
      {
	perror("Eroare la director ");
	return;
      }
   snprintf(sir,MAXSTR,"%s/treasure.dat", hunt);
    struct stat st;
    if(stat(sir,&st)<0)
      {
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
  int ok=0,numarare=0;
  while(read(fd,&t,sizeof(treasure))==sizeof(treasure))
    {numarare++;
      if(strcmp(t.name,trea)==0)
	{
	  printf("Treasure id : %s \n Numele : %s \n Clue : %s\n Value : %d \n Coordonatele : %f,%f \n\n", t.id, t.name,t.clue,t.value,t.coordonate.longitudine, t.coordonate.latitudine);
	  ok=1;
	  break;
	}
    }
  if(ok==1)
    {
      printf("Am gasit acest treasure hunt, cu numarul %d\n",numarare);
    }
  else
    {
      printf("Nu am gasit acest treasure hunt \n");
    }
  printf("--------------------------------------------------\n");
   close(fd);
}
void remove_hunt(const char *hunt)
{
  printf("--------------------------------------------------\n");
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
      perror("Eroare la fisier treasure");
      return;
    }
  if(remove(s1)!=0)
    {
      perror("Eroare la stergerea continutului fisierului treasure \n");
      return;
    }
  snprintf(s1,MAXSTR,"%s/logged_hunt.txt", hunt);
  if(stat(s1,&str)!=0)
    {
      perror("Eroare la fisierul logged_hunt.txt ");
      return;
    }
  if(remove(s1)!=0)
    {
      perror("Eroare la stergerea fisierului logged_hunt.txt ");
      return;
    }
  printf("Continutul acestui hunt a fost sters complet!\n\n");
  if(rmdir(s)==-1)
    {
      perror("Eroare la stergere director");
      return ;
    }
  char sym[MAXSTR];
  snprintf(sym,MAXSTR,"logged_hunt-%s",hunt);
  struct stat st;
  if(lstat(sym,&st)==-1)
    {
      perror("Eroare, legatura symbolica nu exista ");
      return;
    }
  if(unlink(sym)==-1)
    {
      perror("Eroare la stergerea legaturii simbolice");
      return;
    }
  printf("Hunt ul a fost sters cu succes alaturi de legaturile sale simbolice!\n");
  printf("--------------------------------------------------\n");
  
}
void remove_treasure(const char *hunt, const char *id)
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

  treasure tr;
  int ok=1,n=0;
  int nr=nrtreasure(fd);
    lseek(fd,0,SEEK_SET);
   treasure *all=(treasure*)malloc(sizeof(treasure)*nr);
  while(read(fd,&tr,sizeof(treasure))==sizeof(treasure))
    {
      if(strcmp(tr.id,id)==0)
	{
	  ok=0;
	}
      else
	{
	  all[n++]=tr;
	}
    }
  if(ok==1)
    printf("Nu am reusit sa gasim treasure-ul cautat\n");
  else
    {
      lseek(fd,0,SEEK_SET);
      write(fd,all,n*sizeof(treasure));
      ftruncate(fd,n*sizeof(treasure));
      printf("Am sters cu succes treasure-ul cu id ul %s \n",id);
    }
  free(all);
  close(fd);
  printf("--------------------------------------------------\n");
}

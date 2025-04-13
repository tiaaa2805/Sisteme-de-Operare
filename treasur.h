#ifndef TREASUR_H
#define TREASUR_H

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
int nrtreasure(int fd);
int createdir(const char *hunt);
void adaugarea(treasure *tr);
int add(treasure *tr, const char *hunt);
void list(const char *hunt);
void view(const char *hunt, const char *trea);
void remove_hunt(const char *hunt);
void remove_treasure(const char *hunt, int id);

#endif

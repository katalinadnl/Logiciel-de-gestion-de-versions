#ifndef LISTES_H
#define LISTES_H


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct cell {
    char* data ;
    struct cell* next ;
} Cell;

typedef Cell* List ;

//exo1
int hashFile(char *source, char *dest);
char* sha256file(char* file);

//exo2
List* initList();
Cell* buildCell(char* ch);
void insertFirst(List *L, Cell *C);
char* ctos(Cell *c);
char* ltos(List *L);
Cell* listGet(List *L, int i);
Cell* searchList(List *L, char *str);
List* stol(char *s);
void ltof(List *L, char *path);
List* ftol(char* path);
void freeCell(Cell *c);
void freeListe(List *L);

//exo3
List* listdir(char *root_dir);
int file_exists(char *file);
void cp(char *to, char *from);
char* hashToPath(char *hash);
void blobFile(char* file);

#endif
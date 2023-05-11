#ifndef WORKS_H
#define WORKS_H


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Listes.h"

//s2
typedef struct { 
    char* name; 
    char* hash;
    int mode; 
} WorkFile;

typedef struct { 
    WorkFile* tab;
    int size;
    int n; 
} WorkTree;

//exo4
int getChmod(const char *path);
void setMode(int mode, char *path);
WorkFile* createWorkFile(char *name);
char* wfts(WorkFile *wf);
WorkFile* stwf(char *ch);
WorkTree* initWorkTree();
int inWorkTree(WorkTree *wt, char *name);
int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode);
char* wtts(WorkTree* wt);
WorkTree *stwt(char *ch);
int wttf(WorkTree* wt, char* file);
WorkTree* ftwt(char* file);
void freeWorkFile(WorkFile *wf);
void freeWorkTree(WorkTree *wt);
int isDir(const char *fileName);

//exo5
char* blobWorkTree(WorkTree* wt);
char *concat_paths(char *path1, char *path2);
char* saveWorkTree(WorkTree* wt, char* path);
int isWorkTree(char *hash);
void restoreWorkTree(WorkTree* wt, char* path);

#endif
#ifndef COMMIT_H
#define COMMIT_H


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Listes.h"
#include "Works.h"

//s3
typedef struct key_value_pair {
  char *key;
  char *value;
} kvp;

typedef struct hash_table {
  kvp **T;
  int n;
  int size;
} HashTable;

typedef HashTable Commit;

//exo6
kvp* createKeyVal(char* key, char* val);
void freeKeyVal(kvp* kv);
char* kvts(kvp*k);
kvp *stkv(char *str);
Commit* initCommit();
void freeCommit(Commit *c);
unsigned long sdbm(char *str);
void commitSet(Commit* c, char* key, char* value);
Commit* createCommit(char* hash);
char* commitGet(Commit* c, char* key);
char* cts(Commit* c);
Commit* stc(char* ch);
void ctf(Commit* c, char* file);
Commit* ftc(char* file);
char* blobCommit(Commit* c);

//exo7
void initRefs();
void createUpdateRef(char *ref_name, char *hash);
void deleteRef(char* ref_name);
char* getRef(char* ref_name);
void myGitAdd(char* file_or_folder);
void myGitCommit(char* branch_name, char* message);


#endif
#ifndef BRANCH_H
#define BRANCH_H


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Listes.h"
#include "Works.h"
#include "Commit.h"


//exo8
void initBranch();
int branchExists(char* branch);
void createBranch(char* branch);
char* getCurrentBranch();
char* hashToPathCommit(char*hash);
void printBranch(char* branch);
List* branchList(char* branch);
List *getAllCommits();

//exo9
void restoreCommit(char* hash_commit);
void myGitCheckoutBranch(char* branch);
List* filterList(List* L, char* pattern);
void myGitCheckoutCommit(char* pattern);
#endif
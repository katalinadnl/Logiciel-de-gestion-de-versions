#ifndef MERGE_H
#define MERGE_H

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
#include "Branch.h"


//exo11
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts);
WorkTree* creationNewWT(char *branch);
List *merge(char *remote_branch, char *message);
void createDeletionCommit(char *branch, List *conflicts, char *message);


#endif
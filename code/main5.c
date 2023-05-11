#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "header/Commit.h"
#include "header/Works.h"
#include "header/Listes.h"
#include "header/Branch.h"
#include "header/Merge.h"


int main(int argc, char ** argv){

    WorkTree * wt1 = initWorkTree();
    appendWorkTree(wt1, "D.c", "897", 0);
    appendWorkTree(wt1, "B.c", "hdusjs", 0);
    appendWorkTree(wt1, "C.c", "123", 0);
  

    WorkTree * wt2 = initWorkTree();
    appendWorkTree(wt2, "B.c", "ghr3ee", 0);
    appendWorkTree(wt2, "C.c", "123", 0);
    appendWorkTree(wt2, "E.c", "126", 0);

    List* conflicts;

    WorkTree* nonConfl = mergeWorkTrees(wt1, wt2, &conflicts);

    char * chTree = wtts(nonConfl);
    printf(" Fusion\n%s\n", chTree);
    free(chTree);

    char * chList = ltos(conflicts);
    printf(" Conflicts\n%s\n", chList);
    free(chList);

    freeListe(conflicts);
    freeWorkTree(nonConfl);
    freeWorkTree(wt1);
    freeWorkTree(wt2);
    

    return 0;

}
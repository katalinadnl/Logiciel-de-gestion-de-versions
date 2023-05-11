#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "header/Commit.h"
#include "header/Works.h"
#include "header/Listes.h"
#include "header/Branch.h"
#include "header/Merge.h"

//Q11.1 va sepparer les wt dans une liste avec conflicts et un new wt avec nonconflict
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts){
    List* tmp = initList(); // temporaire pour conflicts
    int len1 = wt1->n;
    int len2 = wt2->n;

    WorkTree *nonConflict = initWorkTree();

    //pour wt1

    for(int i=0; i<len1; i++){
        int indice= inWorkTree(wt2, wt1->tab[i].name); //verif la presence dans wt2

        if(indice<0){
            if(inWorkTree(nonConflict, wt1->tab[i].name)==-1){ //s'il n'est pas present 
                appendWorkTree(nonConflict, wt1->tab[i].name, wt1->tab[i].hash, wt1->tab[i].mode); //ajout dans nonConflict
            }
        }else if(strcmp(wt1->tab[i].hash, wt2->tab[indice].hash) == 0){
            if(inWorkTree(nonConflict, wt1->tab[i].name)==-1){
                appendWorkTree(nonConflict, wt1->tab[i].name, wt1->tab[i].hash, wt1->tab[i].mode);
            }
        }else{
            if(searchList(tmp, wt1->tab[i].name)==NULL){ //si dans les conflicts il n'ya pas  
                insertFirst(tmp, buildCell(wt1->tab[i].name)); //on ajout dans les conflicts
            }
        }
    }

    //pour wt2

    for(int i=0; i<len2; i++){
        int indice= inWorkTree(wt1, wt2->tab[i].name); //verif la presence dans wt1

        if(indice<0){
            if(inWorkTree(nonConflict, wt2->tab[i].name)==-1){ //s'il n'est pas present, ajout dans nonConflict
                appendWorkTree(nonConflict, wt2->tab[i].name, wt2->tab[i].hash, wt2->tab[i].mode); 
            }
        }else if(strcmp(wt2->tab[i].hash, wt2->tab[indice].hash) == 0){
            if(inWorkTree(nonConflict, wt2->tab[i].name)==-1){
                appendWorkTree(nonConflict, wt2->tab[i].name, wt2->tab[i].hash, wt2->tab[i].mode);
            }
        }else{
            if(searchList(tmp, wt2->tab[i].name)==NULL){
                insertFirst(tmp, buildCell(wt2->tab[i].name));
            }
        }
    }

    *conflicts= tmp;
    return nonConflict;
}

//Q11.2

//fonction intermediaire pour retoruver le wt associe a son hash depuis le hash d'un commit

WorkTree* creationNewWT(char *branch){
    char *hashCommit = getRef(branch);
    char *hash_br = hashToPath(hashCommit);
    char *hashPathCommit=(char*)malloc(10000*sizeof(char));

    sprintf(hashPathCommit, "%s.c", hash_br);

    Commit* commit = ftc(hashPathCommit);
    char* hash_tree =(char*)malloc(1000*sizeof(char));

    strcpy(hash_tree, hashToPath(commitGet(commit,"tree")));
    
    strcat(hash_tree , ".t");
    WorkTree * wt = ftwt(hash_tree);

    free(hash_tree); 
    free(hashPathCommit); 
    freeCommit(commit); 
    free(hash_br); 
    return wt;
}

List *merge(char *remote_branch, char *message) {
    char* curr_branch = getCurrentBranch();
    
    WorkTree * wt_current = creationNewWT(curr_branch); //current branch
    
    WorkTree * wt_remote = creationNewWT(remote_branch); //branch remote
    
    List *conflicts = initList();
    
    WorkTree *wt = mergeWorkTrees(wt_current, wt_remote, &conflicts);

    char *curr_ref = getRef(curr_branch);
    char *rb_ref = getRef(remote_branch);
    
    if (*conflicts == NULL) {

        char* hashNewWT = blobWorkTree(wt);

        Commit* c = createCommit(hashNewWT);
        commitSet(c, "predecessor", curr_ref);
        commitSet(c, "merged_predecessor", rb_ref);
        commitSet(c, "message", message);

        char* hashC = blobCommit(c);
        FILE *b = fopen(".refs/HEAD", "w");
        fprintf(b, "%s\n", hashC);
        fclose(b);
        char path[256];
        sprintf(path, ".refs/%s", curr_branch);
        FILE *br = fopen(path, "w");
        fprintf(br, "%s\n", hashC);
        fclose(br);
        restoreWorkTree(wt, ".");
        deleteRef(remote_branch);
        free(hashNewWT);
    }
    return conflicts;
}


//Q11.3 creation du commit a partir des conflicts qui reste
void createDeletionCommit(char *branch, List *conflicts, char *message) {

  char *branche_curr= getCurrentBranch();
  myGitCheckoutBranch(branch);
  WorkTree *wt= creationNewWT(branch);
  if (file_exists(".add")) {
    deleteRef(".add");}

  if(wt){
    for(int i = 0;i < wt->n; i++){
      Cell *cell= searchList(conflicts, wt->tab[i].name);
      if(!cell){
        myGitAdd(wt->tab[i].name);
      }
      
    }
    
    myGitCommit(branch, message);
    myGitCheckoutBranch(branche_curr);
    freeWorkTree(wt);
  }
  free(branche_curr);
}







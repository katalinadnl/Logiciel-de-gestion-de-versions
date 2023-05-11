#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include "header/Commit.h"
#include "header/Works.h"
#include "header/Listes.h"
#include "header/Branch.h"

//Q8.1
void initBranch(){ //creer le fichier cache contenat "master"
    if(!file_exists(".current_branch")){
        system("touch .current_branch");
    }
    system("echo master > .current_branch");
}

//Q8.2
int branchExists(char* branch){//retourne 1 si la branche existe, 0 sinon
    if(branch == NULL || strlen(branch)==0) return 0;
    List *l = listdir(".refs");
    int index;
    if(searchList(l, branch) == NULL){
        index=0;
    }else{ 
        index=1;
    }
    freeListe(l);
    return index;
}

//Q8.3
void createBranch(char* branch){//cree une ref branch qui pointe vers le meme commit que la ref HEAD
    if (branchExists(branch) == 1) {
        return;
    }
    char pathHEAD[20] = ".refs/HEAD";

    char command[256];
    sprintf(command, "cat %s > .refs/%s", pathHEAD, branch);
    system(command);
}

//Q8.4
char* getCurrentBranch(){ //lit le fichier current_branch et retourne le nom de la branche courante 
    FILE* f = fopen(".current_branch","r");
    char* branch = malloc(sizeof(char)*1000);
    fscanf(f, "%s",branch);
    return branch;
}

//Q8.5
char* hashToPathCommit(char*hash){ 
    char* chemin = malloc(sizeof(char)*1000);
    char* tmp = hashToPath(hash);
    sprintf(chemin, "%s.c", tmp);
    free(tmp);
    return chemin;
}

void printBranch(char* branch){ 
    char *commit_hash = getRef(branch);

    if (commit_hash == NULL || strlen(commit_hash) == 0) {
        return;
    }

    char *commit_path = malloc(3000 * sizeof(char));
    char *chemin = malloc((3003) * sizeof(char));
    strcpy(chemin, hashToPath(commit_hash));

    sprintf(commit_path, "%s.c", chemin);
    Commit *c = ftc(commit_path);

    while (c != NULL) {

        if(commitGet(c, "message") != NULL){
            printf("%s : %s\n", commit_hash, commitGet(c, "message"));
        }else{ //sans message
            printf("%s\n", commit_hash);
        }

        if (commitGet(c, "predecessor") != NULL) {
            commit_hash = commitGet(c, "predecessor");
            strcpy(chemin, hashToPath(commit_hash));

            sprintf(commit_path, "%s.c", chemin);
            c = ftc(commit_path);
        }else{

        c = NULL;
        }
    }

    free(commit_path);
    free(chemin);
    free(commit_hash);
}

//Q8.6
List* branchList(char* branch){
    List* L =initList();
    char* commit_hash = getRef(branch);
    if(!commit_hash){
        freeListe(L);
        free(commit_hash);
        return NULL;
    }else if (strlen(commit_hash)==0){
        return L;
    }
    char* commit_path =hashToPathCommit(commit_hash);
    Commit* c = ftc(commit_path);
    
    while(c){
        insertFirst(L, buildCell(commit_hash));
    
        if(commitGet(c, "predecessor") == NULL){
            break;
        }else{
        char* ref = commit_hash;
        commit_hash= commitGet(c, "predecessor");
        free(ref);
        
        char *path = commit_path;
        commit_path= hashToPathCommit(commit_hash);
        free(path);
        
        c =ftc(commit_path);
        }
    }
    free(commit_hash);
    free(commit_path);
    freeCommit(c);
    return L;
}

//Q8.7
List *getAllCommits(){
    List* L = initList();
    List* list = listdir(".refs");
    Cell* content = *list;
    while(content){
        if(content->data[0]!='.'){
            List* list2 = branchList(content->data);
            Cell *cell = *list2;
            while (cell){
                if (!searchList(L, cell->data)) {
                Cell* cellule = buildCell(cell->data);
                insertFirst(L, cellule);
            }
            cell = cell->next;
        }
        freeListe(list2);
        }
        content = content->next;
    }
    freeListe(list);
    return L;
}

//Q9.1
void restoreCommit(char* hash_commit){
    char *hash_path = malloc(1000 * sizeof(char));
    char *chemin = malloc((1100) * sizeof(char));
    strcpy(chemin, hashToPath(hash_commit));
    sprintf(hash_path, "%s.c", chemin);

    Commit *c = ftc(hash_path);
    char *tree_hashPath = malloc((1100) * sizeof(char));

    char *tree_hash = commitGet(c, "tree");
    if(tree_hash[strlen(chemin)-1]=='\n'){
        tree_hash[strlen(chemin)-1]='\0';
    }

    strcpy(tree_hashPath, hashToPath(tree_hash));
    strcat(tree_hashPath, ".t");

    WorkTree *wt = ftwt(tree_hashPath);
    restoreWorkTree(wt, ".");
}

//Q9.2
void myGitCheckoutBranch(char* branch){
    FILE * f = fopen(".current_branch", "w");
    if(!f) printf("erreur ouverture\n");        
    fprintf(f, "%s", branch);
    fclose(f);
    char* hash_commit =  getRef(branch);
    FILE *f2 = fopen(".refs/HEAD", "w");
    if(hash_commit!=NULL){
        fprintf(f2, "%s\n", hash_commit);
    }
    fclose(f2);
    char *hash_commit2= getRef(branch);
    if(hash_commit2!=NULL && strlen(hash_commit2)!=0){ 
        restoreCommit(hash_commit2);
    }
}

//Q9.3
List* filterList(List* L, char* pattern){ 
    List* filtered = initList();
    int len_pattern = strlen(pattern);
    Cell* c = *L;
    while(c){
        if (strncmp(pattern, c->data, len_pattern) == 0){
            insertFirst(filtered, buildCell(c->data));
        }
        c = c->next;
    }
    return filtered;
}

//Q9.4
void myGitCheckoutCommit(char* pattern){
    List* L = getAllCommits();
    List* L_filtered = filterList(L, pattern);

    Cell* c = *L_filtered;

    if(!c){
        printf("Il n'y a pas de hash\n");
        freeListe(L_filtered);
        freeListe(L);
        return;
    }

    if(!c->next){
        char * commit_hash = c->data;
        createUpdateRef ("HEAD", commit_hash);
        restoreCommit (commit_hash);
        freeListe(L_filtered);
        freeListe(L);
        return;
    }

    printf("Quelle requete voulez-vous?\n");
    char* str = ltos(L_filtered);
    printf("%s\n", str);
    freeListe(L_filtered);
    freeListe(L);
    free(str);
    return;
}




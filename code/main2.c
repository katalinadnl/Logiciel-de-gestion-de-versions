#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include "header/Listes.h"
#include "header/Works.h"

int main(int argc, char ** argv){
    //EXO4

        //1
        WorkFile *wf1 = createWorkFile("wf1");
        wf1->mode=777;
        wf1->hash=strdup("one");

        WorkFile *wf2 = createWorkFile("wf2");
        wf2->mode=777;
        wf2->hash=strdup("two");

        WorkFile *wf3 = createWorkFile("wf3");
        wf3->mode=777;
        wf3->hash=strdup("three");
        
        //2
        char* chaine1= wfts(wf1); //wf->chaine
        printf("Workfile1: %s\n", chaine1);
        char* chaine2= wfts(wf2);
        printf("Workfile2: %s\n", chaine2);
        char* chaine3= wfts(wf3);
        printf("Workfile3: %s\n", chaine3);

        //3
        WorkFile* wf4= stwf(chaine2); //chaine->wf
        printf("Workfile4: %s\n", wfts(wf4));

        //4
        WorkTree* wt= initWorkTree();

        //5,6
        appendWorkTree(wt, "wf1", "one", 777); //ajout au wt
        appendWorkTree(wt, "wf4", "four", 777);
        int res= inWorkTree(wt, "wf4"); //presence
        printf("res=%d\n", res);

        //7
        char *st = wtts(wt); //wt->chaine
        printf("%s\n", st);
        
        //8
        WorkTree* wt1= stwt(st); //chaine->wt
        printf("WorkTree: %s\n", wtts(wt1));

        //9
        wttf(wt, "fichierTestWork.txt"); //wt->file

        //10
        WorkTree* wt2= ftwt("fichierTestWork.txt"); //file->wt
        printf("%s\n", wtts(wt2));

    //EXO5
        printf("%s\n",blobWorkTree(wt)); //hash du fich temp

    freeWorkFile(wf1);
    freeWorkFile(wf2);
    freeWorkFile(wf3);
    freeWorkTree(wt);
    free(st);
    freeWorkTree(wt1);
    freeWorkTree(wt2);
    
    return 0;
}
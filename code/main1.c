#include <math.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "header/Listes.h"

int main(int argc, char ** argv){

    //EXO2
        //1
        List* L = initList(); 

        //2
        Cell* c1 = buildCell("chaine1"); 
        Cell* c2 = buildCell("chaine2");
        Cell* c3 = buildCell("chaine3");
        Cell* c4 = buildCell("chaine4");

        //3
        insertFirst(L, c1); 
        insertFirst(L, c2);
        insertFirst(L, c3);
        insertFirst(L, c4);

        //4,5
        printf("%s\n", ltos(L)); //ltos = chaine format cell|cell|..
        Cell *get = listGet(L,3); //=ieme elem de L
        printf("%s\n", ctos(get)); //cell->chaine

        //6
        Cell* c5 = searchList(L, "chaine2"); 
        printf("%s\n", ctos(c5));

        //7
        List * L2 = stol("ch1|ch2|ch3|ch4|ch5"); //chaine->liste
        printf("%s\n",ltos(L2)); 

        //8
        char *chaine = "list1|list2|list3|list4";
        ltof(stol(chaine), "write.txt");
    
    //EXO3
        int statut = file_exists("main1.c");
        if(statut==1){
            printf("Le fichier main1.c existe dans le repertoire courant ! \n");
        }else{
            printf("Le fichier main1.c n'existe pas dans le repertoire courant ! \n");
        }
        cp("fichier2.txt", "fichier1.txt"); //copie contenu
        blobFile("fichier2.txt"); //enregistre instantané

    freeCell(get);
    freeListe(L2);

    return 0;

}
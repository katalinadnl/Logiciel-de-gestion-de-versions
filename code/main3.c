#include <math.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include "header/Listes.h"
#include "header/Works.h"
#include "header/Commit.h"


int main(int argc, char ** argv){

    //EXO6
        //1
        kvp* element= createKeyVal("j'adore", "ce projet");
        kvp* element2= createKeyVal("j'aime", "ce projet");
        freeKeyVal(element2);

        //2
        char* chaine= kvts(element); //kvp->chaine
        kvp* copie= stkv(chaine);//chaine_>kvp
        printf("Element: %s\n", chaine);
        printf("Copie: key=%s, val=%s\n", copie->key, copie->value);

        freeKeyVal(element);
        free(chaine);
        freeKeyVal(copie);

        //3,5
        Commit* com= initCommit();
        commitSet(com, "Des", "pates");

        //6
        Commit* com2= createCommit("abcd");

        //7
        char* get= commitGet(com, "Des");
        char* get2= commitGet(com, "abb");
        printf("resultat commitGet1: %s\n", get);
        printf("resultat commitGet2: %s\n", get2);

        //8
        char* chCom= cts(com); //commit->chaine
        printf("%s\n\n", chCom);

        char* chCom2= cts(com2);
       
        freeCommit(com);
        free(chCom2);

        Commit* copie2=stc(chCom); //chaine->copie
        char* chaineCopie= cts(copie2);
        
        free(chCom);

        //9
        ctf(copie2, "WriteCommit.txt");

        char* hash= blobCommit(copie2);
        printf("Hash Commit: %s\n", hash);

        free(hash);
        freeCommit(copie2);

        freeCommit(com2);
        free(get2);
        free(chaineCopie);
    return 0;

}
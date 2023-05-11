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
#include "header/Merge.h"

#define TAILLE 10
#define SIZE_COMMIT 10

int main(int argc, char **argv){

    if(strcmp(argv[1], "init") == 0){ //initialise le rep de references
        initRefs();   //Q7.1
        initBranch(); //Q8.1
    }

    if(strcmp(argv[1], "list-refs") == 0){ //affiche tous les ref existantes
        if(file_exists(".refs")){
            Cell *tmp = *listdir(".refs");
            while(tmp){
                if(tmp->data[0] != '.'){
                    char *content = getRef(tmp->data); //Q7.4
                    printf("%s :\t%s\n", tmp->data, content);
                    free(content);
                }
                tmp = tmp->next;
            }
        }
    }

    if(strcmp(argv[1], "create-ref") == 0){ //creer la ref qui pointe vers le commit correspondant au hash donne
        if(argc == 4){
            createUpdateRef(argv[2], argv[3]); //Q7.2
            createUpdateRef("HEAD", argv[3]);
        }else if(argc == 3){ //sil donne pas une nouvelle ref
            createUpdateRef(argv[2], NULL); //Q7.2
            createUpdateRef("HEAD", NULL);
        }
    }

    if(strcmp(argv[1], "delete-ref") == 0){ //supprime la ref
        deleteRef(argv[2]); //Q7.3
    }

    if(strcmp(argv[1], "add") == 0){ //ajout dans la zone de prep
        for(int i = 2; i < argc; i++){
            myGitAdd(argv[i]); //Q7.5
        }
    }

    if(strcmp(argv[1], "list-add") == 0){ //affiche le contenu de la zone prep
        if(file_exists(".add")){
            WorkTree *wt = ftwt(".add"); //transf le fichier en wt
            char *s = wtts(wt); //transf le wt dasn un chaine de wf
            printf("%s\n", s); //affiche la zone de prep
            free(s);
            freeWorkTree(wt);
        }else{
            printf("La liste .add n'existe pas");
        }
    }

    if(strcmp(argv[1], "clear-add") == 0){
        system("rm .add"); //supprime la zone de prep
    }

    if(strcmp(argv[1], "commit") == 0){ //effectue un commit sur une branche 
        if((argc == 5) && (argv[3][0] == '-') && (argv[3][1] == 'm')){ //sil ya mess
            myGitCommit(argv[2], argv[4]); //Q7.6
        }else{
            myGitCommit(argv[2], NULL);
        }
    }

    //EXO10

    if(strcmp(argv[1], "get-current-branch") == 0){ //affiche le nom de la branche courante
        char *current_branch = getCurrentBranch(); //Q8.4
        printf("%s\n", current_branch);
        free(current_branch);
    }

    if(strcmp(argv[1], "branch") == 0){ //cree une branche avec le nom mis e arg
        if(branchExists(argv[2])){
            printf("La branche %s existe deja\n", argv[2]); //Q8.2
        }else{
            createBranch(argv[2]); //Q8.3
        }
    }

    if(strcmp(argv[1], "branch-print") == 0){ //affiche le hash de tous le commits de la branche
        if(!branchExists(argv[2])){
            printf("La branche %s n'existe pas\n", argv[2]);
        }else{
            printBranch(argv[2]); //Q8.5
        }
    }

    if(strcmp(argv[1], "checkout-branch") == 0){ //deplacement sur la branche
        if(!branchExists(argv[2])){
            printf("La branche %s n'existe pas\n", argv[2]);
        }else{
            myGitCheckoutBranch(argv[2]); //Q9.2
        }
    }

    if(strcmp(argv[1], "checkout-commit") == 0){
        if(argc != 3){
            printf("Erreur: il faut 1 argument apres la commande\n");
        }else{
            myGitCheckoutCommit(argv[2]); //Q9.4
        }
    }

    if(strcmp(argv[1], "merge") == 0){
        
        char *branch = argv[2];
        char *message = argv[3];
        
        char *curr_branch = getCurrentBranch();
        List *conflicts = merge(branch, curr_branch);
        if(*conflicts == NULL){
            printf("La fusion s'est bien passée!\n");
            free(curr_branch);
        }else{
            printf("Il y'a des collissions! On vous propose de choisir une option:\n\n");

            printf("1. Garder les fichiers de la branche courante.\n");
            printf("2. Garder les fichiers de la branche %s\n", argv[2]);
            printf("3. Choix manuel, conflit par conflit, la branche sur laquelle vous souhaitez garder le fichier/repertoire qui est en conflit.\n");
            printf("\nFaites votre choix entre 1 et 3 : \n");

            int choix;
            scanf("%d", &choix);
            if(choix == 1){ //si choix=1 on cree un commit de supp pour la branche <branch>, avant de faire appel a la fonction merge.
                createDeletionCommit(branch, conflicts, message);
                merge(branch, message);
            }
            if(choix == 2){ // si choix= 2 on cree un commit de supp pour la branche courante, avant de faire appel a la fonction merge.
                createDeletionCommit(curr_branch, conflicts, message);
                merge(branch, message);
            }
            if(choix == 3){
                Cell *copie= *conflicts;
                List *list_currente = initList();
                List *list_branch = initList();
                Cell* tmp; 
                //division en deux listes
                while(copie){
                    tmp = buildCell(copie->data); //faire une cellule pour un conflict
                    printf("Quelle version du fichier %s souhaitez vous garder?\n",copie->data);
                    printf("Pour garder la version de la branche courante, tapez 1\n");
                    printf("Pour garder la version de la branche %s, tapez 2", branch);
                    int choix_version;
                    scanf("%d", &choix_version);
                    if(choix_version == 1){ //si il garde la branche courente
                        insertFirst(list_branch, tmp);
                    }
                    if(choix_version == 2){ //s'il garde la branche <branch>
                        insertFirst(list_currente, tmp);
                    }
                    copie= copie->next;
                }
                createDeletionCommit(curr_branch, list_currente, message);
                createDeletionCommit(branch, list_branch, message);
                merge(branch, message);
        
                free(curr_branch);
                freeListe(list_currente);
                freeListe(list_branch);
                freeListe(conflicts);
            }
        }
    }
    return 0;
}
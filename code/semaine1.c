#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "header/Listes.h"


//question 1.1 et 1.2 on a fait dans le terminal

//Q1.3
int hashFile(char *source, char *dest) { //hash le contenu du source et l'ecrit dans dest
  char buff[1000];
  sprintf(buff, "cat %s | sha256sum > %s", source, dest);
  system(buff);
  return 0;
}

//Q1.4
char* sha256file(char* file){ //renvoie le char contenant le hash du fichier file
    static char template[] = "/tmp/myfileXXXXXX";
    char fname[1000];
    strcpy(fname, template);
    int fd = mkstemp(fname);
    hashFile(file, fname);
    char *buff = (char *)malloc(1000 * sizeof(char));
    FILE *f = fopen(fname, "r");
    fgets(buff, sizeof(char) * 10000, f);
    close(fd);
    fclose(f);
    buff[strlen(buff) - 4] = '\0';
    return buff;
}

//Q2.1
List* initList(){ //init une liste vide
    List* new= (List*)malloc(sizeof(List));
    *new=NULL;
    return new;
}

//Q2.2
Cell* buildCell(char *ch){ //alloue et retourne une cell de la liste
    Cell* cell=(Cell*)malloc(sizeof(Cell));
    cell->data=strdup(ch);
    cell->next=NULL;
    return cell;
}

//Q2.3
void insertFirst(List *L, Cell *C){ //ajout en tete
    if(*L==NULL){
        *L = C;
    }else{
        C->next = *L;
        *L = C;
    }
}

//Q2.4
char* ctos(Cell *c){ //return la data d'un cell
    return c->data;
}

/*la fonction ctos va returner la chaine vide si la liste est vide, 
sinon, la fonction ltos va transformer la liste en une chaine avec le format chaine1|chaine2|... */

char* ltos(List *L){ //retorune un chaine avec les datas d'une liste 
    Cell *tmp = *L;
    char *res = (char *)malloc(sizeof(char) * 1000);
    if(tmp==NULL){ //si liste vide
        strcat(res, "\0");
        return res;
    }
    while(tmp){
        strcat(res, ctos(tmp)); //ajout a la suite de res
        strcat(res, "|");
        tmp = tmp->next;
    }
    res[strlen(res) - 1] = '\0'; //fin
    return res;
}

//Q2.5
Cell* listGet(List *L, int i){ //renvoie le i-ème elem de la liste
    Cell * tmp = *L;
    if(tmp==NULL){ //si liste vide
        return NULL;
    }
    int j=0; //pour calculer les elements de la liste 
    while((tmp) && (j!=i)){
        j++;
        tmp=tmp->next;
    }
    return tmp;
}

//Q2.6
Cell* searchList(List *L, char *str){ //recherche un elem dans la liste et renvoie la ref vers lui
    Cell * tmp = *L;
    while(tmp){
        if(strcmp(tmp->data,str)==0){
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

//Q2.7
/*a partir du format chaine1|chaine2|... on va separere avec l'aide 
de la fonction strtok tous les elements et on va transformer la chaine dans une liste*/

List* stol(char *s){ //tranforme la chaine dans une liste
    List *newL = initList();
    char *tmp = strdup(s);
    char *elem = strtok(tmp, "|"); //premier element jusqu'au separateur "|"
    while(elem) {
        insertFirst(newL, buildCell(elem));
        elem = strtok(NULL, "|");  //On demande l'element suivant.
    }
    free(tmp);
    return newL;
}

//Q2.8
void ltof(List *L, char *path){ //ecrit la liste dans un fichier
    FILE *f = fopen(path, "w");
    char *ch = ltos(L); //transformer la liste en chaine
    fprintf(f, "%s\n",ch);
    fclose(f);
}

List* ftol(char* path){ //lit une liste dans un fihcier
    FILE *f = fopen(path, "r");
    char buff[256];
    fgets(buff,256, f); //lire
    return stol(buff);
}

//fonctions de liberer liste et cell

void freeCell(Cell *c){
    if(c!=NULL){
        free(c->data);
        free(c);
    }
}

void freeListe(List *L){
    if(L){
        Cell *l = *L;
        while(l){
            Cell *tmp = l->next;
            freeCell(l);
            l = tmp;
        }
    free(L);
    }
}

//Q3.1
List* listdir(char *root_dir){ //renvoie la liste de tous les fich et rep d'un chemin
    List *L = initList();
    DIR *dp = opendir(root_dir);

    struct dirent *ep;
    if(dp != NULL) {

        while ((ep = readdir(dp)) != NULL) {
        Cell *c = buildCell(ep->d_name);
        insertFirst(L, c);
        }
        closedir(dp);
    }

    return L;
}

//Q3.2
int file_exists(char *file){ //return 1 si le fichier existe, 0 sinon
    List *L = listdir(".");
    int res;

    if(searchList(L, file) == NULL) {
        res = 0;
    } else {
        res = 1;
    }
    freeListe(L);
    return res;
}

//Q3.3
void cp(char *to, char *from){ //copie le contenu d'un fichier vers un autre
    FILE *fr = fopen(from, "r");
    if(fr==NULL){
        printf("Erreur d'ouverture du fichier %s\n", from);
        return ;
    }
    FILE *too = fopen(to, "w");
    char buffer[256];
    while(fgets(buffer,256,fr)){ //lire ligne par ligne et stocker dans buff
        fprintf(too,"%s",buffer); //printf dans le fichier to
    }
    fclose(fr);
    fclose(too);
}

//Q3.4
char* hashToPath(char *hash){ //return le chemin d'un fichier a partir de son hash
    char *dir = (char*)malloc(sizeof(char)*strlen(hash)+1);
    sprintf(dir, "%c%c/%s", hash[0],hash[1],hash+2);
    return dir;
}

//Q3.5
void blobFile(char* file){ //enregistre un instantané du fichier
    char *hash = sha256file(file); //on fait le hash du file
    char *chemin = hashToPath(hash); //le chemin de ce fich

    char rep[3];
    rep[0] = chemin[0];
    rep[1] = chemin[1];
    rep[2] = '\0';

    if(!file_exists(rep)){
        char *mk = malloc(15 * sizeof(char));
        sprintf(mk, "mkdir %c%c", chemin[0], chemin[1]);
        system(mk); //executer la commande mk
        free(mk);
    }

    cp(chemin, file); //on enregistre le nouv chemin
    free(hash);
    free(chemin);
}




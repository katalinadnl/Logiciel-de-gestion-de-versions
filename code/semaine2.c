#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "header/Listes.h"
#include "header/Works.h"
#include <sys/stat.h>

#define TAILLE_MAX 100

int getChmod(const char *path){  //recup les autorisations assoc a un un fihicer (modif, lire, exec..)
    struct stat ret;

    if(stat(path, &ret) == -1){ 
        return -1;
    }

    return  (ret.st_mode & S_IRUSR)|(ret.st_mode & S_IWUSR)|
            (ret.st_mode & S_IXUSR)|/* owner*/
            (ret.st_mode & S_IRGRP)|(ret.st_mode & S_IWGRP)|
            (ret.st_mode & S_IXGRP)|/* group*/
            (ret.st_mode & S_IROTH)|(ret.st_mode & S_IWOTH)|
            (ret.st_mode & S_IXOTH);/* other*/
}

void setMode(int mode, char *path) { //pour modif le autorisations
    char buff[100];
    sprintf(buff, "chmod %o %s", mode, path);
    system(buff);
}

int isDir(const char *fileName){ //si c'est un directory
    struct stat path;
    stat(fileName, &path); //recupere l'etat du fich et remplit path
    return S_ISDIR(path.st_mode);
}

//Q4.1
WorkFile* createWorkFile(char *name){ //creer un wf et l'initialiser
    WorkFile * wf = (WorkFile*)malloc(sizeof(WorkFile));
    wf-> name = strdup(name);
    wf-> hash = NULL;
    wf-> mode = 0;
    return wf;
}

//supp
void freeWorkFile(WorkFile *wf) { 
    if (wf) {
        free(wf->name);
        free(wf->hash);
        free(wf);
    }
    return;
}

//Q4.2
char* wfts(WorkFile *wf){ //convertir le wf en un chaine  contenat le diff champs separes par \t
    if(wf==NULL){
        return "";
    }
    char tmp[1000];
    sprintf(tmp, "%s\t%s\t%d%c", wf->name, wf->hash, wf->mode, '\0');
    char *res = strdup(tmp);
    return res;
}

//Q4.3
WorkFile* stwf(char *ch){ //chaine->wf
    char buff1[100];
    char buff2[100];
    int buff3;
    sscanf(ch, "%s\t%s\t%o", buff1, buff2, &buff3);
    WorkFile *new = createWorkFile(buff1);
    new->hash = strdup(buff2);
    new->mode = buff3;
    return new;
}

//Q4.4
WorkTree* initWorkTree(){ // allouer et initialiser un wt
    WorkTree *new = (WorkTree*)malloc(sizeof(WorkTree));
    new->size = TAILLE_MAX;
    new->n = 0;
    new->tab = (WorkFile *)malloc(TAILLE_MAX * sizeof(WorkFile));
    return new;
}

//supp
void freeWorkTree(WorkTree *wt) {
    int i = 0;
    WorkFile *p = wt->tab;
    while (i < wt->n) {
        WorkFile *wf = &(wt->tab[i]);
        free(wf->name);
        free(wf->hash);
        i++;
    }
    free(p);
    free(wt);
}

//Q4.5
int inWorkTree(WorkTree *wt, char *name){ //verif la presence et si present return position et -1 sinon
    for(int i=0; i< wt->n; i++){
        if(strcmp((wt->tab[i].name), name)==0){ //recherche dasn le tab si c'est le meme nom
            return i;
        }
    }
    return -1;
}

//Q4.6
int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode){ //ajoute un fich ou rep dans wt
    if(inWorkTree(wt,name) != -1){ //si le wf existe deja
        return 0;
    }
    if(wt->n >= wt->size){ //si le wt est rempli
        return 0;
    }

    wt->tab[wt->n].name = strdup(name);
    if(hash!=NULL){
        wt->tab[wt->n].hash = strdup(hash);
    }else{
        wt->tab[wt->n].hash = NULL;
    }
    wt->tab[wt->n].mode = mode;
    (wt->n)++;
    return 1;
}

//Q4.7
char* wtts(WorkTree* wt){ // wt->chaine de wf separes par \n
    char* res = (char*)malloc(sizeof(char)*10000);
    for(int i = 0; i< wt->n; i++){
        char* s = wfts(&(wt->tab[i]));
        strcat(res, s);
        strcat(res, "\n");
    }
    return res;
}

//Q4.8

//on a uitilise la fonc predefinie strtok qui permet d'extraire un a un les elem d'un chaine separees par qqch

WorkTree *stwt(char *ch){ //chaine->wt 
    WorkTree *wt = initWorkTree();
    char *tmp = strdup(ch);
    char *elem = strtok(tmp, "\n"); //premier element
    int i = 0;
    while(elem && i < wt->size) {
        WorkFile *wf = stwf(elem); //transf en workfile
        appendWorkTree(wt, wf->name, wf->hash, wf->mode); //ajout dans worktree
        elem = strtok(NULL, "\n"); //prochain elem
        freeWorkFile(wf); //elib mem
        i++;
    }
    free(tmp);
    return wt;
}

//Q4.9
int wttf(WorkTree* wt, char* file){ //ecrit dans file la chaine de wt
    FILE *f = fopen(file, "w");
    char *tmp = wtts(wt);
    fprintf(f, "%s", tmp);
    fclose(f);
    free(tmp);
    return 0;
}

//Q4.10
WorkTree* ftwt(char* file){ //file->chaine->wt
    FILE *f = fopen(file, "r");
    if(f == NULL){
        return NULL;
    }

    char buffer[256];

    WorkTree * wt = initWorkTree();
    while(fgets(buffer, 256, f)!= NULL){
        WorkFile * wf = stwf(buffer);
        appendWorkTree(wt, wf->name, wf->hash, wf->mode);
        freeWorkFile(wf);
  }
  fclose(f);
  return wt;
}

//Q5.1
char* blobWorkTree(WorkTree* wt){
    static char template[] = "/tmp/myfileXXXXXX";
    char fname[1000];
    strcpy(fname,template); //copie de template dans fname
    fname[17] = '\0';

    int fd= mkstemp(fname);//return un fich ouvert dont le nom sera stocke dans fname
    
    wttf(wt, fname); //ecrit dans fname le wt
    char *hash=sha256file(fname);
    char *hashPath = hashToPath(hash); //le chemin de ce fich
    char rep[3];
    rep[0] = hashPath[0];
    rep[1] = hashPath[1];
    rep[2] = '\0';

    if (!file_exists(rep)) {
        char *mk = malloc(sizeof(char) * 15);
        sprintf(mk, "mkdir %c%c", hashPath[0], hashPath[1]);
        system(mk);
        free(mk);
    }

    strcat(hashPath,".t"); //ajout .t
    cp(hashPath,fname);
    free(hashPath);
    return hash;
}

//Q5.2
char *concat_paths(char *path1, char *path2){ //on fait la concatenation de 2 paths
    char *res = malloc(strlen(path1) + strlen(path2) + 2);
    strcpy(res, path1); //ajout path1 to res
    strcat(res, "/");   
    strcat(res, path2); 
    return res;
}

char* saveWorkTree(WorkTree* wt, char* path){ //conserver l'etat de plusieurs fich a un instant donne
    for (int i = 0; i < wt->n; i++) { //parcourir le wt
        char *finalPath = concat_paths(path, wt->tab[i].name); //le chemin final jusqu'au workfile
        if(isDir(finalPath) == 0){ //si fichier
            blobFile(finalPath); //enregistre un inst
            wt->tab[i].hash = sha256file(finalPath); 
            wt->tab[i].mode = getChmod(finalPath); //recup l'autorisation
        }else{ //si repertoire
            List *list = listdir(finalPath);

            WorkTree *newWT = initWorkTree();
            Cell *tmp = *list;

            while (tmp) {
                if (tmp->data[0] != '.') {
                appendWorkTree(newWT, tmp->data, NULL, 0);
                }
                tmp = tmp->next;
            }

            freeListe(list);
            
            wt->tab[i].mode = getChmod(finalPath);
            char * hashNewWT = saveWorkTree(newWT, finalPath);
            wt->tab[i].hash = strdup(hashNewWT);
            free(hashNewWT);

            freeWorkTree(newWT);
        }
    free(finalPath);
  }
  return blobWorkTree(wt);
}

//Q5.3
int isWorkTree(char *hash) {

    char *path = hashToPath(hash);
    int index = -1;
    char *nom_fichier = path + 3;

    char *nom_fichier_tree = malloc(sizeof(char) * (1000 + 4));
    sprintf(nom_fichier_tree, "%s.t", nom_fichier);
    char rep[3];
    rep[0] = hash[0];
    rep[1] = hash[1];
    rep[2] = '\0';

    List *list = listdir(rep);

    if(searchList(list, nom_fichier_tree) != NULL) {
        index = 1;
    }else if(searchList(list, nom_fichier) != NULL){
        index = 0;
    }

    free(path);
    free(nom_fichier_tree);
    freeListe(list);
    
    return index;
}

void restoreWorkTree(WorkTree* wt, char* path){

    for (int i = 0; i < wt->n; i++) {

    char *pathTotal = concat_paths(path, wt->tab[i].name);
    char *hash = wt->tab[i].hash;
    char *hashCopie = hashToPath(hash);

    if(isWorkTree(hash) == 1) { // c'est un repertoire
      strcat(hashCopie, ".t");
      WorkTree *newWT = ftwt(hashCopie);

      restoreWorkTree(newWT, pathTotal);
      setMode(getChmod(hashCopie), pathTotal);
    }

    else if(isWorkTree(hash) == 0) { // c'est un fichier
      cp(pathTotal, hashCopie);
      setMode(getChmod(hashCopie), pathTotal);
    }

    free(pathTotal);
  }
}



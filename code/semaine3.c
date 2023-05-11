#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "header/Listes.h"
#include "header/Works.h"
#include "header/Commit.h"

#define TAILLE_MAX 1000
#define SIZE_COMMIT 10

//Q6.1
kvp* createKeyVal(char* key, char* val){ //alloue et initialise un elem
    kvp *kv = (kvp *)malloc(sizeof(kvp));
    if (key != NULL) {
        kv->key = strdup(key);
    }else{
        kv->key = NULL;
    }
    if (val != NULL) {
        kv->value = strdup(val);
    }else{
        kv->value = NULL;
    }
    return kv;
}

void freeKeyVal(kvp* kv){ //libere la memoire
    free(kv->key);
    free(kv->value);
    free(kv);
}

//Q6.2
char* kvts(kvp*k){ //Elem-> chaine de forme "cle :valeur"
    if(k==NULL) return NULL;
    char *chaine = (char*)malloc(100 * sizeof(char));
    chaine = strcat(chaine, k->key);
    chaine = strcat(chaine, " :");
    chaine = strcat(chaine, k->value);
    return chaine;
}

kvp *stkv(char *str){ //chaine->elem
  if (str==NULL) return NULL;
  char key[256], val[256];
  sscanf(str, "%s :%s", key, val);
  kvp *newKvp = createKeyVal(key, val);
  return newKvp;
}

//Q6.3
Commit* initCommit(){ //allouer et initialiser commit
    Commit* commit = (Commit*)malloc(sizeof(Commit));
    commit->n = 0;
    commit->size = SIZE_COMMIT;
    commit->T = (kvp**)malloc(SIZE_COMMIT * sizeof(kvp *));
    for (int i = 0; i < commit->size; i++) {
        commit->T[i] = NULL;
    }
    return commit;
}

void freeCommit(Commit *c){
    int i = 0;
    while(i < c->size) {
        if (c->T[i])
        freeKeyVal(c->T[i]);
        i++;
    }
    free(c->T);
    free(c);
}

//Q6.4
unsigned long sdbm(char *str){ //fonction hachage
    unsigned long hash = 0;
    int c;
    while(c == *(str++)){
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

//Q6.5
void commitSet(Commit* c, char* key, char* value){ //insere la paire(key, value) dans la table en gerant les collisions
    int hash = sdbm(key) % c->size;
    if (c->n == c->size){
        printf("Le commit est saturé\n");
        return;
    }
    while(c->T[hash]){
        hash = (hash + 1) % (c->size);
    }
    (c->n)++;
    c->T[hash] = createKeyVal(key, value);
    
}

//Q6.6
Commit* createCommit(char* hash){ //alloue et init un commit et ajoute l'elem oblig correspondant a la cle "tree"
    Commit *newCommit = initCommit();
    commitSet(newCommit, "tree", hash);
    return newCommit;
}

//Q6.7
char* commitGet(Commit* c, char* key){ //recherche dans la table sil existe un element avec 'key', return la valeur de l'element si existe, null sinon
    int i = sdbm(key) % c->size;
    int cpt = 0;
    while (c->T[i] && cpt < c->size) {
        if(strcmp(c->T[i]->key, key) == 0) //si on trouve l'elem
            return c->T[i]->value;
        i = (i + 1) % c->size;
        cpt++;
    }
    return NULL;
}

//Q6.8
char* cts(Commit* c){ //commit->chaine 
    char *s = (char*)malloc(c->n * 100 * sizeof(char));
    s[0] = '\0';
    for(int i = 0; i < c->size; i++){
        if(!(c->T[i])) continue;
        char *si = kvts(c->T[i]); //transf en chaine
        strcat(s, si); //ajout a s
        strcat(s, "\n"); //separer
        free(si);
    }
    s[strlen(s) - 1] = '\0';
    return s;
}

Commit* stc(char* ch){ //chaine->commit
    Commit *c = initCommit();
    char *elem = strtok(ch, "\n");
    while(elem) {
        kvp *tmp = stkv(elem);
        if(tmp){
            commitSet(c, tmp->key, tmp->value);
            freeKeyVal(tmp);
        }
        elem = strtok(NULL, "\n");
    }
    return c;
}

//Q6.9
void ctf(Commit* c, char* file){ //ecrit dans file la chaine de commit
    FILE *f = fopen(file, "w");
    if(!f){
        printf("Erreur: Impossible d'ouvrir %s\n", file);
        return;
    }
    char *s = cts(c);
    fputs(s, f);
    fclose(f);
    free(s);
}

Commit* ftc(char* file){ // new commit depuis le fichier
    FILE *f = fopen(file, "r");
    if(!f){
        printf("Erreur: Impossible d'ouvrir  %s\n", file);
        return NULL;
    }
    char buff[100]; // pour chaque ligne
    char s[1000];   // on met dedans le tout pour apres le transformer en commit
    s[0] = '\0';
    while(fgets(buff, 100, f)){
        strcat(s, buff);
    }
    fclose(f);
    Commit *c = stc(s);
    return c;
}

//Q6.10
char* blobCommit(Commit* c){
    static char template[] = "/tmp/myfileXXXXXX" ;
    char fname[1000];
    strcpy(fname, template);
    fname[17]='\0';

    int fd= mkstemp(fname);
    ctf(c, fname);

    char* hash=sha256file(fname);
    char* hashPath=hashToPath(hash);

    char rep[3];
    rep[0]= hashPath[0];
    rep[1]= hashPath[1];
    rep[2]= '\0';

    if(!file_exists(rep)){
        char *mk=malloc(sizeof(char)*256);
        sprintf(mk, "mkdir %c%c",hashPath[0], hashPath[1]);
        system(mk);
        free(mk);
    }

    strcat(hashPath, ".c");

    cp(hashPath, fname);
    free(hashPath);
    return hash;
}

//Q7.1
void initRefs(){ //cree le rep cache refs
    if (!file_exists(".refs")) {
        system("mkdir .refs");
        system("touch .refs/master");
        system("touch .refs/HEAD");
    }
}

//Q7.2
void createUpdateRef(char *ref_name, char *hash){ //maj une ref en remplacant son contenu par hash
    initRefs();
    if((hash != NULL) && (strlen(hash) != 0)){
        char chaine[256];
        sprintf(chaine, "echo %s > .refs/%s", hash, ref_name);
        system(chaine);

    }else{
        List* refs = listdir(".refs");
        if(searchList(refs, ref_name)==NULL){
            char comm[256];
            strcpy(comm, "touch ");
            strcat(comm, ".refs/");
            strcat(comm, ref_name);
            system(comm);
        }
    }
}

//Q7.3
void deleteRef(char* ref_name){ //supp la ref
    char buff[256];
    sprintf(buff, "rm .refs/%s", ref_name);
    List *directories = listdir(".refs");

    if(searchList(directories, ref_name) != NULL) {
        system(buff);
    }else{
        printf("La reference %s n'existe pas", ref_name);
    }

    freeListe(directories);
}

//Q7.4
char* getRef(char* ref_name){ //recupere vers quoi pointe une ref
    if(file_exists(".refs")) {
        List *directories=listdir(".refs");
        if(searchList(directories, ref_name)!= NULL){
            char *buffer = malloc(sizeof(char)*TAILLE_MAX);
            char *chemin = malloc(sizeof(char)*(TAILLE_MAX + 10));
            strcpy(chemin, ".refs/");
            strcat(chemin, ref_name);

            FILE *f = fopen(chemin, "r");
            if(fgets(buffer, TAILLE_MAX, f)==NULL){
                freeListe(directories);
                fclose(f);
                return "";
            }else{
                if(buffer[strlen(buffer)-1]=='\n'){
                    buffer[strlen(buffer)-1]='\0';
                }
                freeListe(directories);
                fclose(f);
                return buffer;
            }

        }else{
            printf("La reference %s n'existe pas!", ref_name);
            freeListe(directories);
            return NULL;
        }
    }
    return NULL;
}

//Q7.5
void myGitAdd(char* file_or_folder){ //ajout un fich ou rep dans le wt correspondant a la zone de prep
    WorkTree *wt;
    if(!file_exists(".add")){ //on cree le fichier .add sil nexiste pas
        system("touch .add");
        wt = initWorkTree();
    }else{
        wt = ftwt(".add");
    }
    if(file_exists(file_or_folder)){ //si le fichier existe on ajoute dans le worktree
        appendWorkTree(wt, file_or_folder, NULL, 0);
        wttf(wt, ".add"); //on ecrit le wt dans .add
    }else{
        printf("Le fichier ou repertoire %s n'existe pas\n", file_or_folder);
    }
    freeWorkTree(wt);
}

//Q7.6
void myGitCommit(char* branch_name, char* message){ //creer un point de sauvegarde
    if(!file_exists(".refs")){ //si le .refs n'existe pas
        printf("Initialiser d’abord les references du projet\n");
        return;
    }
    char *buff = malloc((TAILLE_MAX + 10) * sizeof(char));
    strcpy(buff, ".refs/");
    strcat(buff, branch_name);

    List *directories = listdir(".refs");

    if(searchList(directories, branch_name) == NULL){
        printf("La branche %s n'existe pas!\n", branch_name);
        return;
    }

    char *last_hash=getRef(branch_name);//contenu du dernier commit de la branche
    char *head_hash = getRef("HEAD"); // head_ref[strlen(head_ref) -1]='\0';
    if(!((strlen(last_hash)==0)&&(strlen(head_hash)==0))){
        if(strcmp(last_hash, head_hash)!= 0){ //si les fichiers ne pointent pas vers la meme chose
            printf("HEAD doit pointer sur le dernier Commit de la branche\n");
            return;
        }
    }

    WorkTree *wt = ftwt(".add"); //on transf le fichier .add en wt

    char* hash = saveWorkTree(wt, "."); //recuperer le hash de wt, "."= rep courant

    if(hash[strlen(hash)-1]=='\n'){
        hash[strlen(hash)-1]='\0';
    }

    Commit *c = createCommit(hash); //creer commit

    FILE *f = fopen(buff, "r");
    char buffer[3* TAILLE_MAX];

    if(strlen(last_hash)!= 0){ //slm si le fich branch_name n'est pas vide
        commitSet(c, "predecessor", last_hash); //ajout comme predecessor
    }
    if(message){ //si il y'a un message
        commitSet(c, "message", message);
    }
    commitSet(c, "author", "DANILA Catalina et SAAD Maria");

    char *hashCommit = blobCommit(c);
    if(hashCommit[strlen(hashCommit)-1]=='\n'){
        hashCommit[strlen(hashCommit)-1]='\0';
    }

    createUpdateRef(branch_name, hashCommit);
    createUpdateRef("HEAD", hashCommit);
    system("rm .add"); //supprime .add

    freeListe(directories);
    free(buff);
    free(hashCommit);
    free(hash);
    freeCommit(c);
    freeWorkTree(wt);
}





#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#define NB_LIGNES 6
#define NB_COL 7
#define AI_DEPTH 5

// Variables globales permettant la gestion du débugage et de quand l'IA doit jouer 
// IA = 0: joueur 1; =1: joueur 2; =2: ne joue pas (mode humain vs humain)
// DEBUG = 0: n'affiche pas les printf de debugage; =1: affiche tous les printf
uint8_t IA = 2;
uint8_t DEBUG = 0;

//Structure utilisée pour stocker les grilles du jeu

typedef struct grille
{
    char **tab;
    uint16_t currP;
    uint8_t gameStatus;
} grille;
// Structure pour construire l'arbre de coups:
// La structure utiliseée est un type de liste qui contient la grille de la position après n coup
// Reval qui est le score du coup et qui est assigné en remontant l'arbre ou en appelant scorePosition
// eval est un reste d'un essai pécedent, n'est pas utilisé
typedef struct node{
    int32_t eval;
    int32_t Reval;
    grille * position;
    struct node * child[NB_COL];
}node_t;


// Fonctions s'allocation mémoire et libération mémoire:
grille* creerGrille();// créer une grille
node_t * creerNode(grille *g);//    créer un noeud de l'arbre
void freeNode(node_t * n);//        libère un noeud
void freeAllNodes(node_t * n);//    libère un arbre
grille *copierGrille(grille * g);// fait une copie de la grille passée en paramètre
void freeGrille(grille *g);//       libère un grille

// Fonctions de contruction d'arbre et de recherche de coup
void construireArbre(int depth,node_t * node);//        construit un arbre par rapport à la racine passée en paramètre(récursif)
uint8_t permutationGrille(grille * g, char coup);//     joue le coup @coup sur la grille g, si le coup est illégal retourne 0 sinon 1 
void generateChilds(node_t * n);//                      genère le NB_COL enfant du noeud n en permutant sa grille
uint8_t getBestMove(node_t * root,uint8_t side);//      calcule en remontant l'evaluation d'une feuille au NB_COL premier enfant
//(^la valeur de retour n'est pas utilisée)
void getBestPositon(node_t * root,uint8_t side);//      (fonction non utilisée) évalue chaque noeud de l'arbre et le stock dans eval
uint8_t isFeuille(node_t * node);//                     retourne 1 si le noeud est une feuille sinon 0

// Fonctions d'évaluation de position
int16_t scorePosition(grille * g,uint8_t side1,char side2);//   retourne le score d'une position
uint8_t jetonCount(grille* g,uint8_t i, uint8_t j, uint8_t depth,char side,int dir1,int dir2);// compte le nombre de jetons à 4 cases adjacentes pour chaque case d'une grille
int16_t returnScoreOfJeton(int16_t sc,uint8_t side);//          retourne les valeurs des score en fonction du nombre de jetons detectés

// Fonctions de jeu
void poserJeton(grille *g, char pos);
uint8_t checkVictoireRec(grille* g, char side,int,int);// détecte une victiore en fonction du dernier coup joué
uint8_t interCheckVictoireRec(grille* g,uint8_t i, uint8_t j, uint8_t depth,char side, int, int);// utilisé pour detecter une victiore
char jouerCoup(grille *g);// joue un coup

// Fonctions d'affichage
void printGrille(grille *g);
void printABR(node_t * node,int8_t depth);

node_t * creerNode(grille *g){
    node_t * n = malloc(sizeof(node_t));
    for(int i = 0;i < NB_COL;i++ ){
        n->child[i] = NULL;
    }
    n->position = copierGrille(g);
    n->eval = 0;
    n->Reval = 0;
    return n;
}

void generateChilds(node_t * n){
    if(n == NULL)
        return;
    for(int i = 0; i < NB_COL; i++){
        n->child[i] = creerNode(n->position);
        if(!permutationGrille(n->child[i]->position, 'A' + i)){
            freeNode(n->child[i]);
            n->child[i] = NULL;
            continue;
        }
    }
}

void freeNode(node_t * n){
    freeGrille(n->position);
    free(n);
}


void freeAllNodes(node_t * n){
    for(uint8_t i = 0; i<NB_COL; ++i){
        if(n->child[i] != NULL){
            freeAllNodes(n->child[i]);
        }
           
    }
    freeNode(n);
}
void printABR(node_t * node,int8_t depth){
    for(int i = 0; i <NB_COL; i++){
        if(node->child[i] == NULL) continue;
        printABR(node->child[i],depth + 1);
        if(DEBUG)
            printf("prof %d, enfant %d, cuurp = %d , eval = %d, Reval = %d \n",depth,i,node->child[i]->position->currP,node->child[i]->eval,node->child[i]->Reval);
        printGrille(node->child[i]->position);
        printf("\n");
    }
}

void construireArbre(int depth,node_t * node){
    if(depth == 0){
        return;
    }
    
    generateChilds(node);
   
    for(int i = 0; i <NB_COL; i++){
        if(node->child[i] == NULL) continue;
        int coord = 0;
        while (node->child[i]->position->tab[coord][i] == ' ')
        {
            ++coord;
        }
        if(checkVictoireRec(node->child[i]->position,'X',coord,i) || checkVictoireRec(node->child[i]->position,'O',coord,i) ){
            //printf("victoire trouvée");
            continue;
        }else{
             construireArbre(depth - 1, node->child[i]);
        }
       
    }
}

void getBestPositon(node_t * root,uint8_t iside){
    
    for(int i = 0; i < NB_COL ; i++){
        if(root->child[i] != NULL){
            getBestPositon(root->child[i],iside);
        }

    }
    char side[2] = {'O','X'};
    root->eval = scorePosition(root->position,side[iside],side[!iside]);  

}

uint8_t isFeuille(node_t * node){
    for(int i = 0; i <NB_COL; i++){
        if(node->child[i] != NULL) return 0; ;
        
    }
    return 1;
}

uint8_t getBestMove(node_t * root,uint8_t iside){
    // retourne l'indice du meilleur coup
    uint8_t index = 0;
    int16_t bestEval = -5600;
    if(root == NULL){
        return 0;
    }
    for(int i = 0; i < NB_COL ; i++){
        if(root->child[i] != NULL){
           index = getBestMove(root->child[i],iside);
        }

    }
    index = 0;
    if(isFeuille(root)){
        char side[2] = {'O','X'};
        root->Reval = scorePosition(root->position,side[iside],side[!iside]);  
        return 0;
    }
    for(int i = 0; i < NB_COL ; i++){
        if(root->child[i] == NULL){
           continue;
        }
        if(iside == root->child[i]->position->currP){
            if(root->child[i]->Reval > bestEval){
                bestEval = root->child[i]->Reval;
                index = i;
            }
        }
        if(iside != root->child[i]->position->currP){
            if(bestEval == -5600){
                bestEval = root->child[i]->Reval;
            }
            if(root->child[i]->Reval < bestEval){
                bestEval = root->child[i]->Reval;
                index = i;
            }
        }

    }
     if(bestEval == -5600){
        bestEval = 0;
     }
    root->Reval = bestEval;

    

    return index;

}




grille *copierGrille(grille * g){
    grille *gc = creerGrille();
     for (uint8_t i = 0; i < NB_LIGNES; ++i)
    {
        for (uint16_t j = 0; j < NB_COL; ++j)
        {
            gc->tab[i][j] = g->tab[i][j];
        }
    }
    gc->currP = g->currP ;
    gc->gameStatus = g->gameStatus;
    return gc;

}

uint8_t permutationGrille(grille * g,char coup){
    if (g->tab[0][coup-'A'] != ' ')
    {
       return 0;
    }
    poserJeton(g,coup);
    //changer le p.currp jesais pas comment sa marche
    g->currP = 1 - g->currP;
    return 1;
}







grille *creerGrille()
{
    grille *g = malloc(sizeof(grille));
    if(g == NULL) assert(0);
    g->tab = (char**)malloc(sizeof(char*) * NB_LIGNES);
    for (uint8_t i = 0; i < NB_LIGNES; ++i)
    {
        g->tab[i] = (char*)malloc(sizeof(char) * NB_COL);
        for (uint16_t j = 0; j < NB_COL; ++j)
        {
            g->tab[i][j] = ' ';
        }
    }
    g->currP = 0;
    g->gameStatus = 0;
    return g;
}

void printGrille(grille *g)
{
    printf("    A  B  C  D  E  F  G\n");
    for (uint8_t i = 0; i < NB_LIGNES; ++i)
    {
        printf("⬛ ");
        for (uint8_t j = 0; j < NB_COL; ++j)
        {
            printf("%s ", (g->tab[i][j] == ' ' ? "🟦" : (g->tab[i][j] == 'X' ? "🔴": "🟡")));
        }
        printf("⬛\n");
    }
    printf("⬛ ⬛ ⬛ ⬛ ⬛ ⬛ ⬛ ⬛ ⬛\n\n");
}

void poserJeton(grille *g, char pos)
{
    uint8_t i = 5;
    char pion[2] = {'X', 'O'};
    while (g->tab[i][pos - 'A'] != ' ')
    {
        --i;
    }
    g->tab[i][pos - 'A'] = pion[g->currP];
}

char jouerCoup(grille *g)
{
    char input = 'A';

    printf("Joueur %d à toi de jouer!\n", g->currP + 1);
    scanf(" %c", &input);
    chercherCoup:
    while ((input > 'G')||(input < 'A')){
        printf("Coup non valide, veillez entrez une lettre entre A et G.\n");
        scanf(" %c", &input);
    }
    if (g->tab[0][input-'A'] != ' ')
    {
        printf("Cette colonne est pleine!\n");
        input = ' ';
        goto chercherCoup;
    }
    
    poserJeton(g, input);
    return input;
}

uint8_t checkVictoireRec(grille* g, char side,int i ,int j){ 
    
    if(interCheckVictoireRec(g,i,j,4,side,1,1)
    + interCheckVictoireRec(g,i,j,4,side,-1,-1) > 4){
        return side;
    }  
    if(interCheckVictoireRec(g,i,j,4,side,0,1)
    + interCheckVictoireRec(g,i,j,4,side,0,-1) > 4){
        return side;
    }  
    if(interCheckVictoireRec(g,i,j,4,side,1,0)
    + interCheckVictoireRec(g,i,j,4,side,-1,0) > 4){
        return side;
    }
    if(interCheckVictoireRec(g,i,j,4,side,1,-1)
    + interCheckVictoireRec(g,i,j,4,side,-1,1) > 4){
        return side;
    }
        
    
    return 0;
}

int16_t scorePosition(grille * g,uint8_t side1,char side2){
    int16_t score = 0;
    for(uint8_t i = 0; i <NB_LIGNES; i++){
        for(uint8_t j = 0; j <NB_COL; j++){
          
                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,1,1),1)/2  + returnScoreOfJeton(jetonCount(g,i,j,4,side2,1,1),0)/2 ;
                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,1,0),1)/2 + returnScoreOfJeton(jetonCount(g,i,j,4,side2,1,0),0)/2 ;
                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,0,1),1)/2 + returnScoreOfJeton(jetonCount(g,i,j,4,side2,0,1),0)/2 ;
                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,1,-1),1)/2 + returnScoreOfJeton(jetonCount(g,i,j,4,side2,1,-1),0)/2 ;

                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,-1,-1),1)/2  + returnScoreOfJeton(jetonCount(g,i,j,4,side2,-1,-1),0)/2 ;
                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,-1,0),1)/2 + returnScoreOfJeton(jetonCount(g,i,j,4,side2,-1,0),0)/2 ;
                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,0,-1),1)/2 + returnScoreOfJeton(jetonCount(g,i,j,4,side2,0,-1),0)/2 ;
                score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,-1,1),1)/2 + returnScoreOfJeton(jetonCount(g,i,j,4,side2,-1,1),0)/2;
            
        }
    }
    return score;
}

int16_t returnScoreOfJeton(int16_t sc,uint8_t side){
    if(sc == 4 && side ){
        return 100;
    }

    if(sc == 4 && !side ){
        return -95;
    }

    if(sc == 3 && side ){
        return 5;
    }

     if(sc == 3 && !side ){
        return -4;
    }
    if(sc == 2 && side ){
        return 2;
    }
    if(sc == 2 && !side ){
        return -1;
    }
    return 0;
}

uint8_t jetonCount(grille* g,uint8_t i, uint8_t j, uint8_t depth,char side,int dir1,int dir2){
     if(i < 0 || 
        j < 0 ||
        i >= NB_LIGNES ||
        j >= NB_COL){
        return 0;
    }
    int8_t d = 0;
    if(depth == 0){
        return 0;
    }
   
       
    d = jetonCount(g,i + dir1,j + dir2,depth - 1,side, dir1, dir2);
   
     
    if(g->tab[i][j] != side && g->tab[i][j] != ' '){
       
        return -1;
    }
    if(d == -1){
        return -1;
    }
    if(d >= 4){
        return 4;
    }
    if(g->tab[i][j] == side){
        return d + 1;
    }
    
    return d;
  
    
}

uint8_t interCheckVictoireRec(grille* g,uint8_t i, uint8_t j, uint8_t depth,char side,int dir1,int dir2){
    if(i < 0 || 
        j < 0 ||
        i >= NB_LIGNES ||
        j >= NB_COL){
        return 0;
        }
    if(g->tab[i][j] != side){
       
        return 0;
    }
    if(depth == 0){
        return 0;
    }
    uint8_t d;
    d = interCheckVictoireRec(g,i + dir1,j + dir2,depth - 1,side, dir1, dir2);
    return d+1;
}

void freeGrille(grille *g)
{
    for (uint8_t i = 0; i < NB_LIGNES; ++i)
    {
        free(g->tab[i]);
    }
    free(g->tab);
    free(g);
}

int main(int argc, char* argv[])
{
    grille *g = creerGrille();
    char input;
    char* titleScreen = "..................................................................................................................................................\n.......PPPPPPPPPP...UUUU..UUUU...IIII.....SSSSSSS........SSSSSSS........AAAAAA.........NNNNNNN......NNNN......CCCCCC.......EEEEEEEEE......444444..\n......PPPP..PPPP...UUUU..UUUU...IIII...SSSS.....SSS...SSSS.....SSS.....AAAAAAA........NNNN.NNNN....NNNN....CCCC...CCCC....EEEE.........44444444...\n.....PPPPPPPPPP...UUUU..UUUU...IIII.......SSSS...........SSSS.........AAAA.AAA.......NNNN..NNNN...NNNN....CCCC...........EEEE.......4444..4444....\n....PPPP.........UUUU..UUUU...IIII..........SSSS...........SSSS......AAAAAAAAA......NNNN...NNNN..NNNN....CCCC...........EEEEEEE...4444444444444...\n...PPPP.........UUUU..UUUU...IIII....SSSS.....SSSS..SSSS.....SSSS...AAAA...AAA.....NNNN....NNNN.NNNN....CCCC...CCCC....EEEE.............4444......\n..PPPP.........UUUUUUUUUU...IIII.......SSSSSSS........SSSSSSS......AAAA....AAA....NNNN......NNNNNNN.......CCCCCC......EEEEEEEEEE.......4444.......\n..................................................................................................................................................\n";
    for (uint16_t i = 0; i < strlen(titleScreen) ; i++)
    {
        printf("%c",titleScreen[i]);
        fflush(NULL);
        usleep(500);
    }
    usleep(500000);
    
    if (argc>1)
    {
        IA = atoi(argv[1]);
    }
    
    if(argc>2){
        DEBUG = atoi(argv[2]);
    }
    if(DEBUG)
        printf("IA = %d \n", IA);


    printGrille(g);
    g->currP = 1 - g->currP;
    while(g->gameStatus == 0){
        g->currP = 1 - g->currP;
        node_t * root = creerNode(g);
        construireArbre(AI_DEPTH,root);
        //getBestPositon(root,!g->currP);
        getBestMove(root,!g->currP);
        if(DEBUG)
            printf("notre tour = %d \n",!g->currP);
        //printABR(root,0);
        uint8_t pos = 0;
        for(int i = 0; i<NB_COL;i++){
            if(root->child[i] == NULL){
                if(pos == i){
                    pos++;
                }
                continue;
            }
            if(isFeuille(root->child[i])){
                pos = i;
                break;
            }
            if(root->child[i]->Reval > root->child[pos]->Reval ){
                pos = i;
            }
        }
      
        if(g->currP == IA ){
            for(int i = 0; i< NB_COL; i++){
                if(root->child[i] == NULL)continue;
                if(DEBUG)
                    printf("move %c has score of %d and Reval of %d\n", 'A' + i, root->child[i]->eval,root->child[i]->Reval);
            }
            if(DEBUG)
                printf("the best move is %c with Reval of %d\n",'A' + pos,root->child[pos]->Reval);
            input = 'A' + pos;
            poserJeton(g, input);
        }
        else{
             for(int i = 0; i< NB_COL; i++){
                if(root->child[i] == NULL)continue;
                if(DEBUG)
                    printf("move %c has score of %d and Reval of %d\n", 'A' + i, root->child[i]->eval,root->child[i]->Reval);
            }
            if(DEBUG)
                printf("the best move is %c with Reval of %d\n",'A' + pos,root->child[pos]->Reval);
            input = jouerCoup(g);
        }
      
        //printGrille(root->child[pos]->position);
        freeAllNodes(root);
       
        
        printGrille(g);
        uint8_t coord = 0;
        char pion[2] = {'X', 'O'};
        char* joueur[2] = {"humain", "IA"};
        if (IA == 2)
        {
            joueur[0] = "humain 1";
            joueur[1] = "humain 2";
        }
        
        printf("Le joueur %s a joué le coup %c\n", IA == 0 ? joueur[!g->currP]: joueur[g->currP], input);
        while (g->tab[coord][input - 'A'] == ' ')
        {
            ++coord;
        }
        g->gameStatus = checkVictoireRec(g,pion[g->currP] ,coord,input - 'A');
        switch (g->gameStatus)
            {
            case 'O':
            case 'X':
                printf("Bravo, le joueur %s a gagné\n Voulez-vous rejouer? [y/n] \n", joueur[g->currP]);
                scanf(" %c", &input);
                if(input == 'y'){
                    freeGrille(g);
                    g = creerGrille();
                    g->currP = 1 - g->currP;}
                break;
            case 2:
                printf("Égalité! Personne n'a perdu, personne n'a gagné!\n Voulez-vous rejouer? [y/n] \n");
                scanf(" %c", &input);
                if(input == 'y'){
                    freeGrille(g);
                    g = creerGrille();
                    g->currP = 1 - g->currP;}
                break;
            default:
                break;
            }
    }
    freeGrille(g);
    return EXIT_SUCCESS;
}
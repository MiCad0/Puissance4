#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define NB_LIGNES 6
#define NB_COL 7



typedef struct grille
{
    char **tab;
    uint16_t currP;
    uint8_t gameStatus;
} grille;

typedef struct node{
    int32_t eval;
    int32_t Reval;
    grille * position;
    struct node * child[NB_COL];
}node_t;

grille* creerGrille();
void printGrille(grille *g);
void poserJeton(grille *g, char pos);
void freeGrille(grille *g);
uint8_t checkVictoireRec(grille* g, char side,int,int);
uint8_t interCheckVictoireRec(grille* g,uint8_t i, uint8_t j, uint8_t depth,char side, int, int);

char jouerCoup(grille *g);
node_t * creerNode(grille *g);
grille *copierGrille(grille * g);
uint8_t permutationGrille(grille * g, char coup);
void generateChilds(node_t n[NB_COL]);

void freeNode(node_t * n);
void freeAllNodes(node_t * n);
void construireArbre(int depth,node_t * node);
void getBestPositon(node_t * root,uint8_t side);
uint8_t getBestMove(node_t * root,uint8_t side);

int16_t scorePosition(grille * g,uint8_t side1,char side2);
int16_t returnScoreOfJeton(int16_t sc,uint8_t side);

uint8_t jetonCount(grille* g,uint8_t i, uint8_t j, uint8_t depth,char side,int dir1,int dir2);

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
        n->child[i] = creerNode(copierGrille(n->position));
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
    char side[2] = {'X','O'};
   
    for(int i = 0; i <NB_COL; i++){
        if(node->child[i] == NULL) continue;
        int coord = 0;
        while (node->child[i]->position->tab[coord][i] == ' ')
        {
            ++coord;
        }
        if(checkVictoireRec(node->child[i]->position,side[node->child[i]->position->currP],i,coord)){
            //printf("victoire trouver");
            continue;
        }
        construireArbre(depth - 1, node->child[i]);
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

uint8_t getBestMove(node_t * root,uint8_t side){
    // retourne l'indice du meilleur coup
    uint8_t index = 0;
    int16_t bestEval = -5600;
    if(root == NULL){
        return 0;
    }
    for(int i = 0; i < NB_COL ; i++){
        if(root->child[i] != NULL){
           index = getBestMove(root->child[i],side);
        }

    }
    index = 0;
    for(int i = 0; i < NB_COL ; i++){
        if(root->child[i] == NULL){
           continue;
        }
        if(side == root->child[i]->position->currP){
            if(root->child[i]->Reval > bestEval){
                bestEval = root->child[i]->Reval;
                index = i;
            }
        }
        if(side != root->child[i]->position->currP){
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
    root->Reval = root->eval + bestEval;

    

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
    printf("  A  B  C  D  E  F  G  \n");
    for (uint8_t i = 0; i < NB_LIGNES; ++i)
    {
        for (uint8_t j = 0; j < NB_COL; ++j)
        {
            printf("||%c", g->tab[i][j]);
        }
        printf("||\n");
    }
    printf("=======================\n\n");
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
    char action = 'A';
    printf("Joueur %d à toi de jouer!\n", g->currP + 1);
    scanf(" %c", &action);
    chercherCoup:
    while ((action > 'G')||(action < 'A')){
        printf("Coup non valide, veillez entrez une lettre entre A et G.\n");
        scanf(" %c", &action);
    }
    if (g->tab[0][action-'A'] != ' ')
    {
        printf("Cette colonne est pleine!\n");
        action = ' ';
        goto chercherCoup;
    }
    
    poserJeton(g, action);
    return action;
}

uint8_t checkVictoireRec(grille* g, char side,int i ,int j){
        //printf("pre: %c;%c,(%d,%d)\n", side, g->tab[i][j], i, j);
    
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
            
            score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,1,1),1)  + returnScoreOfJeton(jetonCount(g,i,j,4,side2,1,1),0) ;
            score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,1,0),1) + returnScoreOfJeton(jetonCount(g,i,j,4,side2,1,0),0) ;
            score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,0,1),1) + returnScoreOfJeton(jetonCount(g,i,j,4,side2,0,1),0) ;
            score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,1,-1),1) + returnScoreOfJeton(jetonCount(g,i,j,4,side2,1,-1),0) ;

             score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,-1,-1),1)  + returnScoreOfJeton(jetonCount(g,i,j,4,side2,-1,-1),0) ;
            score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,-1,0),1) + returnScoreOfJeton(jetonCount(g,i,j,4,side2,-1,0),0) ;
            score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,0,-1),1) + returnScoreOfJeton(jetonCount(g,i,j,4,side2,0,-1),0) ;
            score += returnScoreOfJeton(jetonCount(g,i,j,4,side1,-1,1),1) + returnScoreOfJeton(jetonCount(g,i,j,4,side2,-1,1),0) ;
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
    
    //int d[8] = interCheckVictoireRec(g,i,j,depth - 1,side );
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

int main()
{
    grille *g = creerGrille();
    char action;

    printGrille(g);
    g->currP = 1 - g->currP;
    while(g->gameStatus == 0){
        g->currP = 1 - g->currP;
        node_t * root = creerNode(g);
        construireArbre(5,root);
        getBestPositon(root,!g->currP);
        getBestMove(root,!g->currP);
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
            if(root->child[i]->Reval > root->child[pos]->Reval ){
                pos = i;
            }
        }
      
        //if(g->currP == 0){
            for(int i = 0; i< NB_COL; i++){
                if(root->child[i] == NULL)continue;
                printf("move %c has score of %d and Reval of %d\n", 'A' + i, root->child[i]->eval,root->child[i]->Reval);
            }
            printf("the best move is %c with Reval of %d\n",'A' + pos,root->child[pos]->Reval);
        //}
      
        //printGrille(root->child[pos]->position);
        freeAllNodes(root);
        action = 'A' + pos;
        poserJeton(g, action);
        //action = jouerCoup(g);
        printGrille(g);
        uint8_t coord = 0;
        char pion[2] = {'X', 'O'};
        printf("action = %c\n",action);
        while (g->tab[coord][action - 'A'] == ' ')
        {
            ++coord;
        }
        if(checkVictoireRec(g,pion[g->currP] ,coord,action - 'A') != 0){
            g->gameStatus = 1;
            printf("Bravo, le joueur %d a gagné\n Voulez-vous rejouer? [y/n] \n", g->currP + 1);
            scanf(" %c", &action);
            if(action == 'y'){
                freeGrille(g);
                g = creerGrille();
                g->currP = 1 - g->currP;}
        }
    }
    freeGrille(g);
    return EXIT_SUCCESS;
}
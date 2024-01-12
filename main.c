#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define NB_LIGNES 6
#define NB_COL 7



typedef struct grille
{
    char *tab[NB_LIGNES];
    uint16_t currP;
    uint8_t gameStatus;
} grille;

grille *creerGrille();
void printGrille(grille *g);
char poserJeton(grille *g, char pos);
void freeGrille(grille *g);
uint8_t checkVictoireRec(grille* g, char side,int,int);
uint8_t interCheckVictoireRec(grille* g,uint8_t i, uint8_t j, uint8_t depth,char side, int, int);
char jouerCoup(grille *g);


grille *creerGrille()
{
    grille *g = malloc(sizeof(grille));
    for (uint8_t i = 0; i < NB_LIGNES; ++i)
    {
        g->tab[i] = malloc(sizeof(char));
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

char poserJeton(grille *g, char pos)
{
    uint8_t i = 5;
    char pion[2] = {'X', 'O'};
    while (g->tab[i][pos - 'A'] != ' ')
    {
        --i;
    }
    g->tab[i][pos - 'A'] = pion[g->currP];
    return pion[g->currP];
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
    
    action = poserJeton(g, action);
    return action;
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
        action = jouerCoup(g);
        printGrille(g);
        uint8_t coord = 5;
        char pion[2] = {'X', 'O'};
        while (g->tab[i][pos - 'A'] != ' ')
        {
            --coord;
        }

        if(checkVictoireRec(g,pion[g->currP] ,coord,pos - 'A') != 0){//FIXME!!! Blyat
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
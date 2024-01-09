#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

typedef struct grille
{
    char *tab[6];
    uint16_t currP;
    uint8_t gameStatus;
} grille;

grille *creerGrille();
void printGrille(grille *g);
void poserJeton(grille *g, char pos);
void freeGrille(grille *g);
char checkVictoire(grille *g, char pos);
char jouerCoup(grille *g);


grille *creerGrille()
{
    grille *g = malloc(sizeof(grille));
    for (uint8_t i = 0; i < 6; ++i)
    {
        g->tab[i] = malloc(sizeof(char));
        for (uint16_t j = 0; j < 7; ++j)
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
    for (uint8_t i = 0; i < 6; ++i)
    {
        for (uint8_t j = 0; j < 7; ++j)
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
    if (g->tab[5][action-'A'] != ' ')
    {
        printf("Cette colonne est pleine!\n");
        goto chercherCoup;
    }
    
    poserJeton(g, action);
    return action;
}




void freeGrille(grille *g)
{
    for (uint8_t i = 0; i < 6; ++i)
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
        if(checkVictoire(g, action) == 'V'){
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
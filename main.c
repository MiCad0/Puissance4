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


char checkVictoire(grille* g, char pos){
    uint16_t x;
    uint16_t y = pos - 'A';
    uint16_t i = 0;
    uint16_t score = 1;
    uint16_t check = 1;

    while (g->tab[i][y] == ' ')
    {
        ++i;
    }
    x = i;
    char pion = g->tab[x][y];
    
    while (check)          //Check de victoire en dessous
    {
        if (i<5)
        {
            if (g->tab[i+1][y] == pion)
            {
                if(score < 4){
                    ++score;
                }
                else check = 0;
                ++i;
            }
            else check = 0;
        }
        else check = 0;
        
    }
    if (score == 4)
    {
        return 'V';
    }
    else i = y;

    check = 1;
    score = 1;
    while (check)          //Check victoire sur le coté
    {
        if (i>0)
        {
            if (g->tab[x][i-1] == pion)
            {
                if(score < 4){
                    ++score;
                }
                else check = 0;
                --i;
            }
            else check = 0;
        }
        else check = 0;
    }
    if (score == 4)
    {
        return 'V';
    }
    else i = x;

    while (check)          //Suite de check victoire sur le coté
    {
        if (i>0)
        {
            if (g->tab[x][i+1] == pion)
            {
                if(score < 4){
                    ++score;
                }
                else check = 0;
                --i;
            }
            else check = 0;
        }
        else check = 0;
    }
    if (score == 4)
    {
        return 'V';
    }
    else i = 1;
    score = 1;
    check = 1;

    
    while (check)          //Check en diagonale
    {
        if ((x+i<=6)||(y+i<=5))
        {
            if (g->tab[x+i][y+i] == pion)
            {
                if(score < 4){
                    ++score;
                }
                else check = 0;
                --i;
            }
            else check = 0;
        }
        else check = 0;
    }
    if (score == 4)
    {
        return 'V';
    }
    else i = 1;
    check = 1;
    while (check)          //Suite de check en diagonale
    {
        if ((x+i>=0)&&(y+i>=0))
        {
            if (g->tab[x-i][y-i] == pion)
            {
                if(score < 4){
                    ++score;
                }
                else check = 0;
                --i;
            }
            else check = 0;
        }
        else check = 0;
    }
    if (score == 4)
    {
        return 'V';
    }
    else i = 1;
    score = 1;
    check = 1;
    while (check)          //Check dans l'autre diagonale
    {
        if ((x+i<=6)&&(y+i>=0))
        {
            if (g->tab[x+i][y-i] == pion)
            {
                if(score < 4){
                    ++score;
                }
                else check = 0;
                --i;
            }
            else check = 0;
        }
        else check = 0;
    }
    if (score == 4)
    {
        return 'V';
    }
    else i = 1;
    check = 1;

    while (check)          //Suite de check dans l'autre diagonale
    {
        if ((x+i>=0)&&(y+i<=5))
        {
            if (g->tab[x-i][y+i] == pion)
            {
                if(score < 4){
                    ++score;
                }
                else check = 0;
                --i;
            }
            else check = 0;
        }
        else check = 0;
    }
    if (score == 4)
    {
        return 'V';
    }
    else return 'N';    
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
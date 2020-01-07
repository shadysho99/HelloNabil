/* EL OUAHABI NABIL, 000463021*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct joueur
{
    int identifiant;
    float score;
    int total_parties_remportees;
} joueur;

typedef struct tableau
{
	int total_players;
	joueur* tab_joueur;
} tableau;

tableau new_tableau();

void add_player(tableau* tab, int identifiant, float score, int parties_remportees);

tableau update_tableau(int identifiant1,int identifiant2,float score, tableau tab);

int search_id(int id, tableau* tab);

void free_memory_tableau(joueur* tab_joueur);

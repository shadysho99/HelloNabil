/* EL OUAHABI NABIL, 000463021*/


#include "bibli.h"

tableau new_tableau(){   // on crée tableau et on crée le tableau de joueur qu'il contient et on initialise total_players à 0
	tableau nouveau_tableau;
	nouveau_tableau.tab_joueur = (joueur *)malloc(5*sizeof(joueur));
	if (nouveau_tableau.tab_joueur == NULL){
		printf("Erreur allocation de mémoire");
		exit(EXIT_FAILURE);
	}
	nouveau_tableau.total_players = 0;
	return nouveau_tableau;
}

void add_player(tableau* tab, int identifiant, float score, int parties_remportees){
	joueur new; 
	new.identifiant = identifiant; 
	new.score = score;
	new.total_parties_remportees = parties_remportees;
	if(tab->total_players >= 5){							// Si la mémoire n'est pas suffisante, on realloc 
		tab->tab_joueur = (joueur *) realloc(tab->tab_joueur,(tab->total_players+1)*sizeof(joueur));
		if (tab->tab_joueur == NULL){
			printf("Erreur ré-allocation de mémoire");
			exit(EXIT_FAILURE);
		}
	}
	tab->tab_joueur[tab->total_players] = new;
	tab->total_players+=1;
}

tableau update_tableau(int identifiant1,int identifiant2,float score,tableau tab){	  // fonction principale qui va remplir le tableau de joueur en fonction de chaque paramètre
	int id1 = search_id(identifiant1, &tab);  
	int id2 = search_id(identifiant2, &tab);     // On regarde si les identifiants existent déjà dans le tableau, s'ils n'existent pas, id = -1
		
	if (score == 1 || score == 0){		
		if (score == 1){
			if(id1 == -1){
				add_player(&tab, identifiant1, 1, 1);
			}else{
				tab.tab_joueur[id1].score += 1;
				tab.tab_joueur[id1].total_parties_remportees+=1;
			}
			if (id2 == -1){
				add_player(&tab, identifiant2, 0, 0);
			}
			
		}else{
			if (id2 == -1){
				add_player(&tab, identifiant2, 1, 1);
			}else{
				tab.tab_joueur[id2].score += 1;
				tab.tab_joueur[id2].total_parties_remportees+=1;
			}
			if (id1 == -1){
				add_player(&tab, identifiant1, 0, 0);
			}
		}
	}else{
		if (id1 == -1){
			add_player(&tab,identifiant1,0.5,0);
		}else{
			tab.tab_joueur[id1].score+=0.5;
		}
		if (id2 == -1){
			add_player(&tab,identifiant2,0.5,0);
		}else{
			tab.tab_joueur[id2].score+=0.5;
		}
	}
	return tab;
}

int search_id(int id, tableau* tab){
	int index = -1, i=0;
	for (i = 0 ; i < tab->total_players ; i++){
		if(tab->tab_joueur[i].identifiant == id){
			index = i;
		}
	}
	return index;
}

void free_memory_tableau(joueur* tab_joueur){   // On vide la mémoire du tableau de joueur
	free(tab_joueur);
}


	

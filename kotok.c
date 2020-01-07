/* EL OUAHABI NABIL, 000463021*/

#include "bibli.h"

int main(int argc, char* argv[]){
	
	if (argc < 3) {
		printf("Erreur : ajouter deux arguments\n");
		return 1;
	}
	
	FILE* finput;
	printf("Ouverture du fichier d'entrée: %s\n", argv[1]);
	if ((finput = fopen(argv[1], "r")) == NULL)
	{
		printf("Impossible d'ouvrir ce fichier\n");
		return 2;
	}
	printf("Fichier valide\n");
	
	FILE* foutput;
	if ((foutput = fopen(argv[2], "w")) == NULL){
		//printf("%s", argv[2]);
		printf("Impossible d'ouvrir ce fichier\n");
		return 2;
	}
	printf("Fichier valide\n");
	
	
	tableau tab = new_tableau();   // tab est un struct qui contient un tableau de joueur
	int identifiant1, identifiant2;
	float score;
	
	while ( (fscanf(finput, "%d %d %f",&identifiant1,&identifiant2,&score)) != EOF ){  // Ici on analyse tout le fichier input et on rempli le tableau de joueur dans tab correctement
		tab = update_tableau(identifiant1,identifiant2,score,tab);   // au lieu d'envoyer la valeur de l'adresse de tableau et de jouer avec son pointeur après, ici j'utilise une fonction qui me renverra directement le bon tableau
	}
	
	
	// le tableau de joueur est terminé, on remplit le document output avec les données
	int i;
	for (i = 0 ; i < tab.total_players ; i++){
		fprintf(foutput, "%d %f %d\n", tab.tab_joueur[i].identifiant,tab.tab_joueur[i].score,tab.tab_joueur[i].total_parties_remportees);	
	}
	
	free_memory_tableau(tab.tab_joueur);
	
	fclose(finput);
	fclose(foutput);

	return 0;
}

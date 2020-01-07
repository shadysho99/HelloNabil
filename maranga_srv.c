// EL OUAHABI NABIL 000463021
/*
 * =====================================================================================
 *
 *      Filename:  maranga_cli.c
 *
 *    	Description:  Un serveur pour le jeu d'anagramme, projet 3
 * 		Date : 20-12-2019
 *
 *      Author:  El Ouahabi Nabil
 * 	
 * 		Matricule : 000463021
 *  		 
 *
 * =====================================================================================
 */
 
#define	_DEFAULT_SOURCE			/* Utilise les implémentation POSIX des fonctions réseaux */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>

#include "personal_strings.h"
#include "anagram.h"

#define EXIT_CMD  "\\quit"


#define	TAILLE_BUFFER 1024	



typedef struct infos {
	int socket_client;
	char *message_to_send;
	char *message_recu;
	char* pseudo ;
	int score;
	int loop;
} infos_t;
	

infos_t *liste_infos;

int liste_infos_size = 5; // Nous pouvons avoir 5 joueurs au début
int nombre_joueur = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char** argument;

void *manage_client (void *data);

void send_message (infos_t *data);

void receive_message (infos_t *data);

int	main (int argc, char **argv)
{
	
	liste_infos = malloc(liste_infos_size*sizeof(infos_t));
	
	
	for(int i=0; i<liste_infos_size; i++){
		liste_infos[i].socket_client = 0;
		liste_infos[i].message_to_send = "";
		liste_infos[i].message_recu= "";
		liste_infos[i].pseudo="";
		liste_infos[i].score=0;
		liste_infos[i].loop = 1;
	}
	
	
    struct sockaddr_in adresse_serveur;

    int res;
    int port;

    int local_socket;

    pthread_t new_thread;
   
    memset (&adresse_serveur, 0, sizeof (struct sockaddr_in));

    if (argc != 4)
    {
        fprintf (stderr, "parrot_server port dico message_accueil\n");
        free(liste_infos);
        return EXIT_FAILURE;
    }

    errno = 0;

    port = strtol (argv[1], NULL, 10);         

    if (errno != 0 && port == 0)
    {
        perror ("Impossible de convertir le port <%s>");
        free(liste_infos);
        return EXIT_FAILURE;
    }

    adresse_serveur.sin_port = htons (port);    

    // Famille d'adresse
    adresse_serveur.sin_family = AF_INET;

    // Adresse IP: on écoute sur toutes les interfaces disponibles
    adresse_serveur.sin_addr.s_addr = htonl (INADDR_ANY);

    // XXX La structure d'adresse de socket relative pour l'écoute sur le serveur est maintenant prête à être utilisée
    
    

    // 1. Création socket
    local_socket = socket (PF_INET, SOCK_STREAM, 0);
    if (local_socket == -1)
    {
        fprintf (stderr, "Imposible d'ouvrir le socket\n");
        free(liste_infos);
        return EXIT_FAILURE;
    }

    // 2. Lien entre le descripteur de fichier et la structure (obligatoire pour un serveur)
    res = bind (local_socket, (struct sockaddr *) &adresse_serveur, sizeof (struct sockaddr_in));
    if (res == -1)
    {
        /* La fonction bind donne également une valeu à errno, on peut l'utiliser!
         *
         * perror ("Impossible de lier le socket et la structure d'adresse: ");
         * */
        fprintf (stderr, "Impossible de lier le socket et la structure d'adresse.\n");
        close (local_socket);                   /* On ferme le socket avant de quitter */
        free(liste_infos);
        return EXIT_FAILURE;
    }

    // XXX Ici le socket local est prêt à être utilisé!

    /* {{{ -------- Connexion -------- */

    // Du côté serveur, on initialise une écoute active
    res = listen (local_socket, 20);
    if (res == -1)
    {
        // perror ("Impossible de se mettre en écoute: ");
        fprintf (stderr, "Impossible de se mettre en écoute.\n");
        close (local_socket);
        free(liste_infos);
        return EXIT_FAILURE;
    }

   
    while (1)
    {
        int socket_client;
        struct sockaddr_in adresse_client;
        socklen_t taille_struct_addr_client;
        socket_client = accept (local_socket, (struct sockaddr *) &adresse_client, &taille_struct_addr_client);
        if (socket_client == -1)
        {
            fprintf (stderr, "Connexion impossible.\n");
			// on ne quitte pas le programme car ce n'est pas une erreur critique 
            continue;
        }
		
		argument = argv; // pour que argv soit global
        pthread_create (&new_thread, NULL, manage_client, &socket_client);
        
  
    }

	free(liste_infos);
	close (local_socket);
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */


void send_message (infos_t *data){

	uint32_t packet_size;
	
	int taille_envoyee;
	int longueur_chaine;
	int socket_client = data->socket_client;
	char *message = data->message_to_send;
    char *str_parser;
    int res;

	longueur_chaine = strlen (message);
	packet_size = htonl(longueur_chaine);
	
	res = send(socket_client, &packet_size, sizeof (uint32_t), 0);
	if (res == -1)
	{
		fprintf (stderr, "Impossible d'envoyer la taille du message.\n");
		close (socket_client);

	}
	
	for (str_parser = message, taille_envoyee = 0; taille_envoyee < longueur_chaine; )
	{
		res = send (socket_client, str_parser, longueur_chaine - taille_envoyee, 0);
		// Dans un premier temps, on gère les erreurs
		if (res == -1)
		{
			fprintf (stderr, "Impossible d'envoyer le message.\n");
			close (socket_client);
			continue;
		}

		taille_envoyee += res;
		str_parser += res;
	}

}

void receive_message (infos_t *data){
	
	uint32_t packet_size;
	int taille_recue;
	int longueur_chaine;
	int socket_client = data->socket_client;
    char *str_parser;
    char* str_buffer;
    str_buffer = malloc(1024*sizeof(char));
    int res;
    
    int connect = 1;
	
	
    res = recv (socket_client, &packet_size, sizeof (uint32_t), 0);
	if (res == -1)
	{
		fprintf (stderr, "Erreur à la réception de la taille.\n");
		close (socket_client);
	}

	longueur_chaine = ntohl (packet_size);

	if (longueur_chaine > TAILLE_BUFFER - 1){
		str_buffer = realloc(str_buffer, (longueur_chaine+1)*sizeof(char));	
	}

	for (str_parser = str_buffer, taille_recue = 0; taille_recue < longueur_chaine; )
	{
		res = recv (socket_client, str_parser, longueur_chaine - taille_recue, 0);
		// Dans un premier temps, on gère les erreurs
		if (res == -1)
		{
			fprintf (stderr, "Impossible de recevoir le message.\n");
			close (socket_client);
			data->loop = 0;
			break;
		}
		else if (res == 0)
		{
			printf("Fermeture socket côté client.\n");
			close (data->socket_client);
			data->loop = 0;
			connect = 0;
			break;
		}

		taille_recue += res;
		str_parser += res;
	}
	
	if (connect == 1){
		str_buffer[taille_recue] = '\0';

		if (taille_recue != longueur_chaine)
		{
			fprintf (stderr, "Réception partielle: %s\n", str_buffer);
		}
		else
		{
			printf ("%s\n", str_buffer);
			if(!strcmp(data->pseudo, "")){
				data->pseudo = str_buffer;
			}
		
			data->message_recu = str_buffer;
		}
	}
	
	free(str_buffer); // pas oublier de free str_buffer qui est créé sur le heap dans la fonction
}


void *manage_client (void *data)
{	
	infos_t infos_client;
	int socket_client = *((int *) data);
	
	infos_client.socket_client = socket_client;
	infos_client.message_to_send = "";
	infos_client.message_recu= "";
	infos_client.pseudo="";
	infos_client.score = 0;
	infos_client.loop = 1;
	
	receive_message(&infos_client);

	int trouve =0;
	int indice = 0;
	
	pthread_mutex_lock(&mutex);

	for (int i=0; i<liste_infos_size; i++){
		if ( !strcmp(liste_infos[i].pseudo, infos_client.pseudo)){
			indice = i;
			infos_client.score = liste_infos[i].score;
			trouve = 1;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);
	
	pthread_mutex_lock(&mutex);
	if (trouve == 0){
		nombre_joueur++;
		if (nombre_joueur > liste_infos_size){
			liste_infos = realloc(liste_infos, nombre_joueur*sizeof(infos_t));
			liste_infos_size++;
		}
		if (liste_infos == NULL){
			printf("Erreur ré-allocation de mémoire\n");
			exit(EXIT_FAILURE);
		}
		indice = nombre_joueur-1;
		liste_infos[indice].pseudo = infos_client.pseudo;
		liste_infos[indice].score = 0;
		liste_infos[indice].loop = 1;
		liste_infos[indice].socket_client = 0;
		liste_infos[indice].message_recu = "";
		liste_infos[indice].message_to_send = ""; 
	}	

	pthread_mutex_unlock(&mutex);

    int ret_val;
    dict_t dico;
    anagram_t anagram;
    
    pstring_t user_entry = empty_pstring();
    
    ret_val = init_dico(&dico, argument[2]);
    
    init_anagram(&anagram);
 

	// Maintenant on peut envoyer le message de Bienvenue
	
	char* welcome = argument[3];
	
	infos_client.message_to_send = welcome;

	
	send_message(&infos_client);
	

	// ON COMMENCE LE JEU
	while (infos_client.loop == 1)
    {
        // On génère un nouvel anagram
        ret_val = new_anagram (&anagram, &dico);
        if (ret_val == -1)
        {
            perror ("Création d'un nouvel anagramme: ");
            free_pstring(&user_entry);
            free_anagram(&anagram);
            free_dico(&dico);
			
		return (void*)EXIT_FAILURE;
        }
        

        char *mot = anagram.mot_courant.str;
        infos_client.message_to_send = mot;
        
        
		send_message(&infos_client);

        // Boucle secondaire (on itère sur l'anagramme courant tant que l'utilisateur ne l'a pas
        // trouvé)
        do
        {
            receive_message(&infos_client);
            if (infos_client.loop == 0){
				
				pthread_mutex_unlock(&mutex);
				liste_infos[indice].score = infos_client.score;
				pthread_mutex_unlock(&mutex);
			
				return EXIT_SUCCESS;
			}
            
        } while (!is_solution (&anagram, infos_client.message_recu )); // teste si l'entrée est une solution 

		
		int gain = strlen(mot);
		infos_client.score += gain;
		
		
		char str_gain[100];
		sprintf(str_gain, "%d", gain);
			
		
		char str_score_total[100];
		sprintf(str_score_total, "%d", infos_client.score);
		
		char bravo[] = "Bravo, vous avez gagné ";
		char bravo_milieu[] = " points (total: ";
		char bravo_fin[] = ")";
		
		strcat(bravo, str_gain);
		strcat(bravo, bravo_milieu);
		strcat(bravo, str_score_total);	
		strcat(bravo, bravo_fin);

		infos_client.message_to_send = bravo;
		send_message(&infos_client);


    }

	return EXIT_SUCCESS;
}


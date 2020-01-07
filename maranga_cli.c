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




#define	_DEFAULT_SOURCE			    /* Utilise les implémentation POSIX des fonctions réseaux */

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

#define max(x,y) ( x < y ? y : x )              /* Instruction ternaire pour déterminer le maximum entre deux nombres */

#define EXIT_CMD  "!quit"

 
int main (int argc, char **argv)
{
	int TAILLE_BUFFER = 1024; // to modify it later
	
    struct sockaddr_in adresse_client;
    struct sockaddr_in adresse_serveur;

    int res;
    int port;

    int local_socket;
    uint32_t packet_size;
    int longueur_chaine;

    // Cette variable sert dans le cas où l'envoi (utilisation de send) nécessite plusieurs appels
    char *str_parser;
    int taille_envoyee = 0;

    // Les vriables nécessaires pour la réception d'un message
    char *str_buffer = malloc(TAILLE_BUFFER*sizeof(char));  // on le crée sur le heap pour pouvoir realloc si besoin
    int taille_recue = 0;

    // VARIABLE DE MULTIPLEXAGE
    fd_set lecture;

   
    memset (&adresse_client, 0, sizeof (struct sockaddr_in));
    memset (&adresse_serveur, 0, sizeof (struct sockaddr_in));

    if (argc != 4)
    {
		printf("%i\n", argc);
        fprintf (stderr, "maranga_cli adr_ip port pseudo\n");
        free(str_buffer);
        return EXIT_FAILURE;
    }

    // Adresse ip
    res = inet_aton (argv[1], &adresse_serveur.sin_addr);
    if (!res)                                   // inet_aton retourne 0 en cas d'erreur
    {
        fprintf (stderr, "Impossible de convertir l'adresse <%s>\n", argv[1]);
        free(str_buffer);
        return EXIT_FAILURE;
    }

    errno = 0;

    port = strtol (argv[2], NULL, 10);                 /* Conversion en int */

    // On peut ensuite tester si tout s'est bien passé (un coup d'oeil à la page de manuel de strtol
    // nous permet de connaître son fonctionnement en cas d'erreur
    if (errno != 0 && port == 0)
    {
        perror ("Impossible de convertir le port :");
        free(str_buffer);
        return EXIT_FAILURE;
    }

    adresse_serveur.sin_port = htons (port);     /* Conversion représentation mémoire */

    // Famille d'adresse
    adresse_serveur.sin_family = AF_INET;


    // 1. Création socket
    local_socket = socket (PF_INET, SOCK_STREAM, 0);
    if (local_socket == -1)
    {
        fprintf (stderr, "Imposible d'ouvrir le socket\n");
        free(str_buffer);
        return EXIT_FAILURE;
    }

    // 2. Préparation de la structure d'adresse de socket locale
    adresse_client.sin_family = AF_INET;
    adresse_client.sin_port = htons (0);            // On laisse le choix du port à l'OS
    adresse_client.sin_addr.s_addr = htonl (INADDR_ANY);

    // 3. Lien entre le descripteur de fichier et la structure (optionnel pour le client mais
    // recommandé)
    res = bind (local_socket, (struct sockaddr *) &adresse_client, sizeof (struct sockaddr_in));
    if (res == -1)
    {
        fprintf (stderr, "Impossible de lier le socket et la structure d'adresse.\n");
        close (local_socket);                   /* On ferme le socket avant de quitter */
        free(str_buffer);
        return EXIT_FAILURE;
    }

	// CONNEXION
    res =  connect (local_socket, (struct sockaddr *) &adresse_serveur, sizeof (struct sockaddr_in));
    if (res == -1)
    {
        // perror ("Impossible de se connecter au serveur: ");
        fprintf (stderr, "Impossible de se connecter au serveur.\n");
        close (local_socket);
        free(str_buffer);
        return EXIT_FAILURE;
    }
	
	// COMMUNICATION 
	

	longueur_chaine = strlen (argv[3]);
	packet_size = htonl (longueur_chaine);
	
	res = send (local_socket, &packet_size, sizeof (uint32_t), 0);
	if (res == -1)
	{
		fprintf (stderr, "Impossible d'envoyer la taille du message.\n");
		close (local_socket);
		free(str_buffer);
		return EXIT_FAILURE;
	}
	

	for (str_parser = argv[3], taille_envoyee = 0; taille_envoyee < longueur_chaine; )
	{
		res = send (local_socket, str_parser, longueur_chaine - taille_envoyee, 0);
		if (res == -1)
		{
			// perror ("Impossible d'envoyer le message: ");
			fprintf (stderr, "Impossible d'envoyer le message.\n");
			close (local_socket);
			free(str_buffer);
			return EXIT_FAILURE;
		}

		taille_envoyee += res;                  /* On ajoute le nombre d'octets de l'envoi courant au nombre d'octets envoyés */
		str_parser += res;                      /* De la même manière on décale str_parser de manière à reprendre là où send s'est arrêté */
	}
	
	int exit = 0;
	
    while (!exit)
    {
        FD_ZERO (&lecture);  // fdset lecture; plus haut
        FD_SET (STDIN_FILENO, &lecture);            /*on ajoute lecture à l'entrée standard */
        FD_SET (local_socket, &lecture);            /* le socket connecté au serveur */

        res = select (1 + max (STDIN_FILENO, local_socket), &lecture, NULL, NULL, NULL);
        if (res == -1)
        {
            perror ("Problème de multiplexage: ");
            close (local_socket);
            free(str_buffer);
            return EXIT_FAILURE;
        }
        

		// FD_ISSET --> on teste si fd appartient à l'ensemble
        if (FD_ISSET (STDIN_FILENO, &lecture))  /* Si c'est le clavier, on lit le message et on l'envoie */
        {
			
            fgets (str_buffer, TAILLE_BUFFER, stdin);

            // On convertit l'entier en représentation réseau
            longueur_chaine = strlen (str_buffer);

            
            //On enlève le \n au besoin
            if (str_buffer[longueur_chaine - 1] == '\n')
            {
                str_buffer[longueur_chaine - 1] = '\0';
                longueur_chaine--;
            }
              
             if (!strcmp (EXIT_CMD, str_buffer))
            {
                fprintf (stdout, "\nExiting\n");
                close (local_socket);
       
                exit = 1;
                continue;
            }

            packet_size = htonl (longueur_chaine);

            // Et on l'envoie au serveur
            res = send (local_socket, &packet_size, sizeof (uint32_t), 0);
            
            if (res == -1)
            {
                // perror ("Impossible d'envoyer la taille du message: ");
                fprintf (stderr, "Impossible d'envoyer la taille du message.\n");
                close (local_socket);
                free(str_buffer);
                return EXIT_FAILURE;
            }

            for (str_parser = str_buffer, taille_envoyee = 0; taille_envoyee < longueur_chaine; )
            {
                res = send (local_socket, str_parser, longueur_chaine - taille_envoyee, 0);
                // Dans un premier temps, on gère les erreurs
                if (res == -1)
                {
                    fprintf (stderr, "Impossible d'envoyer le message.\n");
                    close (local_socket);
                    free(str_buffer);
                    return EXIT_FAILURE;
                }

                taille_envoyee += res;                  /* On ajoute le nombre d'octets de l'envoi courant au nombre d'octets envoyés */
                str_parser += res;                      /* De la même manière on décale str_parser de manière à reprendre là où send s'est arrêté */
            }
        }
        
        if (FD_ISSET (local_socket, &lecture))  /* Si c'est le socket, on lit et on affiche le message recu */
        {
            // On attend deux messages, un contenant la taille de la chaîne envoyée, l'autre la chaîne
            // en question
            res = recv (local_socket, &packet_size, sizeof (uint32_t), 0);
            if (res == -1)
            {
                fprintf (stderr, "Erreur à la réception de la taille.\n");
                close (local_socket);
                free(str_buffer);
                return EXIT_FAILURE;
            }
            

            // On convertit l'entier en représentation machine
            longueur_chaine = ntohl (packet_size);
            
            if (longueur_chaine > TAILLE_BUFFER - 1){
				str_buffer = realloc(str_buffer, (longueur_chaine+1)*sizeof(char));	
				TAILLE_BUFFER = longueur_chaine;
			}

            // On peut lire le message
            for (str_parser = str_buffer, taille_recue = 0; taille_recue < longueur_chaine; )
            {
                res = recv (local_socket, str_parser, longueur_chaine - taille_recue, 0);
                // Dans un premier temps, on gère les erreurs
                if (res == -1)
                {
                    fprintf (stderr, "Impossible de recevoir le message.\n");
                    // S'il y a une erreur, on se contente d'une réception partielle et on sort de la boucle
                    break;
                }
                else if (res == 0)
                {
                    printf ("Fermeture socket côté serveur.\n");
					close (local_socket);
					free(str_buffer);  // on n'oublie pas de free notre seul pointeur créé sur le heap
					return EXIT_SUCCESS; // on met fin au main, on arrête  le code
                }

                taille_recue += res;
                str_parser += res;
            }

            str_buffer[taille_recue] = '\0';

            // En cas de réception partielle, on informe l'utilisateur
            if (taille_recue != longueur_chaine)
            {
                fprintf (stderr, "Réception partielle: %s\n", str_buffer);
            }
            // Sinon on affiche le message recu
            else
            {
                printf ("Message recu: %s\n", str_buffer);
            }
        }
    }
    
    
	free(str_buffer);
    close (local_socket);
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

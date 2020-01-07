/*
 * =====================================================================================
 *
 *       Filename:  exo1.c
 *
 *    Description:  Un exemple simple de création de threads et d'accès concurrentiel
 *
 *        Version:  1.0
 *        Created:  20/11/2019 22:50:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

int variable_globale  = 4;

    void *
double_val (void *data)
{
    int i = 0;
    int res;
    pthread_mutex_t mutex = *((pthread_mutex_t *)data);

    res = pthread_mutex_lock (&mutex);
    if (res != 0)
    {
        perror ("Impossible de verrouiller la zone mémoire: ");
        pthread_exit (NULL);
    }

    for (i = 0; i < 26; i++)
    {
        variable_globale *= 2;
    }
    printf ("%d\n", variable_globale);

    pthread_mutex_unlock (&mutex);

    pthread_exit (NULL);
}

    void *
plus_cinq (void *data)
{
    int i = 0;
    int res;
    pthread_mutex_t mutex = *((pthread_mutex_t *)data);

    res = pthread_mutex_lock (&mutex);
    if (res != 0)
    {
        perror ("Impossible de verrouiller la zone mémoire: ");
        pthread_exit (NULL);
    }

    for (i = 0; i < 170; i++)
    {
        variable_globale += 5;
    }
    printf ("%d\n", variable_globale);

    pthread_mutex_unlock (&mutex);

    pthread_exit (NULL);
}

    void *
moins_trois (void *data)
{
    int i = 0;
    int res;
    pthread_mutex_t mutex = *((pthread_mutex_t *)data);

    res = pthread_mutex_lock (&mutex);
    if (res != 0)
    {
        perror ("Impossible de verrouiller la zone mémoire: ");
        pthread_exit (NULL);
    }

    for (i = 0; i < 168; i++)
    {
        variable_globale -= 5;
    }
    printf ("%d\n", variable_globale);

    pthread_mutex_unlock (&mutex);

    pthread_exit (NULL);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  main function
 * =====================================================================================
 */
    int
main (int argc, char **argv)
{
    int ret_val;
    pthread_t thread_un, thread_deux, thread_trois;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    ret_val = pthread_create (&thread_un, NULL, double_val, &mutex);
    if (ret_val != 0)
    {
        /*
         * Attention! Les fonctions pthread ne modifient pas la valeur de errno, l'utilisation de
         * perror ne peut se faire telle quelle. Cependant, la valeur de retour de ces fonctions est
         * en réalité le code d'erreur errno en lui même. 
         *
         * Ce dernier peut être exploité via la fonction strerror (disponible dans string.h) pour
         * afficher le message d'erreur qu'aurait affiché perror.
         *
         * L'appel suivant est équivalent à:
         *
         * errno = ret_val;
         * perror ("Création du thread: ");
         */
        fprintf (stderr, "Création du thread: %s\n", strerror (ret_val));
        return EXIT_FAILURE;
    }

    ret_val = pthread_create (&thread_deux, NULL, plus_cinq, &mutex);
    if (ret_val != 0)
    {
        /*
         * Attention! Les fonctions pthread ne modifient pas la valeur de errno, l'utilisation de
         * perror ne peut se faire telle quelle. Cependant, la valeur de retour de ces fonctions est
         * en réalité le code d'erreur errno en lui même. 
         *
         * Ce dernier peut être exploité via la fonction strerror (disponible dans string.h) pour
         * afficher le message d'erreur qu'aurait affiché perror.
         *
         * L'appel suivant est équivalent à:
         *
         * errno = ret_val;
         * perror ("Création du thread: ");
         */
        fprintf (stderr, "Création du thread: %s\n", strerror (ret_val));
        return EXIT_FAILURE;
    }

    ret_val = pthread_create (&thread_trois, NULL, moins_trois, &mutex);
    if (ret_val != 0)
    {
        /*
         * Attention! Les fonctions pthread ne modifient pas la valeur de errno, l'utilisation de
         * perror ne peut se faire telle quelle. Cependant, la valeur de retour de ces fonctions est
         * en réalité le code d'erreur errno en lui même. 
         *
         * Ce dernier peut être exploité via la fonction strerror (disponible dans string.h) pour
         * afficher le message d'erreur qu'aurait affiché perror.
         *
         * L'appel suivant est équivalent à:
         *
         * errno = ret_val;
         * perror ("Création du thread: ");
         */
        fprintf (stderr, "Création du thread: %s\n", strerror (ret_val));
        return EXIT_FAILURE;
    }

    pthread_join (thread_un, NULL);
    pthread_join (thread_deux, NULL);
    pthread_join (thread_trois, NULL);

    ret_val = pthread_mutex_lock (&mutex);
    if (ret_val != 0)
    {
        perror ("Main: mutex lock erreur ");
        return EXIT_FAILURE;
    }

    printf ("Final: %d\n", variable_globale);
    
    pthread_mutex_unlock (&mutex);

    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

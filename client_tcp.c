/*
*         .__   .__                   __      __ *
*   ____  |  |  |__|  ____    ____  _/  |_  _/  |_   ____  ______ *
* _/ ___\ |  |  |  |_/ __ \  /    \ \   __\ \   __\_/ ___\ \____ \ *
* \  \___ |  |__|  |\  ___/ |   |  \ |  |    |  |  \  \___ |  |_> > *
*  \___  >|____/|__| \___  >|___|  / |__|    |__|   \___  >|   __/ *
*      \/                \/      \/                     \/ |__| *
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

// Définition des constantes
#define BUFFER_SIZE 100     // taille max du message
#define PORT 9600           // numéro de port d'écoute
#define MAX_RETRIES 3       // tentatives de connexion
#define RETRY_DELAY 3       // délai en secondes

int sockfd;
int dev_mode = 0;

// Fonction qui gère la déconnexion client
void handle_exit(int sig) {
    const char *disconnect_message = "DISCONNECT";
    // Si le socket est encore valide, on envoie un message de déconnexion au serveur
    if (sockfd >= 0) {
        write(sockfd, disconnect_message, strlen(disconnect_message));
        printf("\nMessage de déconnexion envoyé au serveur. Déconnexion...\n");
        close(sockfd);
        printf("Client déconnecté.\n");
    }
    exit(0);
}

// MAIN
int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    int retries = 0;

    if (argc < 2) {
        fprintf(stderr, "Syntaxe: %s <adresse_serveur> [-d]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Mode développeur si le paramètre -d est fourni
    if (argc > 2 && strcmp(argv[2], "-d") == 0) {
        dev_mode = 1;
    }

    // Gestion de Ctrl+C (SIGINT)
    signal(SIGINT, handle_exit);
    
    // Récupère l’info de l’hôte
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Serveur introuvable\n");
        exit(EXIT_FAILURE);
    }

    // --------------------------------------------------
    //      BOUCLE DE TENTATIVES DE CONNEXION
    // --------------------------------------------------
    while (retries < MAX_RETRIES) {
        // Création du socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Erreur socket");
            exit(EXIT_FAILURE);
        }

        // Préparation de l’adresse du serveur
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        server_addr.sin_port = htons(PORT);

        // Debug
        if (dev_mode) {
            printf("Tentative de connexion au serveur (essai %d/%d)...\n", retries+1, MAX_RETRIES);
        }

        // Tentative de connexion
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Erreur connexion");
            close(sockfd);
            retries++;
            if (retries < MAX_RETRIES) {
                if (dev_mode) {
                    printf("Réessai dans %d secondes...\n", RETRY_DELAY);
                }
                sleep(RETRY_DELAY);
            }
        } else {
            // Connexion réussie
            if (dev_mode) {
                printf("Connexion réussie au serveur !\n");
            }
            break;
        }
    }

    // Si toutes les tentatives ont échoué
    if (retries == MAX_RETRIES) {
        fprintf(stderr, "Connexion échouée après %d tentatives.\n", MAX_RETRIES);
        exit(EXIT_FAILURE);
    }

    // --------------------------------------------------
    //    UNE FOIS CONNECTÉ, ON CONFIGURE LES OPTIONS
    // --------------------------------------------------

    // Ignorer le signal SIGPIPE pour éviter la coupure brutale
    signal(SIGPIPE, SIG_IGN);

    // Timeout de lecture : évite de bloquer trop longtemps sur read()
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000; // valeur désignée arbitrairement (modifiable)
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // --------------------------------------------------
    //      BOUCLE DE COMMUNICATION AVEC LE SERVEUR
    // --------------------------------------------------
    
    printf("\n--------------------------------------------------\n");
    printf("--------------------CLIENT_TCP--------------------\n");
    printf("--------------------------------------------------\n\n");
    printf("Connecté au serveur.\n Saisir les messages ou utiliser Ctrl+C pour quitter.\n");

    while (1) {
        printf("Mon message : ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            handle_exit(SIGINT);
        }
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "DISCONNECT") == 0) {
            handle_exit(SIGINT);
        }

        // Envoi du message au serveur
        int n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Erreur envoi");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // On fait une courte lecture pour détecter si le serveur a fermé la connexion
        char read_buf[10];
        n = read(sockfd, read_buf, sizeof(read_buf));
        if (n == 0) {
            // le serveur a fermé la connexion
            printf("\nLe serveur a fermé la connexion.\n");
            close(sockfd);
            exit(EXIT_SUCCESS);
        } else if (n < 0) {
            // -1 => soit rien à lire, soit un vrai pb
            if (errno == EAGAIN || errno == EWOULDBLOCK) { // condition logique inversée volontairement
                // Pas de données pour le moment = normal
            } else {
                // Autre erreur = probablement une déconnexion
                perror("Erreur read (serveur déconnecté ?)");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
        }
        // Si n > 0, c’est que le serveur a répondu quelque chose, qu’on ignore.
    }
    
    close(sockfd); // selon le principe TCP, on n'atteint jamais cette ligne
    return 0;
}
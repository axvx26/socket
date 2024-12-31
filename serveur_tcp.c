#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Ajouté pour inet_ntoa ou inet_ntop
#include <signal.h>
#include <time.h>
#include <pthread.h>    // Ajouté pour les threads

#define BUFFER_SIZE 100
#define PORT 9600

int dev_mode = 0;
int sockfd = -1;        // Rendre sockfd global pour y accéder dans le gestionnaire de signal

/**
 * @brief Gestionnaire pour le signal SIGINT (Ctrl+C).
 */
void handle_signal(int sig) {
    if (sockfd >= 0) {
        printf("\nArrêt du serveur suite à Ctrl+C.\n");
        close(sockfd);
    }
    exit(EXIT_SUCCESS);
}

/**
 * @brief Thread qui surveille l'entrée standard du serveur.
 *        Si l'administrateur tape SHUTDOWN, on arrête le serveur.
 */

void* server_console_thread(void* arg) {
    char cmd_buffer[128];

    while (1) {
        // Lecture depuis stdin (terminal)
        if (fgets(cmd_buffer, sizeof(cmd_buffer), stdin) != NULL) {
            cmd_buffer[strcspn(cmd_buffer, "\n")] = '\0';

            // Commande CLEAR
            if (strcmp(cmd_buffer, "CLEAR") == 0) {
                system("clear");  // Sur Linux/macOS
                //system("cls");  // Sur Windows
            }
            // Commande SHUTDOWN
            else if (strcmp(cmd_buffer, "SHUTDOWN") == 0) {
                printf("Arrêt du serveur...\n");
                if (sockfd >= 0) {
                    close(sockfd);
                }
                exit(0);
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char ip_str[INET_ADDRSTRLEN]; // Pour inet_ntop

    // Gestion du mode développeur
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        dev_mode = 1;
    }

    // Installation du gestionnaire de signal pour Ctrl+C
    signal(SIGINT, handle_signal);

    // Création du socket serveur
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);

    // Association (bind) de l'adresse
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // header
    printf("\n--------------------------------------------------\n");
    printf("-------------------SERVEUR_TCP--------------------\n");
    printf("--------------------------------------------------\n\n");

    // Mise en écoute
    listen(sockfd, 5);
    printf("Serveur en attente sur le port %d...\n", PORT);

    // Lancement d'un thread pour surveiller la console (stdin)
    pthread_t tid;
    if (pthread_create(&tid, NULL, server_console_thread, NULL) != 0) {
        perror("Erreur création thread console");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    // On peut détacher le thread pour qu'il se nettoie lui-même à la fin
    pthread_detach(tid);

    // Boucle principale du serveur
    while (1) {
        new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (new_sock < 0) {
            perror("Erreur accept");
            continue; // on continue le while
        }

        // Récupération de l'IP client sous forme lisible
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
        if (dev_mode) {
            printf("Connexion acceptée depuis %s:%d\n", ip_str, ntohs(client_addr.sin_port));
        } else {
            printf("Une connexion a été acceptée.\n");
        }

        // Boucle de communication avec le client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int n = read(new_sock, buffer, BUFFER_SIZE - 1);
            if (n <= 0) {
                if (dev_mode){
                    printf("Déconnexion du client %s:%d\n", ip_str, ntohs(client_addr.sin_port));
                } else {
                    printf("Un client a été déconnecté.\n");
                }
                close(new_sock);
                break; // on sort de la boucle client
            }

            buffer[n] = '\0';

            if (dev_mode){
                printf("Message reçu du client %s:%d : %s\n", ip_str, ntohs(client_addr.sin_port), buffer);
                printf("Taille du message reçu : %d octets.\n", n);
            } else {
                printf("%s\n", buffer);
            }
        }
    }

    // Fermeture du socket serveur (en théorie on n'atteint jamais ici)
    close(sockfd);
    return 0;
}
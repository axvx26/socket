/*
*                                          __ *
*   ______  ____  _______ ___  __  ____  _/  |_   ____  ______ *
*  /  ___/_/ __ \ \_  __ \\  \/ /_/ __ \ \   __\_/ ___\ \____ \ *
*  \___ \ \  ___/  |  | \/ \   / \  ___/  |  |  \  \___ |  |_> > *
* /____  > \___  > |__|     \_/   \___  > |__|   \___  >|   __/ *
*      \/      \/                     \/             \/ |__| *
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

// Définition des constantes
#define BUFFER_SIZE 100 // taille max du message
#define PORT 9600       // numéro de port d'écoute

int dev_mode = 0;
int sockfd = -1;

// Variable pour indiquer qu'on veut arrêter le serveur
volatile sig_atomic_t run_server = 1;

void handle_signal(int sig) {
    if (sockfd >= 0) {
        printf("\nArrêt du serveur suite à Ctrl+C.\n");
        close(sockfd);
    }
    exit(EXIT_SUCCESS);
}

void* server_console_thread(void* arg) {
    char cmd_buffer[128];

    while (1) {
        if (fgets(cmd_buffer, sizeof(cmd_buffer), stdin) != NULL) {
            cmd_buffer[strcspn(cmd_buffer, "\n")] = '\0';
            if (strcmp(cmd_buffer, "CLEAR") == 0) {
                system("clear"); 
            }
            else if (strcmp(cmd_buffer, "SHUTDOWN") == 0) {
                printf("Arrêt du serveur...\n");
                run_server = 0;         // on signale l'arrêt
                if (sockfd >= 0) {
                    close(sockfd);      // on ferme le socket
                }
                exit(0);               // on quitte le programme
            }
        }
    }
    return NULL;
}

// MAIN
int main(int argc, char *argv[]) {
    int new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char ip_str[INET_ADDRSTRLEN];

    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        dev_mode = 1;
    }

    signal(SIGINT, handle_signal);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("\n--------------------------------------------------\n");
    printf("-------------------SERVEUR_TCP--------------------\n");
    printf("--------------------------------------------------\n\n");

    listen(sockfd, 5);
    printf("Serveur en attente sur le port %d...\n", PORT);

    // Thread console
    pthread_t tid;
    if (pthread_create(&tid, NULL, server_console_thread, NULL) != 0) {
        perror("Erreur création thread console");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    pthread_detach(tid);

    // Boucle principale d'accept
    while (run_server) {
        new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (new_sock < 0) {
            // Si on a explicitement fermé le socket, on quitte proprement
            if (!run_server) {
                break;
            }
            perror("Erreur accept");
            continue; 
        }

        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
        if (dev_mode) {
            printf("Connexion acceptée depuis %s:%d\n", ip_str, ntohs(client_addr.sin_port));
        } else {
            printf("Une connexion a été acceptée.\n");
        }

        // Boucle de communication
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int n = read(new_sock, buffer, BUFFER_SIZE - 1);
            if (n <= 0) {
                if (dev_mode) {
                    printf("Déconnexion du client %s:%d\n", ip_str, ntohs(client_addr.sin_port));
                } else {
                    printf("Un client a été déconnecté.\n");
                }
                close(new_sock);
                break;
            }

            buffer[n] = '\0';
            if (dev_mode) {
                printf("Message reçu du client %s:%d : %s\n",
                       ip_str, ntohs(client_addr.sin_port), buffer);
                printf("Taille du message reçu : %d octets.\n", n);
            } else {
                printf("%s\n", buffer);
            }
        }
    }

    // Fin, on ferme le socket serveur si encore ouvert
    if (sockfd >= 0) {
        close(sockfd);
    }
    return 0;
}
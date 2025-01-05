/*
*                                                                   .___ *
*   ______  ____  _______ ___  __  ____   __ __ _______   __ __   __| _/______ *
*  /  ___/_/ __ \ \_  __ \\  \/ /_/ __ \ |  |  \\_  __ \ |  |  \ / __ | \____ \ *
*  \___ \ \  ___/  |  | \/ \   / \  ___/ |  |  / |  | \/ |  |  // /_/ | |  |_> > *
* /____  > \___  > |__|     \_/   \___  >|____/  |__|    |____/ \____ | |   __/ *
*      \/      \/                     \/                             \/ |__| *
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>

// Définition des constantes
#define BUFFER_SIZE 100  // taille max du message
#define PORT 9600        // port d'écoute du serveur


// MAIN
int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Création du socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse locale
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Liaison du socket à l'adresse locale
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors du bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP en attente de messages sur le port %d...\n", PORT);

    // Boucle principale
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("Erreur lors de la réception");
            continue;
        }

        buffer[n] = '\0';
        printf("Message reçu : %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
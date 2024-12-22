#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100  // Taille maximale du message à recevoir
#define PORT 9600        // Port d'écoute du serveur

int main() {
    /*
    * Variables du serveur
    */
    int sockfd;                                  // Descripteur du socket
    struct sockaddr_in server_addr, client_addr; // Adresses du serveur et du client
    socklen_t client_len = sizeof(client_addr);  // Taille de la structure client_addr
    char buffer[BUFFER_SIZE];                    // Tampon pour stocker les messages reçus

    /*
    * Création du socket
    */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  // Création d'un socket UDP
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    /*
    * Configuration de l'adresse locale
    */
    memset(&server_addr, 0, sizeof(server_addr)); // Initialisation
    server_addr.sin_family = AF_INET;             // Famille d'adresses : IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;     // Écouter sur toutes les interfaces locales
    server_addr.sin_port = htons(PORT);           // Port d'écoute

    /*
    * Liaison du socket à l'adresse locale
    */
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors du bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP en attente de messages sur le port %d...\n", PORT);

    /*
    * Boucle générale (infinie)
    */
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
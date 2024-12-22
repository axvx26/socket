#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100 // Taille maximale du message à recevoir
#define PORT 9600       // Port d'écoute du serveur

int main(int argc, char *argv[]) {
    int sockfd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    struct hostent *host;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Vérification des arguments
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <nom_ou_IP_du_serveur>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Création du socket principal
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Résolution du nom d'hôte
    host = gethostbyname(argv[1]);
    if (host == NULL) {
        fprintf(stderr, "Erreur : nom d'hôte non valide\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse locale du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, host->h_addr, host->h_length);
    server_addr.sin_port = htons(PORT);

    // Liaison du socket à l'adresse locale
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors du bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Mettre le socket en écoute
    if (listen(sockfd, 5) < 0) {
        perror("Erreur lors de l'écoute");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur TCP en attente de connexions sur %s:%d...\n", argv[1], PORT);

    // Boucle générale (infinie)
    while (1) {
        new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (new_sock < 0) {
            perror("Erreur lors de l'acceptation");
            continue;
        }

        printf("Connexion acceptée avec un client.\n");

        // Boucle pour lire plusieurs messages du client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int n = read(new_sock, buffer, BUFFER_SIZE - 1);
            if (n <= 0) {
                if (n < 0) {
                    perror("Erreur lors de la lecture");
                } else {
                    printf("Connexion terminée par le client.\n");
                }
                break;
            }

            buffer[n] = '\0';
            printf("Message reçu : %s\n", buffer);

        }
        printf("Client a demandé la fermeture de la connexion.\n");

        close(new_sock);
    }

    close(sockfd);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 100 // Taille maximale du message à recevoir
#define PORT 9600       // Port d'écoute du serveur

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage : %s <nom_ou_IP_du_serveur>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Création du socket client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Résolution du nom d'hôte
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Erreur : serveur introuvable\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // Connexion au serveur
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors de la connexion");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connexion établie avec le serveur. Entrez vos messages :\n");

    // Boucle pour envoyer plusieurs messages
    while (1) {
        printf("Entrez un message : ");
        fgets(buffer, BUFFER_SIZE, stdin);      // Lire une ligne depuis l'entrée standard
        buffer[strcspn(buffer, "\n")] = '\0';   // Supprimer le saut de ligne

        // Si l'utilisateur entre "exit", fin
        if (strcmp(buffer, "exit") == 0) {
            printf("Fermeture de la connexion.\n");
            break;
        }

        // Envoyer le message au serveur
        int n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Erreur lors de l'envoi");
        }
    }

    close(sockfd);
    return 0;
}
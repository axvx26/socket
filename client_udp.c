#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100 // Taille maximale du message à envoyer
#define PORT 9600       // Port d'écoute du serveur

int main(int argc, char *argv[]) {
    /*
    * Variables du client
    */
    int sockfd;                         // Descripteur de socket
    struct sockaddr_in server_addr;     // Structure d'adresse locales du serveur
    struct hostent *server;             // Structure pour la résolution de l'adresse IP du serveur
    char buffer[BUFFER_SIZE];           // Tampon pour les messages à envoyer

    // Vérification des arguments en ligne de commande
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <nom_ou_IP_serveur>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*
    * Code du client
    */
    // Ouvrir le socket du client
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Récupérer l’adresse IP du serveur à partir de son nom donné en ligne de commande
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Erreur : serveur introuvable\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Remplir la structure d’adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));                           // Initialisation
    server_addr.sin_family = AF_INET;                                       // Famille d'adresse (IPv4)
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length); // Copier l'adresse IP du serveur
    server_addr.sin_port = htons(PORT);                                     // Port du serveur, converti en format réseau

    // Boucle générale du serveur (infinie)
    while (1) {
        printf("Entrez un message : ");
        fgets(buffer, BUFFER_SIZE, stdin); // Lire un message depuis l'entrée standard
        buffer[strcspn(buffer, "\n")] = '\0';

        // Envoyer la chaîne lue au serveur
        int n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (n < 0) {
            perror("Erreur lors de l'envoi");
        }
    }

    // Fermer le socket
    close(sockfd);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100  // Taille maximale du message à recevoir
#define PORT 9600        // Port d'écoute du serveur

void run_tcpdump_in_background() {
    pid_t pid = fork();
    if (pid == 0) {
        // Processus enfant : exécuter tcpdump
        char *cmd[] = {"sudo", "tcpdump", "-i", "lo0", "port", "9600", NULL};
        execvp(cmd[0], cmd);

        // Si execvp échoue
        perror("Erreur lors de l'exécution de tcpdump");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus pour tcpdump");
        exit(EXIT_FAILURE);
    }

    // Processus parent : continuer le serveur
    printf("tcpdump lancé en arrière-plan (PID : %d).\n", pid);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Vérifier si l'option -d (debug) est activée
    int debug_mode = 0;
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug_mode = 1;
    }

    if (debug_mode) {
        run_tcpdump_in_background();
    }

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
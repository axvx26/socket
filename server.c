https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
https://broux.developpez.com/articles/c/sockets/
/*
Code du serveur
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>

/* Port local du serveur */
#define PORT 9600

int main (int argc, char *argv[])
{
/*
* Variables du serveur
*/

// Déclarer ici les variables suivantes :
// - sockfd le descripteur de socket
    int sockfd;

//  - structure d'adresse locale du serveur
    struct in_addr {
        u_long s_addr;
    };

//   - structure d'adresse du client
    struct sockaddr_in {
        u_short sin_family;
        u_short sin_port;
        struct in_addr sin_addr
        char sin_zero[8];
    }

//   - taille de l'adresse du client
    int addrlen;

/*
* Code du serveur
*
* - Ouvrir le socket du serveur
* - Remplir la structure d'adresse locale du serveur :
*   - la famille d'adresse
*   - l'adresse IP
*   - le port
* - Spécifier l'adresse locale du socket du serveur
*/

// Ouvrir le socket du serveur
sockfd = socket(PF_INET, SOCK_DGRAM, 0);
if (sockfd == INVALID_SOCKET) {
    perror("socket()");
    exit(errno);
}

// Initialisation de la structure d'adresse locale du serveur
sockaddr_in sin = { 0 };

sin.sin_addr.s_addr = htonl(INADDR_ANY);

sin.sin_family = PF_INET;

sin.sin_port = htons(PORT);

if(bind (sockfd, (const struct sockaddr *) &sin, sizeof sin) == SOCKET_ERROR){
    perror("bind()");
    exit(errno);
}

/*
* Boucle générale du serveur (infinie)
* Envoie et réception des données
*/
    while (1){
        // Reste à faire recv, write et close
    }
    return 0;
}
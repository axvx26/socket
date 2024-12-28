#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

void lance_serveur(int port) {
    int socket_serveur = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in my_addr = {0};
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(port);
    my_addr.sin_family = PF_INET;

    if (bind(socket_serveur, (const struct sockaddr *) &my_addr, sizeof(my_addr)) != 0) {
        printf("Erreur lors du binding");
        return;
    }

    if (listen(socket_serveur, 20) != 0) {
        printf("Erreur lors du listen");
        return;
    }
    
    int my_addr_size = sizeof(my_addr);
    int connexion_socket = accept(socket_serveur, (struct sockaddr *) &my_addr, &my_addr_size);
    if (connexion_socket == -1) {
        printf("Erreur lors de la connexion au client");
        return;
    }

    char input_buf[20];
    char output_buf[20] = "ok\0";
    do {
        memset(input_buf, 0, 20);
        read(connexion_socket, input_buf, 20);
        printf("reçu le message du client : %s", input_buf);

        // sprintf(output_buf, "ACK : %s ", input_buf);
        write(connexion_socket, "ok", 20);
        int cmp = strcmp(input_buf, "/");
        if (cmp == 0) {
            break;
        }
    } while(1);
    
    close(socket_serveur);
}
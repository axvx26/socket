#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

void lance_client(char* addr, int port) {
    int connexion_socket = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in my_addr = {0};
    inet_pton(PF_INET, addr, &(my_addr.sin_addr));
    my_addr.sin_port = htons(port);
    my_addr.sin_family = PF_INET;


    if (connect(connexion_socket, (const struct sockaddr *) &my_addr, sizeof(my_addr)) != 0) {
        printf("Erreur lors de la connexion au serveur");
        return;
    }
    
    printf("Saisir votre message:\n");
    char output_buf[20] = "ok\0";
    scanf("%s", output_buf);
    write(connexion_socket, output_buf, 20);

    char input_buf[20];
    memset(input_buf, 0, 20);
    read(connexion_socket, input_buf, 20);
    printf("reçu le message du serveur: %s", input_buf);
    
    
    close(connexion_socket);
}
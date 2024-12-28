#include "client_TCP.h"

int main(int argc, char * argv[]) {
    // argv[1] : l'adresse IP du serveur
    // argv[2] : le port
    lance_client(argv[1], atoi(argv[2]));
    return 0;
}
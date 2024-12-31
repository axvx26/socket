/*
*                  .__            __                        /\            .___ *
*   _____  _____   |__|  ____   _/  |_   ____  ______      / /  __ __   __| _/______ *
*  /     \ \__  \  |  | /    \  \   __\_/ ___\ \____ \    / /  |  |  \ / __ | \____ \ *
* |  Y Y  \ / __ \_|  ||   |  \  |  |  \  \___ |  |_> >  / /   |  |  // /_/ | |  |_> > *
* |__|_|  /(____  /|__||___|  /  |__|   \___  >|   __/  / /    |____/ \____ | |   __/ *
*       \/      \/          \/              \/ |__|     \/                 \/ |__| *
*/

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int protocole = 0;
    int role = 0;
    int c;
    char reponse_dev = 'n';   // par défaut mode dev = 'n'
    int dev_mode = 0;

    // Boucle pour s'assurer d'avoir un protocole valide
    do {
        printf("=== Choix du protocole ===\n");
        printf("1. TCP\n");
        printf("2. UDP\n");
        printf("Entrez votre choix : ");

        if (scanf("%d", &protocole) != 1) {
            // On vide le buffer si saisie invalide
            while ((c = getchar()) != '\n' && c != EOF);
            protocole = 0; 
        }

        if (protocole != 1 && protocole != 2) {
            printf("Protocole invalide. Veuillez réessayer.\n\n");
        }
    } while (protocole != 1 && protocole != 2);

    // Boucle pour s'assurer d'avoir un rôle valide
    do {
        printf("\n=== Choix du rôle ===\n");
        printf("1. Client\n");
        printf("2. Serveur\n");
        printf("Entrez votre choix : ");

        if (scanf("%d", &role) != 1) {
            while ((c = getchar()) != '\n' && c != EOF);
            role = 0;
        }

        if (role != 1 && role != 2) {
            printf("Rôle invalide. Veuillez réessayer.\n");
        }
    } while (role != 1 && role != 2);

    // Demander si on veut activer le mode développeur
    printf("\nVoulez-vous activer le mode développeur ? (o/n) : ");
    // On vide le buffer avant de lire un caractère
    while ((c = getchar()) != '\n' && c != EOF);

    scanf("%c", &reponse_dev);
    // Si l'utilisateur tape 'o' ou 'O', on active dev_mode
    if (reponse_dev == 'o' || reponse_dev == 'O') {
        dev_mode = 1;
    }

    // protocole 1 = TCP, protocole 2 = UDP
    // role 1 = Client, role 2 = Serveur
    if (protocole == 1) {
        // TCP
        if (role == 1) {
            // Client TCP
            if (dev_mode) {
                system("./client_tcp localhost -d");
            } else {
                system("./client_tcp localhost");
            }
        } else {
            // Serveur TCP
            if (dev_mode) {
                system("./serveur_tcp -d");
            } else {
                system("./serveur_tcp");
            }
        }
    } else {
        // UDP
        if (role == 1) {
            // Client UDP
            system("./client_udp localhost"); // pas de mode dev en client UDP
        } else {
            // Serveur UDP
            if (dev_mode) {
                system("./serveur_udp -d");
            } else {
                system("./serveur_udp");
            }
        }
    }

    return 0;
}
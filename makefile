# Nom du compilateur
CC = gcc

# Options de compilation (ajoutez -g pour débug, -Wall pour warnings, etc.)
CFLAGS = -Wall -O2

# Cibles "finales" que l'on veut obtenir
all: client_tcp serveur_tcp client_udp serveur_udp launcher

client_tcp: client_tcp.c
	$(CC) $(CFLAGS) $< -o $@

serveur_tcp: serveur_tcp.c
	$(CC) $(CFLAGS) $< -o $@

client_udp: client_udp.c
	$(CC) $(CFLAGS) $< -o $@

serveur_udp: serveur_udp.c
	$(CC) $(CFLAGS) $< -o $@

launcher: main.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f client_tcp serveur_tcp client_udp serveur_udp launcher
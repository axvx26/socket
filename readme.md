# TP Programmation réseau

## Noms et adresses mail

- VO Alexis : alexis.vo@etu-upsaclay.fr
- GERVINET Théo : theo.gervinet@etu-upsaclay.fr
  

## Liste des fichiers

- client_udp.c
- client_tcp.c
- serveur_udp.c
- serveur_tcp.c
- main.c
- makefile

## Répartition du travail

Le travail a été effectué en parallèle par les deux membres du binôme pour une répartition égale.

## Fonctionnalités

Les fichiers listés ci-dessus permettent la communication entre deux processus sur la base du modèle client/serveur,
avec les protocoles de communication TCP et UDP. Les codes respectent les schémas de la structure client/serveur selon le protocole.
Un makefile permet de générer les différents exécutables ainsi qu'un launcher. L'exécution du launcher propose à l'utilisateur de choisir
le protocole puis le rôle (client ou serveur) voulu avant d'exécuter le code adapté. Il propose également un mode développeur pour du debugage.

### Serveur tcp (serveur_tcp.c)

Le programme du serveur tcp est multi threadé:
- un thread de type console qui attend la saisie clavier de l'utilisateur pour répondre à deux commandes:
  - CLEAR qui efface la console
  - SHUTDOWN qui permet d'arrêter le serveur
- un thread correspondant au programme principal qui assure la communication TCP

Le programme s'arrête si captation du signal envoyé par Ctrl+C

### Client tcp (client_tcp.c)

Le programme du client tcp tente d'établir une connexion avec un serveur un certain nombre de fois et s'arrête s'il n'y arrive pas.
S'il y arrive, l'utilisateur peut envoyer ses messages et se déconnecter ensuite soit en tapant le message "DISCONNECT", soit grâce à
Ctrl+C

### Serveur udp (serveur_udp.c)

A COMPLETER

### Client udp (client_udp.c)

Le programme du client udp fonctionne selon le principe classique du protocole
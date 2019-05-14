#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>

#define PORT_TCP 8080
#define PORT_UDP 8888
#define TAILLE_BUFFER 1024
#define TRUE   1
#define FALSE  0
#define MULTICAST_GROUP "226.1.2.3"
#define MULTICAST_IP "127.0.1.1"

struct info_client {
  char pseudo[24];
  char adresse[24];
};
struct liste_client {
  int id;
  char pseudo[24];
  char adressetcp[24];
  char identifiant;
};

int creerSocketTCP();
void menuAttente();
char* getIp();
#endif

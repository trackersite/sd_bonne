#ifndef COMMON_H
#define COMMON_H

#define PORT_TCP 8080
#define PORT_UDP 8888
#define TAILLE_BUFFER 200
#define TRUE   1
#define FALSE  0
#define MULTICAST_GROUP "226.1.2.3"
#define MULTICAST_IP "127.0.1.1"

struct clients_connectes {
  char *pseudo;
  struct sockaddr_in infos;
};

int creerSocketTCP();


#endif

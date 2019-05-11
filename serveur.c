#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include "serveur.h"

int main (int argc, char *argv[]) {
  /*UDP*/
  struct sockaddr_in multicast_s; /*adresse et inof de multicast*/
  struct info_client infos_client; /* contient les infos d'un client qui passe par le multicast*/
  struct liste_client clients_connectes[4];
  int multicast_len;  /*taille de la structure multicast_s*/
  struct ip_mreq requete_multicast;
  /* TCP */
  char buffer_serveur[TAILLE_BUFFER];/* buffer for sending & receiving data */
  struct sockaddr_in contacter_client; /* server address information*/
  int socket_mcast;                 /* socket multicast serveur          */
  int socket_tcp;                 /* socket connected to client          */
  int identifiant = 1;
  char* id;

  /* set up socket */
  socket_mcast = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_mcast < 0) {
      perror("socket");
      exit(1);
  }
  bzero((char *)&multicast_s, sizeof(multicast_s));
  multicast_s.sin_family = AF_INET;
  multicast_s.sin_addr.s_addr = htonl(INADDR_ANY);
  multicast_s.sin_port = htons(PORT_UDP);
  multicast_len = sizeof(multicast_s);

  if (bind(socket_mcast, (struct sockaddr *) &multicast_s, sizeof(multicast_s)) < 0) {
      perror("bind");
  	  exit(1);
  }
  requete_multicast.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
  requete_multicast.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(socket_mcast, IPPROTO_IP, IP_ADD_MEMBERSHIP, &requete_multicast, sizeof(requete_multicast)) < 0) {
  	 perror("setsockopt requete_multicast");
  	 exit(1);
  }
  while (1) {
      if ((recvfrom(socket_mcast,(struct info_client*)&infos_client , sizeof(infos_client), 0, (struct sockaddr *) &multicast_s, &multicast_len) < 0)) {
          perror("recvfrom() server");
      }
    	printf("%s: Message réçu du Client via Multicast = \"%s\"\n", inet_ntoa(multicast_s.sin_addr), infos_client.pseudo);
      printf("Adresse réçu du Client via Multicast = \"%s\"\n", infos_client.adresse);

      strcpy(clients_connectes[identifiant-1].pseudo, infos_client.pseudo);
      strcpy(clients_connectes[identifiant-1].adressetcp, infos_client.adresse);
      clients_connectes[identifiant-1].id = identifiant;
      /*printf("ps%s\n", clients_connectes[identifiant-1].pseudo);
      printf("ad%s\n", clients_connectes[identifiant-1].adressetcp);
      printf("id%d\n", clients_connectes[identifiant-1].id);*/
      identifiant++;
      /*********************************************************************/
      /*                                                                   */
      /*                                                                   */
      /*                          PARTIE TCP                               */
      /*                                                                   */
      /*                                                                   */
      /*********************************************************************/
      // socket create and varification
      socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
      if (socket_tcp == -1) {
          printf("socket creation failed...\n");
          exit(0);
      }
      bzero(&contacter_client, sizeof(contacter_client));
      // assign IP, PORT
      contacter_client.sin_family = AF_INET;
      contacter_client.sin_addr.s_addr = inet_addr(infos_client.adresse);
      contacter_client.sin_port = htons(PORT_TCP);

      // connect the client socket to server socket
      if (connect(socket_tcp, (struct sockaddr*)&contacter_client, sizeof(contacter_client)) != 0) {
          printf("connection with the server failed...\n");
          exit(0);
      } else {
          printf("connected to the server..\n");
      }
      strcpy(buffer_serveur, "Hello from TCP");
      if (send(socket_tcp, &clients_connectes, sizeof(clients_connectes), 0) < 0) {
          perror("Send()");
          exit(1);
      }

      printf("Message envoyé au client.\n");

      close(socket_tcp);
  }
  printf("Server ended successfully\n");
  return 0;
}

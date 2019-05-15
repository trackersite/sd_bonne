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
  struct sockaddr_in multicast_s; /* adresse et inof de multicast */
  struct info_client infos_client; /* contient les infos d'un client qui passe par le multicast */
  struct liste_client clients_connectes[4]; /* liste de clients en ligne */
  int multicast_len;  /* taille de la structure multicast_s */
  struct ip_mreq requete_multicast; /* requete multicast */
  /* TCP */
  char buffer_serveur[TAILLE_BUFFER]; /* buffer pour envoyer les données */
  struct sockaddr_in contacter_client; /* adresse du client */
  int socket_mcast;                 /* socket Multicast du serveur */
  int socket_tcp;                 /* socket TCP du client */
  int identifiant = 1; /* identifiant */

  /* creation de la socket UDP Multicast */
  socket_mcast = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_mcast < 0) {
      perror("socket");
      exit(EXIT_FAILURE);
  }

  /* initialisation de la structure à zéro */
  bzero((char *)&multicast_s, sizeof(multicast_s));

  /* Affecter les information du serveur dans la structure */
  multicast_s.sin_family = AF_INET;
  multicast_s.sin_addr.s_addr = htonl(INADDR_ANY);
  multicast_s.sin_port = htons(PORT_UDP);
  multicast_len = sizeof(multicast_s);

  /* initialisation des id des clients */
  for (int i = 0; i < 4; i++) {
    clients_connectes[i].id = -1;
  }

  /* bind de l'adresse Multicast */
  if (bind(socket_mcast, (struct sockaddr *) &multicast_s, sizeof(multicast_s)) < 0) {
      perror("bind()");
  	  exit(EXIT_FAILURE);
  }

  /* parametrage du groupe et adresse Multicast */
  requete_multicast.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
  requete_multicast.imr_interface.s_addr = htonl(INADDR_ANY);

  /* configuration du Multicast */
  if (setsockopt(socket_mcast, IPPROTO_IP, IP_ADD_MEMBERSHIP, &requete_multicast, sizeof(requete_multicast)) < 0) {
  	 perror("setsockopt requete_multicast");
  	 exit(EXIT_FAILURE);
  }

  while (1) {
    /* reçoit message */
      if ((recvfrom(socket_mcast,(struct info_client*)&infos_client , sizeof(infos_client), 0, (struct sockaddr *) &multicast_s, &multicast_len) < 0)) {
          perror("recvfrom() server");
          exit(EXIT_FAILURE);
      }

      printf("Connexion Multicast etablie.\n");

      /* ajoute l'information par rapport aux clients dans la structure */
      strcpy(clients_connectes[identifiant-1].pseudo, infos_client.pseudo);
      strcpy(clients_connectes[identifiant-1].adressetcp, infos_client.adresse);
      /* affectation d'identifiant unique */
      clients_connectes[identifiant-1].id = identifiant;

      /*********************************************************************/
      /*                                                                   */
      /*                          PARTIE TCP                               */
      /*                                                                   */
      /*********************************************************************/

      /* creation de la socket TCP */
      if ((socket_tcp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
          printf("socket creation failed...\n");
          exit(0);
      }

      /* initialisation de la structure à zéro */
      bzero(&contacter_client, sizeof(contacter_client));
      /* parametrage du adresse client */
      contacter_client.sin_family = AF_INET;
      contacter_client.sin_addr.s_addr = inet_addr(infos_client.adresse);
      contacter_client.sin_port = htons(PORT_TCP);

      /* accepter que 4 clients */
      if (identifiant > 4) {
        printf("Connexion refuse, le serveur est plein.\n");
        close(socket_tcp);
      } else {
        /* Si le nombre de clients <= 4 alors accepter la connexion */
        if (connect(socket_tcp, (struct sockaddr*)&contacter_client, sizeof(contacter_client)) != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }

        printf("Connexion serveur TCP etablie.\n");

        /* envoyer la structure contenant l'information des clients */
        if (send(socket_tcp, clients_connectes, sizeof(clients_connectes), 0) < 0) {
            perror("Send()");
            exit(1);
        }
        identifiant++;

        close(socket_tcp);
    }
  }
  return 0;
}

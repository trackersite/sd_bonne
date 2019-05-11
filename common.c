#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "common.h"

 int creerSocketTCP() {
   // adresse socket coté client
   static struct sockaddr_in addr_client;
   // adresse socket locale
   static struct sockaddr_in addr_serveur;
   // longueur adresse
   int lg_addr;
   // socket d'écoute
   int socket_ecoute;

   // création socket TCP d'écoute
   if ((socket_ecoute = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
     perror("creation socket");
     exit(1);
   }

   // liaison de la socket d'écoute sur le port
   bzero((char *) &addr_serveur, sizeof(addr_serveur));
   addr_serveur.sin_family = AF_INET;
   addr_serveur.sin_port = htons(0);
   addr_serveur.sin_addr.s_addr=htonl(INADDR_ANY);

   if (bind(socket_ecoute, (struct sockaddr*)&addr_serveur, sizeof(addr_serveur))== -1 ) {
     perror("erreur bind socket écoute");
     exit(1);
   }

   return socket_ecoute;
 }

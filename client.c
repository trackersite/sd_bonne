#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "client.h"

int main (int argc, char *argv[]) {
  struct info_client info_client;
  struct sockaddr_in multicast_c;
  int multicastlen, socket_mcast, err_envoi;

  char buffer_tcp[TAILLE_BUFFER];              /* data buffer for sending & receiving */
  struct hostent *hostnm;    /* server host name information        */
  struct sockaddr_in requete_serveur, adresse_serveur; /* requete qui arrive du serveur tcp et son adresse */
  int socket_tcp;                     /* client tcp socket for accept connexions */
  int server_len;                     /* taille structure socket tcp */
  int accepte_tcp;                    /* evaluation de fonction accept */
  struct liste_client clients_en_ligne[4];

  // socket create and verification
  socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_tcp == -1) {
      printf("socket creation failed...\n");
      exit(0);
  }

  bzero(&requete_serveur, sizeof(requete_serveur));
  requete_serveur.sin_family = AF_INET;
  //requete_serveur.sin_addr.s_addr = htonl(INADDR_ANY);
  requete_serveur.sin_port = htons(PORT_TCP); //8080

  char name[1024];
  gethostname(name, 1023);

   /* The host name is the first argument. Get the server address. */
   hostnm = gethostbyname(name);

   if (hostnm == (struct hostent *) 0) {
       fprintf(stderr, "Gethostbyname failed\n");
       exit(2);
   }

   /*
    * Put the server information into the server structure.
    * The port must be put into network byte order.
    */
  requete_serveur.sin_family      = AF_INET;
  requete_serveur.sin_port        = htons(PORT_TCP);
  requete_serveur.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

  //printf("DEBUG : %s\n", inet_ntoa(requete_serveur.sin_addr));

  printf("Veuillez choisir votre pseudo : ");
  scanf("%s", info_client.pseudo);
  strcpy(info_client.adresse, inet_ntoa(requete_serveur.sin_addr));
  /* set up socket */
  socket_mcast = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_mcast < 0) {
      perror("socket");
      exit(1);
  }
  bzero((char *)&multicast_c, sizeof(multicast_c));
  multicast_c.sin_family = AF_INET;
  multicast_c.sin_addr.s_addr = htonl(INADDR_ANY);
  multicast_c.sin_port = htons(PORT_UDP);
  multicastlen = sizeof(multicast_c);
  multicast_c.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

	err_envoi = sendto(socket_mcast, &info_client, sizeof(info_client), 0, (struct sockaddr *) &multicast_c, multicastlen);
	if (err_envoi < 0) {
   	   perror("sendto");
  	   exit(1);
	}

  /*********************************************************************/
  /*                          PARTIE TCP                               */
  /*********************************************************************/

  /* Permettre de connecter plusieurs clients en meme temps. */ // TODO: delete if bugs
  if (setsockopt(socket_tcp, SOL_SOCKET, SO_REUSEADDR, &requete_serveur, sizeof(requete_serveur)) < 0) {
  	 perror("setsockopt client");
  	 exit(1);
  }

  // Binding newly created socket to given IP and verification
  if ((bind(socket_tcp, (struct sockaddr*)&requete_serveur, sizeof(requete_serveur))) != 0) {
      printf("socket bind failed...\n");
      exit(0);
  }

  // Now server is ready to listen and verification
  if ((listen(socket_tcp, 4)) != 0) {
      printf("Listen failed...\n");
      exit(0);
  }

  server_len = sizeof(adresse_serveur);
  accepte_tcp = accept(socket_tcp, (struct sockaddr*)&adresse_serveur, &server_len);

  if (accepte_tcp < 0) {
      printf("server acccept failed...\n");
      exit(0);
  }

  /* The server sends back the same message. Receive it into the buffer. */
  if (recv(accepte_tcp, (struct liste_client *)&clients_en_ligne, sizeof(clients_en_ligne), 0) < 0) {
      perror("Recv()");
      exit(0);
  } else {
    int socket_autre_client;
    struct sockaddr_in addr_autre_clients;
    char buffer_echange[1024];

    printf("Liste des clients connectes: %s\n", buffer_tcp);

    //On fait une boucle pour que le client conecte aux autre client deja sur la partie
    for (int i = 0; i < 4; i++) {
      if (clients_en_ligne[i].id != -1) {
        printf("Identifiant : %d\n", clients_en_ligne[i].id);
        printf("Pseudo du client : %s\n", clients_en_ligne[i].pseudo);
        printf("Adresse IP du client : %s\n", clients_en_ligne[i].adressetcp);

        if (clients_en_ligne[i].pseudo != info_client.pseudo) {

          socket_autre_client = socket(AF_INET, SOCK_STREAM, 0);
          if (socket_autre_client == -1) {
              printf("socket creation failed...\n");
              exit(0);
          }

          bzero(&addr_autre_clients, sizeof(addr_autre_clients));
          // assign IP, PORT
          addr_autre_clients.sin_family = AF_INET;
          addr_autre_clients.sin_addr.s_addr = inet_addr(clients_en_ligne[i].adressetcp);
          addr_autre_clients.sin_port = htons(PORT_TCP);

          // connect the client socket to server socket
          if (connect(socket_autre_client, (struct sockaddr*)&addr_autre_clients, sizeof(addr_autre_clients)) != 0) {
              printf("connection with the server failed...\n");
              exit(0);
          }

          strcpy(buffer_echange, "Hello from Client one to Client two.");
          printf("Message d'autre client : %s\n", buffer_echange);

          if (send(socket_autre_client, buffer_echange, sizeof(buffer_echange), 0) < 0) {
              perror("Send()");
              exit(1);
          }
        }
      }
    }
  }

  close(socket_tcp);

  while (1) {

  }

  //close(socket_autre_client);

  return 0;
}

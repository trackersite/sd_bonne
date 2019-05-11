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
  struct liste_client log_clients[4];
  struct sockaddr_in multicast_c;
  int multicastlen, socket_mcast, err_envoi;

  char buffer_tcp[TAILLE_BUFFER];              /* data buffer for sending & receiving */
  struct hostent *hostnm;    /* server host name information        */
  struct sockaddr_in requete_serveur, adresse_serveur; /* requete qui arrive du serveur tcp et son adresse */
  int socket_tcp;                     /* client tcp socket for accept connexions */
  int server_len;                     /* taille structure socket tcp */
  int accepte_tcp;                    /* evaluation de fonction accept */

  // socket create and verification
  socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_tcp == -1) {
      printf("socket creation failed...\n");
      exit(0);
  } else {
      printf("Socket successfully created..\n");
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

	printf("Requête Multicast envoyé au Multicast.\n");
	err_envoi = sendto(socket_mcast, &info_client, sizeof(info_client), 0, (struct sockaddr *) &multicast_c, multicastlen);
	if (err_envoi < 0) {
   	   perror("sendto");
  	   exit(1);
	}

  /*********************************************************************/
  /*                                                                   */
  /*                                                                   */
  /*                          PARTIE TCP                               */
  /*                                                                   */
  /*                                                                   */
  /*********************************************************************/

  // Binding newly created socket to given IP and verification
  if ((bind(socket_tcp, (struct sockaddr*)&requete_serveur, sizeof(requete_serveur))) != 0) {
      printf("socket bind failed...\n");
      exit(0);
  }else{
      printf("Socket successfully binded..\n");
  }

  // Now server is ready to listen and verification
  if ((listen(socket_tcp, 4)) != 0) {
      printf("Listen failed...\n");
      exit(0);
  }else{
      printf("Server listening..\n");
  }

  server_len = sizeof(adresse_serveur);
  accepte_tcp = accept(socket_tcp, (struct sockaddr*)&adresse_serveur, &server_len);

  if (accepte_tcp < 0) {
      printf("server acccept failed...\n");
      exit(0);
  }else{
      printf("Connexion acceptee.\n");
  }
  /* The server sends back the same message. Receive it into the buffer. */
  if (recv(accepte_tcp, (struct liste_client*)&log_clients, sizeof(log_clients), 0) < 0) {
      perror("Recv()");
      exit(0);
  } else {

    printf("Message recu du serveur TCP pseudo: %s\n", log_clients[0].pseudo);
    printf("Message recu du serveur TCP adresse: %s\n", log_clients[0].adressetcp);
    printf("Message recu du serveur TCP id : %d\n", log_clients[0].id);
    /*
    printf("Message recu du serveur TCP pseudo: %s\n", log_clients[1].pseudo);
    printf("Message recu du serveur TCP adresse: %s\n", log_clients[1].adressetcp);
    printf("Message recu du serveur TCP id : %d\n", log_clients[1].id);
    */
    //On fait une boucle pour que le client conecte aux autre client deja sur la partie
    for (int i = 0; i < 4; i++) {
      /*TODO:C'EST L'IDDE IL FAUT LE MODIFIER JE PENSE
      if (log_clients[i].pseudo != info_client.pseudo && log_clients[i].adresse != NULL) {
        /* connexion de ce client aux autres client de la partie */
        // socket create and varification
      /*  struct sockaddr_in etab_conn_autres;
        int conn_aux_autres
        conn_aux_autres = socket(AF_INET, SOCK_STREAM, 0);
        if (conn_aux_autres == -1) {
            printf("socket creation failed...\n");
            exit(0);
        }
        bzero(&etab_conn_autres, sizeof(etab_conn_autres));
        // assign IP, PORT
        etab_conn_autres.sin_family = AF_INET;
        etab_conn_autres.sin_addr.s_addr = inet_addr(infos_client.adresse);
        etab_conn_autres.sin_port = htons(PORT_TCP);
        // connect the client socket to server socket
        if (connect(conn_aux_autres, (struct sockaddr*)&etab_conn_autres, sizeof(etab_conn_autres)) != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        } else {
            printf("connected to the server..\n");
        }
        strcpy(buffer_serveur, "Hello from TCP");
        if (send(conn_aux_autres, &etab_conn_autres, sizeof(etab_conn_autres), 0) < 0) {
            perror("Send()");
            exit(1);
        }*/
      }
    }
  
  printf("Tout s'est bien passée\n");
  return 0;
}

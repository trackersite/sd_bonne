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
  struct sockaddr_in addr;
  int addrlen, sock, cnt;
  struct ip_mreq mreq;
  char message[50];

  printf("Veuillez choisir votre pseudo : ");
  scanf("%s", message);

  /* set up socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORT_UDP);
  addrlen = sizeof(addr);

  addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

	 printf("Requête Multicast envoyé au Multicast.\n");
	 cnt = sendto(sock, message, sizeof(message), 0, (struct sockaddr *) &addr, addrlen);
	 if (cnt < 0) {
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

   char buf[1024];              /* data buffer for sending & receiving */
   struct hostent *hostnm;    /* server host name information        */
   struct sockaddr_in addr_client, serv_addr; /* server address                      */
   int sockfd;                     /* client socket                       */
   int len;
   int connfd;

   // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }

    bzero(&addr_client, sizeof(addr_client));

   // assign IP, PORT
   addr_client.sin_family = AF_INET;
   addr_client.sin_addr.s_addr = htonl(INADDR_ANY);
   addr_client.sin_port = htons(PORT_TCP); //8080

   // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (struct sockaddr*)&addr_client, sizeof(addr_client))) != 0) {
      printf("socket bind failed...\n");
      exit(0);
  } else {
    printf("Socket successfully binded..\n");
  }

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  } else {
    printf("Server listening..\n");
  }

  len = sizeof(serv_addr);

  connfd = accept(sockfd, (struct sockaddr*)&serv_addr, &len);
  if (connfd < 0) {
      printf("server acccept failed...\n");
      exit(0);
  } else {
    printf("Connexion acceptee.\n");
  }

  /* The server sends back the same message. Receive it into the buffer. */
  if (recv(connfd, buf, sizeof(buf), 0) < 0) {
      perror("Recv()");
      exit(6);
  } else {
    printf("Message recu du serveur TCP : %s\n", buf);
  }

   printf("Client Ended Successfully\n");

  return 0;
}

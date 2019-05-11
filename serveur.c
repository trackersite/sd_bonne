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
  struct sockaddr_in addr;
  int addrlen, sock, cnt;
  struct ip_mreq mreq;
  char message[50];
  /* TCP */
  char buf[1024];              /* buffer for sending & receiving data */
  struct sockaddr_in client; /* client address information          */
  struct sockaddr_in server, addr_client; /* server address information          */
  int s;                     /* socket for accepting connections    */
  int ns;                    /* socket connected to client          */
  int namelen;               /* length of client name               */
  int sockfd;

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

  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    perror("bind");
	  exit(1);
  }

  mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
	 perror("setsockopt mreq");
	 exit(1);
  }

  while (1) {
 	 if ((recvfrom(sock, message, sizeof(message), 0, (struct sockaddr *) &addr, &addrlen) < 0)) {
     perror("recvfrom() server");
   }
	 printf("%s: Message réçu du Client par Multicast = \"%s\"\n", inet_ntoa(addr.sin_addr), message);


   // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }

  bzero(&addr_client, sizeof(addr_client));
  // assign IP, PORT
  addr_client.sin_family = AF_INET;
  addr_client.sin_addr.s_addr = inet_addr("127.0.0.1");
  addr_client.sin_port = htons(PORT_TCP);

  // connect the client socket to server socket
  if (connect(sockfd, (struct sockaddr*)&addr_client, sizeof(addr_client)) != 0) {
      printf("connection with the server failed...\n");
      exit(0);
  } else {
    printf("connected to the server..\n");
  }

  memset(buf, 0, TAILLE_BUFFER);
  strcpy(buf, "Hello from TCP");

  if (send(sockfd, buf, sizeof(buf), 0) < 0) {
      perror("Send()");
      exit(5);
  }

  printf("Message envoyé au client.\n");

  }

  /*********************************************************************/
  /*                                                                   */
  /*                                                                   */
  /*                          PARTIE TCP                               */
  /*                                                                   */
  /*                                                                   */
  /*********************************************************************/

   close(ns);
   close(s);

   printf("Server ended successfully\n");

   return 0;
}

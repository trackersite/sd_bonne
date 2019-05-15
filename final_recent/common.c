#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h>
#include <sys/time.h>
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

   void menuAttente() {
     printf("\t***************************************************\n");
     printf("\t**               SALLE D'ATTENTE                 **\n");
     printf("\t***************************************************\n");
     printf("\t***************************************************\n");
     printf("\t**                                               **\n");
     printf("\t**              1.- Commencer partie             **\n");
     printf("\t**                                               **\n");
     printf("\t**              2.- Definir le score gagnant     **\n");
     printf("\t**                                               **\n");
     printf("\t**              3.- Afficher clients en ligne    **\n");
     printf("\t**                                               **\n");
     printf("\t**              9.- Deconnecter                  **\n");
     printf("\t**                                               **\n");
     printf("\t***************************************************\n");
     printf("\t***************************************************\n");
     printf("\tSaisie: ");
}

char* getIp() {
  struct ifaddrs *addrs, *tmp;
  getifaddrs(&addrs);
  tmp = addrs;
  struct sockaddr_in *pAddr;

  while (tmp) {
      if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
          pAddr = (struct sockaddr_in *)tmp->ifa_addr;
      }
      tmp = tmp->ifa_next;
  }
  freeifaddrs(addrs);

  return inet_ntoa(pAddr->sin_addr);
}

// Defining comparator function as per the requirement
static int myCompare(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

void trier(const char* arr[], int n) {
    qsort(arr, n, sizeof(const char*), myCompare);
}

void trierListe(struct liste_client clients_en_ligne[]) {

  const char* arr[] = {"NULL", "NULL", "NULL", "NULL"};

  int n = sizeof(arr) / sizeof(arr[0]);

  for (int i = 0; i < n; i++) {
    if (clients_en_ligne[i].id != -1) {
      arr[i] = clients_en_ligne[i].pseudo;
    }
  }

  trier(arr, n);

  printf("\t***************************************************\n");
  printf("\t**               CLIENTS EN LIGNE                **\n");
  printf("\t***************************************************\n");
  printf("\t***************************************************\n");
  for (int i = 0; i < n; i++) {
    if ((strcmp(arr[i], "NULL")) != 0) {
      printf("\tPseudo : %s \n", arr[i]);
      printf("\t-----------------------------------------------\n");
      }
    }
}

/* GESTION DE LA PARTIE */

//permet de générer nos 3 lancers de dés
 int tirerDes(int des[3]) {
     srand(time(NULL));
     for(int i = 0; i<3; i++ ){
       des[i] = rand()%(7-1)+1; //retourne un entier aléatoire entre 1 et 6
     }
     return(0);
 }

 int definirLeScore() {
   int score;

   printf("Veuillez definir le score gagnant : ");
   scanf("%d", &score);

   return score;
 }

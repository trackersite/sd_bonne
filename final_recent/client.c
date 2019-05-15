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
  /* UDP */
  struct info_client info_client; /* structure contenant les infos du client */
  struct sockaddr_in multicast_c; /* adresse du Multicast */
  int multicastlen, socket_mcast;
  /* TCP */
  char buffer_tcp[TAILLE_BUFFER]; /* le buffer pour la requete TCP */
  struct hostent *hostnm; /* variable qui va contenir le nom de la machine hôte */
  struct sockaddr_in requete_serveur, adresse_serveur; /* requete qui arrive du serveur tcp et son adresse */
  int socket_tcp;                     /* client tcp socket pour accepter connexions */
  int server_len;                     /* taille d'adresse_serveur du socket tcp */
  int accepte_tcp;                    /* evaluation de fonction accept */
  struct liste_client clients_en_ligne[4]; /* structure contenant la liste des clients connectés */
  char buffer_echange[TAILLE_BUFFER]; /* buffer qui contient les messages entre clients TCP */
  int socket_autre_client; /* socket d'autres clients */
  struct sockaddr_in addr_autre_clients; /* adresse des autres clients */
  char name[TAILLE_BUFFER]; /* buffer qui contient les messages entre clients TCP */
  int choix; /* sert pour le choix dans le menu */
  int nb_joueurs = 1;
  int des[3];
  int gagne = 0;
  int score = 0;


  /* creation de la socket UDP pour la partie Multicast */
  socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_tcp == -1) {
      printf("socket creation failed...\n");
      exit(0);
  }

  /* initialisation de la structure à zéro */
  bzero(&requete_serveur, sizeof(requete_serveur));

  requete_serveur.sin_family = AF_INET; /* assigner la famille INET */
  requete_serveur.sin_port = htons(PORT_TCP); /* PORT d'écoute pour TCP */

  /* récupérer le nom de la hôte */
  gethostname(name, 1023);
  hostnm = gethostbyname(name);

   if (hostnm == (struct hostent *) 0) {
      fprintf(stderr, "Erreur de hostmn.\n");
       exit(EXIT_FAILURE);
   }

   /* Affecter les information du serveur dans la structure */
  requete_serveur.sin_family      = AF_INET;
  requete_serveur.sin_port        = htons(PORT_TCP);
  requete_serveur.sin_addr.s_addr = inet_addr(getIp());

  /* choix du pseudo */
  printf("Veuillez choisir votre pseudo : ");
  scanf("%s", info_client.pseudo);
  strcpy(info_client.adresse, inet_ntoa(requete_serveur.sin_addr));

  /* creation de la socket Multicast */
  if ((socket_mcast = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
  }

  /* initialisation de la structure à zéro */
  bzero((char *)&multicast_c, sizeof(multicast_c));

  /* Affecter les informations au Multicast */
  multicast_c.sin_family = AF_INET;
  multicast_c.sin_addr.s_addr = htonl(INADDR_ANY);
  multicast_c.sin_port = htons(PORT_UDP);
  multicastlen = sizeof(multicast_c);
  multicast_c.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

  /* envoyer la requête au Multicast contenant les données du client TCP */
    if ((sendto(socket_mcast, &info_client, sizeof(info_client), 0, (struct sockaddr *) &multicast_c, multicastlen)) == -1) {
          perror("Multicast sendto()");
         exit(EXIT_FAILURE);
    }

  /*********************************************************************/
  /*                          PARTIE TCP                               */
  /*********************************************************************/

  /* Permettre de connecter plusieurs clients en meme temps. */
  if (setsockopt(socket_tcp, SOL_SOCKET, SO_REUSEADDR, &requete_serveur, sizeof(requete_serveur)) < 0) {
       perror("client setsockopt()");
       exit(EXIT_FAILURE);
  }

  /* bind adresse TCP */
  if ((bind(socket_tcp, (struct sockaddr*)&requete_serveur, sizeof(requete_serveur))) != 0) {
      perror("client bind()");
      exit(EXIT_FAILURE);
  }

  /* attendre les connexions TCP */
  if ((listen(socket_tcp, 4)) != 0) {
      printf("Listen failed...\n");
      exit(EXIT_FAILURE);
  }

  /* calcul de la longueur de l'adresse du serveur */
  server_len = sizeof(adresse_serveur);
  /* accepter nouvell requête TCP */
  accepte_tcp = accept(socket_tcp, (struct sockaddr*)&adresse_serveur, &server_len);

  if (accepte_tcp < 0) {\
    perror("client accept()");
    exit(EXIT_FAILURE);
  }

  /* reçois le message du client TCP */
  if (recv(accepte_tcp, (struct liste_client *)&clients_en_ligne, sizeof(clients_en_ligne), 0) < 0) {
      perror("client recv()");
      exit(EXIT_FAILURE);
  } else {
    memset(buffer_tcp, 0, TAILLE_BUFFER);
    printf("\t***************************************************\n");
    printf("\t**               LISTE CLIENTS                   **\n");
    printf("\t***************************************************\n");

    /* connecter aux autres clients */
    for (int i = 0; i < 4; i++) {
      if (clients_en_ligne[i].id != -1) {
        printf("\tIdentifiant : %d\n", clients_en_ligne[i].id);
        printf("\tPseudo du client : %s\n", clients_en_ligne[i].pseudo);
        printf("\tAdresse IP du client : %s\n", clients_en_ligne[i].adressetcp);

        /* Etablir la connexion avec autre clients si les ID sont differents */
        if (clients_en_ligne[i].id != 1) {
          socket_autre_client = socket(AF_INET, SOCK_STREAM, 0);
          nb_joueurs++;
          if (socket_autre_client == -1) {
              printf("socket creation failed...\n");
              exit(EXIT_FAILURE);
          }

          bzero(&addr_autre_clients, sizeof(addr_autre_clients));

          /* assigner les information dans la structure du nouveau client */
          addr_autre_clients.sin_family = AF_INET;
          addr_autre_clients.sin_addr.s_addr = inet_addr(clients_en_ligne[i].adressetcp);
          addr_autre_clients.sin_port = htons(PORT_TCP);

          /* Permettre de connecter plusieurs clients en meme temps. */
          if (setsockopt(socket_autre_client, SOL_SOCKET, SO_REUSEADDR, &addr_autre_clients, sizeof(addr_autre_clients)) < 0) {
             perror("client setsockopt()");
             exit(EXIT_FAILURE);
          }
          /* connexion TCP  */
          if (connect(socket_autre_client, (struct sockaddr*)&addr_autre_clients, sizeof(addr_autre_clients)) != 0) {
              perror("client connect()");
              exit(EXIT_FAILURE);
          }

          /* messageà envoyer */
          strcpy(buffer_echange, "\tSalut de la part d'un client.");
          printf("Message d'autre client : %s\n", buffer_echange);

          /* envoie de message */
          if (send(socket_autre_client, buffer_echange, sizeof(buffer_echange), 0) < 0) {
              perror("client send()");
              exit(EXIT_FAILURE);
          }
        }
      }
    }
  }

  /* fermeture de la socket écoute */
  close(socket_tcp);

  int cont = 1;
  /* le menu */
  do {
      menuAttente();
      scanf("%d", &choix);
      switch(choix){
          case 1:
            printf("\tVeuillez choisir une score : ");
            scanf("%d", &score);
            while(gagne == 0){
              printf("\t*********************TOUR %d*********************\n", cont);
              for(int i = 0; i < 3; i++){
                for (int j = 1; j < 4; j++) {
                          des[j] = 0;
                          des[j] = rand()%(1-7) +1;
                          printf("\tDe %d: %d\n", j,des[j]);
                    }
                    //cas de la velute
                    if(des[0]+ des[1] == des[2]){
                      clients_en_ligne[i].score = clients_en_ligne[i].score + (des[2] * des[2]);
                    }

                    //cas de la chouette
                    if(des[0] == des[1]){
                      clients_en_ligne[i].score = clients_en_ligne[i].score + (des[0] * des[0]);
                    }

                    //cas du cul de chouette
                    if(des[0] == des[1] == des[2]){
                      switch(des[0]){
                          case 1:
                            clients_en_ligne[i].score = clients_en_ligne[i].score + 50;
                            break;
                          case 2:
                            clients_en_ligne[i].score = clients_en_ligne[i].score + 60;
                            break;
                          case 3:
                            clients_en_ligne[i].score = clients_en_ligne[i].score + 70;
                            break;
                          case 4:
                            clients_en_ligne[i].score = clients_en_ligne[i].score + 80;
                            break;
                          case 5:
                            clients_en_ligne[i].score = clients_en_ligne[i].score + 90;
                            break;
                          case 6:
                            clients_en_ligne[i].score = clients_en_ligne[i].score + 100;
                            break;
                      }
                    }
                    for (int i = 0; i < 4; i++) {
                        printf("\tScore %s: %d\n", clients_en_ligne[i].pseudo, clients_en_ligne[i].score);
                    }
                    if(clients_en_ligne[i].score >= score){
                      printf("\t%s a gagné! \n", clients_en_ligne[i].pseudo);
                      gagne = 1;
                      break;
                    }
                  }
                  printf("\n");
                  cont++;
                }
            break;
          case 2:
            trierListe(clients_en_ligne);
            break;
          case 9:
            break;
          default:
            printf("\nEntrez un nombre valide! \n \n");
            break;
          }
  } while (choix != 9);

  /* fermeture de la socket du nouveau client */
  close(socket_autre_client);

  return 0;
}

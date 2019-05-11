#ifndef CLIENT_H
#define CLIENT_H
#include "common.h"

#define MAX_TAILLE_NOM 30

struct Joueurs {
  int id;
  char pseudo[MAX_TAILLE_NOM];
  int points;
  int adresse_socket;
  int socket;
  int maitre;
};

#endif

#ifndef MESSAGGI_H
#define MESSAGGI_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include "list.h"

/*#define STRNUM 30*/

/*#define MISURA_UMIDITA 0*/
/*#define MISURA_TEMPERATURA 1*/

/*typedef struct Misura_ {*/
/*  char nome[STRNUM+1];*/
/*  int tipo;*/
/*  double valore;*/
/*  int contatore;*/
/*} Misura;*/

/*void PrintMisura(Misura misura);*/

void ForwardItem(char *host_name, int port, itemType item, int* answer);

LIST RemoveLettore(LIST l, char* nome_lettore);

#endif

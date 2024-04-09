#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"


#define BUF_SIZE 1000


char *host_name = "127.0.0.1"; /* local host */
int port = 8000;

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    struct hostent* server;
    LIST sensori, tmp;
    itemType msg;
    int i;

    if (argc < 3) {
        printf("Usages:\n");
        printf("  %s nome %d sensore1 [sensore2 ...]\n", argv[0], CLIENTE_LETTORE_AGGIUNTA);
        printf("  %s nome %d\n", argv[0], CLIENTE_LETTORE_RIMOZIONE);
        exit(-1);
    }
    strncpy(msg.nome_lettore, argv[1], STRNUM);
    strncpy(msg.nome_sensore, "EMTPY", STRNUM);
    msg.tipo_cliente = atoi(argv[2]);
    msg.valore = -1.0;
    msg.sockfd = -1;

    if (msg.tipo_cliente == CLIENTE_LETTORE_AGGIUNTA) {
        printf("Opzione: ISCRIZIONE a notifiche sensori\n");
    } else if (msg.tipo_cliente == CLIENTE_LETTORE_RIMOZIONE) {
        printf("Opzione: DISISCRIZIONE a notifiche sensori\n");
    } else {
        printf("Opzione NON valida: terminazione processo\n");
        exit(0);
    }

    if ((server = gethostbyname(host_name)) == 0) {
        perror("Error resolving local host\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *) (server->h_addr))->s_addr;
    serv_addr.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error opening socket\n");
        exit(1);
    }

    if (connect(sockfd, (void*) &serv_addr, sizeof (serv_addr)) == -1) {
        perror("Error connecting to socket\n");
        exit(1);
    }

    if (msg.tipo_cliente == CLIENTE_LETTORE_AGGIUNTA) {
        sensori = NewList();
        for (i = 3; i < argc; ++i) {
            strncpy(msg.nome_sensore, argv[i], STRNUM);
            sensori = EnqueueLast(sensori, msg);
        }
        printf("lista sensori a cui iscriversi/disiscriversi\n");
        PrintList(sensori);


        printf("invio lista sensori per sottoscrizione alle misure di sensori\n");
        tmp = sensori;
        while (!isEmpty(tmp)) {
            PrintItem(tmp->item);
            if (send(sockfd, &tmp->item, sizeof (tmp->item), 0) == -1) {
                perror("Error on send\n");
                exit(1);
            }
            tmp = tmp->next;
        }
        strncpy(msg.nome_sensore, "", STRNUM);
        printf("invio messaggio con nome_sensore=\"\" di fine lista\n");
        if (send(sockfd, &msg, sizeof (msg), 0) == -1) {
            perror("Error on send\n");
            exit(1);
        }

        printf("\ninizio ricezione messaggi\n");

        do {
            if (recv(sockfd, &msg, sizeof (msg), 0) == -1) {
                perror("Error in receiving response from server\n");
                exit(1);
            }
            printf("\n---\nricevuta misura: ");
            PrintItem(msg);
        } while (strncmp(msg.nome_sensore, "", STRNUM) != 0);
        close(sockfd);
    } else if (msg.tipo_cliente == CLIENTE_LETTORE_RIMOZIONE) {
        printf("\n---\ninvio messaggio di disiscrizione:\n");
        strncpy(msg.nome_sensore, "", STRNUM);
        msg.tipo_cliente = CLIENTE_LETTORE_RIMOZIONE;
        PrintItem(msg);
        if (send(sockfd, &msg, sizeof (msg), 0) == -1) {
            perror("Error on send\n");
            exit(1);
        }
    }

    return 0;
}




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "util.h"
#include "list.h"

#define BUF_SIZE 1000


char *host_name = "127.0.0.1"; /* local host */
int port_hub = 8001;

int main(int argc, char *argv[]) {
    itemType msg;
    int sockfd, num_letture, i, answer;
    
    srand(time(NULL));

    if (argc < 3) {
        printf("Usage: %s nome num_letture\n", argv[0]);
        exit(-1);
    }
    strncpy(msg.nome_sensore, argv[1], STRNUM);
    strncpy(msg.nome_lettore, "", STRNUM);
    msg.tipo_cliente = CLIENTE_SENSORE;
    msg.valore = 0.0;
    msg.sockfd = -1;
    num_letture = atoi(argv[2]);


    struct sockaddr_in serv_addr;
    struct hostent* server;

    if ((server = gethostbyname(host_name)) == 0) {
        perror("Error resolving local host\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *) (server->h_addr))->s_addr;
    serv_addr.sin_port = htons(port_hub);

    for (i = 0; i < num_letture; ++i) {
        printf("\n---\nconnessione e invio misura %d\n", i);

        sockfd = socket(PF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("Error opening socket\n");
            exit(1);
        }
        
        if (connect(sockfd, (void*) &serv_addr, sizeof (serv_addr)) == -1) {
            perror("Error connecting to socket\n");
            exit(1);
        }

        msg.valore = 40.0 * (rand() * 1.0 / RAND_MAX) - 10.0;
        PrintItem(msg);

        if (send(sockfd, &msg, sizeof (msg), 0) == -1) {
            perror("Error on send\n");
            exit(1);
        }

        printf("in attesa di conferma...\n");

        if (recv(sockfd, &answer, sizeof (answer), 0) == -1) {
            perror("Error in receiving response from server\n");
            exit(1);
        }

        printf("risposta del server %d e chiusura connessione\n", answer);
        close(sockfd);
        
        sleep(5);
    }

    printf("fine invio misure\n");


    return 0;
}




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>
#include "list.h"
#include "util.h"


#define BUF_SIZE 1000


char *host_name = "127.0.0.1"; /* local host */
int port_server = 8000;
int port_hub = 8001;

int main(int argc, char** argv) {
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    LIST sensori;
    itemType item;
    int num_sensori, ret, sockfd, tmpsockfd, answer;

    sensori = NewList();


    if (argc < 2) {
        printf("Usage: %s num_sensori\n", argv[0]);
        exit(-1);
    }
    num_sensori = atoi(argv[1]);


    // Socket opening
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error opening socket");
        exit(1);
    }

    int options = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0) {
        perror("Error on setsockopt");
        exit(1);
    }

    bzero(&serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_hub);

    // Address bindind to socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) == -1) {
        perror("Error on binding");
        exit(1);
    }

    // Maximum number of connection kept in the socket queue
    if (listen(sockfd, 20) == -1) {
        perror("Error on listen");
        exit(1);
    }


    socklen_t address_size = sizeof ( cli_addr);

    while (1) {
        printf("\n---\nHub: in attesa di connessioni\n");

        // New connection acceptance		
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &address_size);
        if (newsockfd == -1) {
            perror("Error on accept");
            exit(1);
        }

        // Message reception
        if (recv(newsockfd, &item, sizeof (item), 0) == -1) {
            perror("Error on receive");
            exit(1);
        }

        printf("Ricevuto messaggio:\n");
        PrintItem(item);
        item.sockfd = newsockfd;
        sensori = EnqueueLast(sensori, item);
        printf("lista degli %d/%d sensori in attesa di comunicare:\n", getLength(sensori), num_sensori);
        PrintList(sensori);

        if (getLength(sensori) >= num_sensori) {
            printf("inizio inoltro dati sensoriali: %d/%d\n", getLength(sensori), num_sensori);
            ret = 1;
            while (!isEmpty(sensori) && ret) {
                item = getHead(sensori);
                sensori = DequeueFirst(sensori);

                ret = fork();
                if (ret == 0) {
                    tmpsockfd = item.sockfd;
                    item.sockfd = -1;
                    printf("Hub figlio %d: chiusura socket server\n", getpid());
                    close(sockfd);
                    printf("Hub figlio %d: inoltro item\n", getpid());
                    PrintItem(item);
                    ForwardItem(host_name, port_server, item, &answer);
                    printf("Hub figlio %d: inoltro la risposta del server %d\n", getpid(), answer);
                    if (send(tmpsockfd, &answer, sizeof (answer), 0) == -1) {
                        perror("Error on send");
                        exit(1);
                    }
                    close(tmpsockfd);
                    printf("Hub figlio %d: chiusura delle altre connessioni aperte\n", getpid());
                    while (!isEmpty(sensori)) {
                        item = getHead(sensori);
                        close(item.sockfd);
                        sensori = DequeueFirst(sensori);
                    }
                    sensori = DeleteList(sensori);
                    printf("Hub figlio %d: FINE operazioni\n", getpid());
                    exit(0);
                } else {
                    printf("Hub padre: lascio la gestione del client al processo %d: prossimo client\n", ret);
                    close(item.sockfd);
                }
            }
        }
    }

    close(sockfd);

    sensori = DeleteList(sensori);

    return 0;
}




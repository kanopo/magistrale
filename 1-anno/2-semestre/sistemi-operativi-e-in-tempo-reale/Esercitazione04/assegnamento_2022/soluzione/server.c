#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>
#include "list.h"
#include "util.h"


#define BUF_SIZE 1000


int port_hub = 8000;

int main() {
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    LIST notifiche, tmp;
    itemType item;

    notifiche = NewList();

    // Socket opening
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
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
        printf("\n---\nattesa di nuova connessione\n");

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
        printf("Messaggio dal client: ");
        PrintItem(item);

        if (item.tipo_cliente == CLIENTE_LETTORE_AGGIUNTA) {
            printf("LETTORE: Iscrizione alla ricezione misure\n");
            
            while (strncmp(item.nome_sensore, "", STRNUM) != 0) {
                item.sockfd = newsockfd;
                notifiche = EnqueueLast(notifiche, item);
                printf("iscrizione a: ");
                PrintItem(item);

                if (recv(newsockfd, &item, sizeof (item), 0) == -1) {
                    perror("Error on receive");
                    exit(1);
                }
            }
        } else if (item.tipo_cliente == CLIENTE_LETTORE_RIMOZIONE) {
            printf("LETTORE: Disiscrizione dalle notifiche\n");
            notifiche = RemoveLettore(notifiche, item.nome_lettore);
        } else if (item.tipo_cliente == CLIENTE_SENSORE) {
            printf("SENSORE: pubblicazione misura\n");
            PrintItem(item);

            int answer = 0;
            tmp = notifiche;
            while (!isEmpty(tmp)) {
                if (strncmp(tmp->item.nome_sensore, item.nome_sensore, STRNUM) == 0) {
                    answer++;
                    strncpy(item.nome_lettore, tmp->item.nome_lettore, STRNUM);
                    printf("notifico misura al lettore sottoscritto (numero notifiche %d)\n  ", answer);
                    PrintItem(tmp->item);
                    if (send(tmp->item.sockfd, &item, sizeof (item), 0) == -1) {
                        perror("Error on receive");
                        exit(1);
                    }
                }
                tmp = tmp->next;
            }

            printf("messaggio ACK al sensore: notificati %d lettori\n", answer);
            if (send(newsockfd, &answer, sizeof (answer), 0) == -1) {
                perror("Error on receive");
                exit(1);
            }
            close(newsockfd);
        }
        
        printf("\nLista notifiche:\n");
        PrintList(notifiche);
    }

    notifiche = DeleteList(notifiche);

    close(sockfd);
    return 0;
}




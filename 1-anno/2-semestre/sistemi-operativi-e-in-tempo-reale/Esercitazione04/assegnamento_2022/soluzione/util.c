#include "util.h"

/*void PrintMisura(Misura misura) {*/
/*  printf("sensore \"%s\", tipo %d, value %d, contatore %d\n", misura.nome, misura.tipo, misura.valore, misura.contatore); */

/*}*/

void ForwardItem(char *host_name, int port, itemType item, int* answer) {
    struct sockaddr_in serv_addr;
    struct hostent* server;
    int sockfd;

    if ((server = gethostbyname(host_name)) == 0) {
        perror("Error resolving local host\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *) (server->h_addr))->s_addr;
    serv_addr.sin_port = htons(port);

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error opening socket\n");
        exit(1);
    }

    printf("%s,%d: apertura connesstione indirizzo \"%s\" porta %d\n", __FILE__, __LINE__, host_name, port);

    if (connect(sockfd, (void*) &serv_addr, sizeof (serv_addr)) == -1) {
        perror("Error connecting to socket\n");
        exit(1);
    }

    printf("%s,%d: inoltro item:\n", __FILE__, __LINE__);
    PrintItem(item);

    if (send(sockfd, &item, sizeof (item), 0) == -1) {
        perror("Error on send\n");
        exit(1);
    }

    printf("%s,%d: in attesa di risposta\n", __FILE__, __LINE__);

    if (recv(sockfd, answer, sizeof (int), 0) == -1) {
        perror("Error in receiving response from server\n");
        exit(1);
    }

    printf("%s,%d: risposta %d\n", __FILE__, __LINE__, *answer);

    close(sockfd);
}

LIST RemoveLettore(LIST l, char* nome_lettore) {
    LIST tmp, todel;
    int sockfd = -1;
    itemType item;
    
    while (!isEmpty(l) && strncmp(l->item.nome_lettore, nome_lettore, STRNUM) == 0) {
        todel = l;
        l = l -> next;
        printf("%s,%d: rimozione item: ", __FILE__, __LINE__);
        PrintItem(todel->item);
        sockfd = todel->item.sockfd;
        deleteNode(todel);
    }

    if (!isEmpty(l)) {
        tmp = l;
        while (!isEmpty(tmp->next)) {
            if (strncmp(tmp->next->item.nome_lettore, nome_lettore, STRNUM) == 0) {
                todel = tmp->next;
                tmp->next = todel->next;
                printf("%s,%d: rimozione item: ", __FILE__, __LINE__);
                PrintItem(todel->item);
                sockfd = todel->item.sockfd;
                deleteNode(todel);
            } else {
                tmp = tmp->next;
            }
        }
    }
    if (sockfd >= 0) {
        printf("%s,%d: chiusura connessione con lettore", __FILE__, __LINE__);

        strncpy(item.nome_sensore,"", STRNUM);
        strncpy(item.nome_lettore,"UNSUBSCRIBE", STRNUM);
        item.sockfd = -1;
        item.tipo_cliente = -1;
        item.valore = -1.0;
        if (send(sockfd, &item, sizeof (item), 0) == -1) {
            perror("Error on send\n");
            exit(1);
        }

        close(sockfd);
        //        if (send(sockfd, &item, sizeof (item), 0) == -1) {
        //            perror("Error on send\n");
        //            exit(1);
        //        }
    }
    return l;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libinetsocket.h"
#include "network.h"
#include "stdbool.h"
#include "utils.h"
#include "vec.h"
#include "string.h"
#include "network.h"
#include "stdint.h"
#include "arpa/inet.h"

#define MESSAGE_LEN_HEADER 2 // Number of bytes used to specify the message len
#define MAX_MESSAGE_LEN 4096 // in bytes

// Ok, so I need : a message header that contains the length of the message
// Then I read a bunch of bytes until I read the correct number
// Then that's my message : I can return it when the application code asks me if I read a new message



char *ActiveSocketReader_read(ActiveSocketReader *me, int pfd) {
    if (me->nbBytesRead < me->nbBytesToRead) {
        int ret = read(pfd, &me->currentMessage[me->nbBytesRead], me->nbBytesToRead - me->nbBytesRead);
        if (ret < 0) {
            perror(0);
            exit(1);
        }

        if (ret > 0) {
            printf("read : ");
            for (int i =0; i< ret; i++) {
                printf("%c ", me->currentMessage[me->nbBytesRead + i]);
            }
            printf("\n");    
        }
        me->nbBytesRead += ret;
    }
    if (me->nbBytesRead == me->nbBytesToRead) {
        return me->currentMessage;
    } else {
        return NULL;
    }
}

void ActiveSocketReader_reinit(ActiveSocketReader *me) {
    me->nbBytesRead = 0;
}

ActiveSocketReader NewActiveSocketReader(unsigned nbBytesToRead) {
    return (ActiveSocketReader){
        .currentMessage = malloc(nbBytesToRead*sizeof(char)),
        .nbBytesRead = 0,
        .nbBytesToRead = nbBytesToRead,
    };
}

ActiveSocket NewActiveSocket() {
    return (ActiveSocket){
        .pfd = -1,
        .currentMessageLen = 0,
        .latestMessage = EmptyVec(MAX_MESSAGE_LEN, sizeof(char)),
        .messageLenReader = NewActiveSocketReader(MESSAGE_LEN_HEADER),
        .messageReader = NewActiveSocketReader(MAX_MESSAGE_LEN),
    };
}

Vec *ActiveSocket_readMessage(ActiveSocket *me) {
    if (me->currentMessageLen <= 0) {
        char *messageLenBuf = ActiveSocketReader_read(&me->messageLenReader, me->pfd);
        if (messageLenBuf != NULL) {
            me->currentMessageLen = ntohs(*(uint16_t*)messageLenBuf);
            ActiveSocketReader_reinit(&me->messageLenReader);
            me->messageReader = NewActiveSocketReader(me->currentMessageLen);
        }
    }

    if (me->currentMessageLen > 0) {
        printf("CURRENT MESSAGE LEN %d\n", me->currentMessageLen);
        char *messageBuf = ActiveSocketReader_read(&me->messageReader, me->pfd);
        if (messageBuf != NULL) {
            printf("before write to vec : \n");
            for (int i = 0; i < me->currentMessageLen; i++) {
                printf("%c ", messageBuf[i]);
            }
            printf("\n");

            // WARNING : On ne check pas la capacité du vecteur ici !
            memcpy(me->latestMessage.contents, messageBuf, me->currentMessageLen);
            me->latestMessage.size = me->currentMessageLen;
            ActiveSocketReader_reinit(&me->messageReader);
            me->currentMessageLen = 0;
            return &me->latestMessage;
        }
    }

    return NULL;
}

Server NewServer() {
    return (Server){
        .sfd = -1,
        .super = NewActiveSocket(),
    };
}

void Server_initAndWaitForConnection(Server *me, char *address, char *port) {
    me->sfd = create_inet_server_socket(address, port, LIBSOCKET_TCP, LIBSOCKET_IPv4, SOCK_NONBLOCK);
    if (me->sfd < 0) {
        perror(0);
        exit(1);
    }

    me->super.pfd = - 1;


    // Block until a connection is made.
    while (me->super.pfd < 0) {
        me->super.pfd = accept_inet_stream_socket(me->sfd, 0, 0, 0, 0, 0, 0);
    }

    // if (me->super.pfd < 0) {
    //     perror(0);
    //     exit(1);
    // }
}

void Server_closeConnection(Server *me) {
    int ret = destroy_inet_socket(me->sfd);
    if (ret < 0) {
        perror(0);
        exit(1);
    }
}

void ActiveSocket_closeConnection(ActiveSocket *me) {
    int ret = destroy_inet_socket(me->pfd);
    if (ret < 0) {
        perror(0);
        exit(1);
    }
}

void ActiveSocket_connectToServer(ActiveSocket *me, char *address, char *port) {
    me->pfd = create_inet_stream_socket(address, port, LIBSOCKET_IPv4, 0);

    if (me->pfd < 0) {
        perror(0);
        exit(1);
    }
}

void ActiveSocket_write(ActiveSocket *me, char *message, uint16_t messageLen) {
    uint16_t networkMessageLen = htons(messageLen);
    write(me->pfd, &networkMessageLen, MESSAGE_LEN_HEADER);
    write(me->pfd, message, messageLen);
}

void ActiveSocket_writeStr(ActiveSocket *me, char *message) {
    ActiveSocket_write(me, message, strlen(message));
}
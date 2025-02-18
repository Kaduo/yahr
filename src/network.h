#pragma once
#include "vec.h"
#include "stdint.h"

typedef struct ActiveSocketReader {
    char *currentMessage;
    unsigned nbBytesToRead;
    unsigned nbBytesRead;
} ActiveSocketReader;


typedef struct ActiveSocket {
    int pfd; // Peer file descriptor
    ActiveSocketReader messageLenReader;
    ActiveSocketReader messageReader;
    Vec latestMessage;
    uint16_t currentMessageLen;
} ActiveSocket;

typedef struct Server {
    ActiveSocket super;
    int sfd; // Server file descriptor
} Server;


ActiveSocket NewActiveSocket();
Server NewServer();
Vec *ActiveSocket_readMessage(ActiveSocket *me);
void Server_initAndWaitForConnection(Server *me, char *address, char *port);
void Server_closeConnection(Server *me);
void ActiveSocket_closeConnection(ActiveSocket *me);
void ActiveSocket_connectToServer(ActiveSocket *me, char *address, char *port);
void ActiveSocket_write(ActiveSocket *me, char *message, uint16_t messageLen);
void ActiveSocket_writeStr(ActiveSocket *me, char *message);
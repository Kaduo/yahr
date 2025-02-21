#pragma once
#include "input.h"
#include "haptic.h"
#include "rodfactory.h"

typedef struct Session {
    InputService *inputService;
    HapticService *hapticService;
    Rods *rods;
} Session;


// Une par user.
// TODO : trouver un meilleur nom.
typedef struct SessionList {
    Session session;
    char *sessionFolderPath; // path to the directory that contains the .rods files, must end with "/"
    char *userFolderPath;
    int sessionId;
    int userId;
    int nbSessions;
} SessionList;

void Session_saveToFile(Session *me, FILE *file);
void Session_loadFromFile(Session *me, FILE* file);
void Session_loadFromFileName(Session *me, char *fileName);
void Session_saveToFileName(Session *me, char *fileName);
Session NewSession(InputService *inputService, HapticService *hapticService);

SessionList NewSessionList(char *sessionFolderPath, char *saveFolderPath, int userId, int nbSessions, InputService *inputService, HapticService *hapticService);
void SessionList_loadCurrentSession(SessionList *me);
void SessionList_saveCurrentSession(SessionList *me);
bool SessionList_loadNextSession(SessionList *me);
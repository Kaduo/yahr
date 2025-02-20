#include "input.h"
#include "haptic.h"
#include "rodfactory.h"
#include "serialize.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raylib.h"
#include "session.h"


// Interface que je veux :
// Charger une session (à partir d'un nom de fichier)
// Possibilité de sauvegarder une session
// Possibilité de sauvegarder les inputs

void Session_loadFromFile(Session *me, char *fileName) {
    if (me->rods != NULL) {
        FreeRods(me->rods);
    }
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror(0);
        exit(1);
    }
    
    me->rods = ReadRods(file);

    if (fclose(file) != 0) {
        perror(0);
        exit(1);
    }

    SetRodServices(me->rods, me->inputService, me->hapticService);
}

void Session_saveToFile(Session *me, char *fileName) {
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        perror(0);
        exit(1);
    }
    WriteRods(file, me->rods);
    if (fclose(file) != 0) {
        perror(0);
        exit(1);
    }
}

Session NewSession(InputService *inputService, HapticService *hapticService) {
    return (Session){
        .inputService = inputService,
        .hapticService = hapticService,
        .rods = NULL,
    };
}

// Il me faut une liste de noms de fichiers. L'option de demander un nom d'utilisateurs aussi. Ou bien je l'incrémente ?
// Je le lis dans un fichier et je l'incrémente automatiquement ?

// Je veux une fonction qui me prend en entrée une base, ajoute l'user_id, le session_id, et m'enregistre tout ça.

const char *SESSION_FILE_EXTENSION = ".rods";


// Une par user.
// TODO : trouver un meilleur nom.
typedef struct SessionList {
    Session session;
    char *sessionFolderPath; // path to the directory that contains the .rods files, must end with "/"
    char *userFolderPath;
    int sessionId;
    unsigned userId;
    unsigned nbSessions;
} SessionList;

const unsigned MAX_PATH_LEN = 50;


char *ConstructUserFolderPath(char *baseSaveFolderPath, unsigned userId, char *buf) {
    buf[0] = 0;

    char strUserId[3];
    sprintf(strUserId, "%d", userId);

    strcat(strcat(strcat(strcat(buf, baseSaveFolderPath), "user_"), strUserId), "/");
    return buf;
}

char *ConstructSessionPath(char *parentFolderPath, int sessionId, char *buf) {
    buf[0] = 0;

    char strSessionId[3];
    sprintf(strSessionId, "%d", sessionId);
    strcat(strcat(strcat(strcat(buf, parentFolderPath), "session_"), strSessionId), SESSION_FILE_EXTENSION);

    return buf;
}

SessionList NewSessionList(char *sessionFolderPath, char *saveFolderPath, unsigned userId, unsigned nbSessions, InputService *inputService, HapticService *hapticService) {

    SessionList sessionList = (SessionList){
        .session = NewSession(inputService, hapticService),
        .sessionFolderPath = sessionFolderPath,
        .userId = userId,
        .nbSessions = nbSessions,
        .sessionId = -1,
        .userFolderPath = ConstructUserFolderPath(saveFolderPath, userId, calloc(sizeof(char), MAX_PATH_LEN)),
    };

    if (MakeDirectory(sessionList.userFolderPath) != 0) {
        perror(0);
        exit(1);
    }
    return sessionList;
}

void SessionList_loadCurrentSession(SessionList *me) {
    char sessionPath[MAX_PATH_LEN];
    ConstructSessionPath(me->sessionFolderPath, me->sessionId, sessionPath);
    Session_loadFromFile(&me->session, sessionPath);
}

void SessionList_saveCurrentSession(SessionList *me) {
    char savePath[MAX_PATH_LEN];
    ConstructSessionPath(me->userFolderPath, me->sessionId, savePath);
    Session_saveToFile(&me->session, savePath);
}

bool SessionList_loadNextSession(SessionList *me) {
    
    if (me->sessionId >= me->nbSessions - 1) {
        return false;
    }


    me->sessionId++;
    SessionList_loadCurrentSession(me);
    return true;
}
#include "input.h"
#include "haptic.h"
#include "rodfactory.h"
#include "serialize.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raylib.h"
#include "session.h"
#include "stdbool.h"


// Interface que je veux :
// Charger une session (à partir d'un nom de fichier)
// Possibilité de sauvegarder une session
// Possibilité de sauvegarder les inputs




void Session_loadFromFile(Session *me, FILE* file) {
    if (me->rods != NULL) {
        FreeRods(me->rods);
    }
    
    me->rods = ReadRods(file);

    if (fclose(file) != 0) {
        perror(0);
        exit(1);
    }

    SetRodServices(me->rods, me->inputService, me->hapticService);
}

void Session_loadFromFileName(Session *me, char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror(0);
        printf("HMMMM\n");
        printf("HMMM %s\n", fileName);
        exit(1);
    }
    printf("oh no wait\n");
    Session_loadFromFile(me, file);
}

void Session_saveToFile(Session *me, FILE *file) {
    WriteRods(file, me->rods);
}

void Session_saveToFileName(Session *me, char *fileName) {
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        perror(0);
        exit(1);
    }

    Session_saveToFile(me, file);

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



const unsigned MAX_PATH_LEN = 100;


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

SessionList NewSessionList(char *sessionFolderPath, char *saveFolderPath, int userId, int nbSessions, InputService *inputService, HapticService *hapticService) {

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
    printf("CURRENT SESSION FILENAME : %s\n", sessionPath);
    Session_loadFromFileName(&me->session, sessionPath);
    printf("success loading no ?\n");
}

void SessionList_saveCurrentSession(SessionList *me) {
    char savePath[MAX_PATH_LEN];
    ConstructSessionPath(me->userFolderPath, me->sessionId, savePath);
    Session_saveToFileName(&me->session, savePath);
}

bool SessionList_loadNextSession(SessionList *me) {
    
    if (me->sessionId >= me->nbSessions - 1) {
        printf("whaa! %d %d\n", me->sessionId, me->nbSessions - 1);
        return false;
    }


    me->sessionId++;
    SessionList_loadCurrentSession(me);
    return true;
}
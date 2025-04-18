#pragma once
#include "raylib.h"

#include "stdio.h"
#include "tap.h"
// TODO : add HistoryInputService
// TODO : add option to consume inputs

typedef struct InputService InputService;
struct InputService {
    void (*poll)(InputService *me);
    // HashMap inputStatus;
    Vector2 (*getMousePosition)(InputService *me);
    bool (*isMouseButtonPressed)(InputService *me, MouseButton button);
    bool (*isMouseButtonReleased)(InputService *me, MouseButton button);
    bool (*isMouseButtonUp)(InputService *me, MouseButton button);
    bool (*isMouseButtonDown)(InputService *me, MouseButton button);
    Vector2 (*getMouseDelta)(InputService *me);
};

typedef enum ButtonStatus {
    PRESSED,
    RELEASED,
    DOWN,
    UP
} ButtonStatus;

ButtonStatus GetButtonStatus(InputService *me, MouseButton button);

InputService NewPhysicalInputService(void);


typedef struct WriteTapInputService {
    InputService super;
    InputService inner;
    FILE *save;
    bool isClosed;
} WriteTapInputService;

typedef struct ReadTapInputService ReadTapInputService;

struct ReadTapInputService {
    InputService super;
    FILE *save;
    bool leftMouseButtonDown;
    Tap futureTap;
    Tap currentTap;
    Tap oldTap;
    float startTime;
};

WriteTapInputService NewWriteTapInputService(InputService inner, char *filename);
void WriteTapInputServiceCloseSave(WriteTapInputService *me);
void WriteTapInputServiceOpenSave(WriteTapInputService *me, char *filename);
ReadTapInputService NewReadTapInputService(char *filename);
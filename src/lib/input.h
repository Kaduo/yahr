#pragma once
#include "raylib.h"
#include "hashmap.h"
#include "stdio.h"

typedef struct InputService InputService;

// TODO : add HistoryInputService
// TODO : add option to consume inputs
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
} WriteTapInputService;

typedef struct ReadTapInputService ReadTapInputService;

struct ReadTapInputService {
    InputService super;

};
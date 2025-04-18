#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include "string.h"
#include "stdlib.h"
#include "serialize.h"
#include "time.h"

// TODO : add HistoryInputService
// TODO : add option to consume inputs

const InputService NullInputService = {0};

ButtonStatus GetButtonStatus(InputService *me, MouseButton button) {
    if (me->isMouseButtonPressed(me, button)) {
        return PRESSED;
    }
    if (me->isMouseButtonReleased(me, button)) {
        return RELEASED;
    }
    if (me->isMouseButtonDown(me, button)) {
        return DOWN;
    }
    if (me->isMouseButtonUp(me, button)) {
        return UP;
    }

    fprintf(stderr, "WHY ???");
    return UP;
}

// void PhysicalInputService_poll(InputService *me) {
//     HashMap_clear(&me->inputStatus);
//     HashMap_insert(&me->inputStatus, "LeftMouseButtonStatus", );
// }

void PhysicalInputService_poll(InputService *me) {}

Vector2 PhysicalInputService_getMousePosition(InputService *me) {
    return GetMousePosition();
}

bool PhysicalInputService_isMouseButtonPressed(InputService *me, MouseButton button) {
    return IsMouseButtonPressed(button);
}

bool PhysicalInputService_isMouseButtonReleased(InputService *me, MouseButton button) {
    return IsMouseButtonReleased(button);
}


bool PhysicalInputService_isMouseButtonDown(InputService *me, MouseButton button) {
    return IsMouseButtonDown(button);
}


bool PhysicalInputService_isMouseButtonUp(InputService *me, MouseButton button) {
    return IsMouseButtonUp(button);
}

Vector2 PhysicalInputService_getMouseDelta(InputService *me) {
    return GetMouseDelta();
}


void PhysicalInputService_Construct(InputService *me) {
    me->poll = PhysicalInputService_poll;
    me->getMousePosition = PhysicalInputService_getMousePosition;
    me->isMouseButtonReleased = PhysicalInputService_isMouseButtonReleased;
    me->isMouseButtonPressed = PhysicalInputService_isMouseButtonPressed;
    me->isMouseButtonUp = PhysicalInputService_isMouseButtonUp;
    me->isMouseButtonDown= PhysicalInputService_isMouseButtonDown;
    me->getMouseDelta = PhysicalInputService_getMouseDelta;
}

InputService NewPhysicalInputService(void) {
    InputService physicalInputService;
    PhysicalInputService_Construct(&physicalInputService);
    return physicalInputService;
}

void WriteTapInputService_poll(InputService *_me) {
    WriteTapInputService *me = (WriteTapInputService*)_me;
    Tap tap = {0};
    bool writeTap = false;
    me->inner.poll(&me->inner);
    if (me->inner.isMouseButtonReleased(&me->inner, MOUSE_BUTTON_LEFT)
    || me->inner.isMouseButtonPressed(&me->inner, MOUSE_BUTTON_LEFT)) {
        writeTap = true;
        tap.flipped = true;
    } else {
        writeTap = me->inner.isMouseButtonDown(&me->inner, MOUSE_BUTTON_LEFT);
    }
    tap.mousePosition = me->inner.getMousePosition(&me->inner);
    tap.time = GetTime();
    if (writeTap) {
        WriteTap(me->save, tap);
    }
    fflush(me->save);
}


bool WriteTapInputService_isMouseButtonPressed(InputService *_me, MouseButton button) {
    WriteTapInputService *me = (WriteTapInputService*)_me;
    return me->inner.isMouseButtonPressed(&me->inner, button);
}

bool WriteTapInputService_isMouseButtonReleased(InputService *_me, MouseButton button) {
    WriteTapInputService *me = (WriteTapInputService*)_me;

    return me->inner.isMouseButtonReleased(&me->inner, button);
}


bool WriteTapInputService_isMouseButtonDown(InputService *_me, MouseButton button) {
    WriteTapInputService *me = (WriteTapInputService*)_me;

    return me->inner.isMouseButtonDown(&me->inner, button);
}


bool WriteTapInputService_isMouseButtonUp(InputService *_me, MouseButton button) {
    WriteTapInputService *me = (WriteTapInputService*)_me;

    return me->inner.isMouseButtonUp(&me->inner, button);
}

Vector2 WriteTapInputService_getMousePosition(InputService *_me) {
    WriteTapInputService *me = (WriteTapInputService*)_me;
    return me->inner.getMousePosition(&me->inner);
}

Vector2 WriteTapInputService_getMouseDelta(InputService *_me) {
    WriteTapInputService *me = (WriteTapInputService*)_me;

    return me->inner.getMouseDelta(&me->inner);
}

void WriteTapInputService_Construct(WriteTapInputService *me) {
    me->super.poll = WriteTapInputService_poll;
    me->super.getMousePosition = WriteTapInputService_getMousePosition;
    me->super.isMouseButtonReleased = WriteTapInputService_isMouseButtonReleased;
    me->super.isMouseButtonPressed = WriteTapInputService_isMouseButtonPressed;
    me->super.isMouseButtonUp = WriteTapInputService_isMouseButtonUp;
    me->super.isMouseButtonDown= WriteTapInputService_isMouseButtonDown;
    me->super.getMouseDelta = WriteTapInputService_getMouseDelta;
}


WriteTapInputService NewWriteTapInputService(InputService inner, char *filename) {

    WriteTapInputService writeTapInputService = {0};
    FILE *save = fopen(filename, "w");
    if (save != NULL) {
        writeTapInputService.save = save;
    } else {
        perror(0);
        exit(1);
    }
    writeTapInputService.inner = inner;
    WriteTapInputService_Construct(&writeTapInputService);
    return writeTapInputService;
}

void WriteTapInputServiceCloseSave(WriteTapInputService *me) {
    if (!me->isClosed) {
        fprintf(me->save, "endsave\n");
        fclose(me->save);
        me->isClosed = true;
    }
}

void WriteTapInputServiceOpenSave(WriteTapInputService *me, char *filename) {
    WriteTapInputServiceCloseSave(me);
    FILE *save = fopen(filename, "w");
    if (save != NULL) {
        me->save = save;
        fprintf(me->save, "%u\n", (unsigned)time(NULL));
        me->isClosed = false;
    } else {
        perror(0);
        exit(1);
    }
}


void ReadTapInputService_poll(InputService *_me) {
    ReadTapInputService *me = (ReadTapInputService*)_me;
    if (feof(me->save)) {
        printf("FINITO\n");
    }

    if (GetTime() + me->startTime > me->futureTap.time) {
        me->oldTap = me->currentTap;
        me->currentTap = me->futureTap;
        me->futureTap = ReadTap(me->save);
        if (me->currentTap.flipped) {
            me->leftMouseButtonDown = !me->leftMouseButtonDown;
            printf("flipped!\n");
        }
    }
}

bool ReadTapInputService_isMouseButtonPressed(InputService *_me, MouseButton button) {
    ReadTapInputService *me = (ReadTapInputService*)_me;
    if (button == MOUSE_BUTTON_LEFT) {
        return me->leftMouseButtonDown && me->currentTap.flipped;
    } else {
        return false;
    }
}

bool ReadTapInputService_isMouseButtonReleased(InputService *_me, MouseButton button) {
    ReadTapInputService *me = (ReadTapInputService*)_me;

    if (button == MOUSE_BUTTON_LEFT) {
        return !me->leftMouseButtonDown && me->currentTap.flipped;
    } else {
        return false;
    }
}


bool ReadTapInputService_isMouseButtonDown(InputService *_me, MouseButton button) {
    ReadTapInputService *me = (ReadTapInputService*)_me;
    if (button == MOUSE_BUTTON_LEFT) {
        return me->leftMouseButtonDown;
    } else {
        return false;
    }

}


bool ReadTapInputService_isMouseButtonUp(InputService *_me, MouseButton button) {
    ReadTapInputService *me = (ReadTapInputService*)_me;
    if (button == MOUSE_BUTTON_LEFT) {
        return !me->leftMouseButtonDown;
    } else {
        return false;
    }
}

Vector2 ReadTapInputService_getMousePosition(InputService *_me) {
    ReadTapInputService *me = (ReadTapInputService*)_me;
    return me->currentTap.mousePosition;
}

Vector2 ReadTapInputService_getMouseDelta(InputService *_me) {
    ReadTapInputService *me = (ReadTapInputService*)_me;
    return Vector2Subtract(me->currentTap.mousePosition, me->oldTap.mousePosition);
}

void ReadTapInputService_Construct(ReadTapInputService *me) {
    me->super.poll = ReadTapInputService_poll;
    me->super.getMousePosition = ReadTapInputService_getMousePosition;
    me->super.isMouseButtonReleased = ReadTapInputService_isMouseButtonReleased;
    me->super.isMouseButtonPressed = ReadTapInputService_isMouseButtonPressed;
    me->super.isMouseButtonUp = ReadTapInputService_isMouseButtonUp;
    me->super.isMouseButtonDown= ReadTapInputService_isMouseButtonDown;
    me->super.getMouseDelta = ReadTapInputService_getMouseDelta;
}

ReadTapInputService NewReadTapInputService(char *filename) {
    FILE *save = fopen(filename, "r");
    if (save != NULL) {
        char con[50];
        ReadStringTillSpace(save, con); // Skip absolute time at start;
        // Tap startTap = ReadTap(save);
        Tap startFutureTap = ReadTap(save);
        ReadTapInputService readTapInputService = {
            .futureTap = startFutureTap,
            .leftMouseButtonDown = false,
            .currentTap = {0},
            .oldTap = {0},
            .save = save,
            .startTime = startFutureTap.time,
        };
        ReadTapInputService_Construct(&readTapInputService);
        return  readTapInputService;
    } else {
        perror(0);
        exit(1);
    }
}
#include "raylib.h"
#include "stdbool.h"
#include "stdlib.h"
#include "input.h"
#include "stdio.h"
#include "serialize.h"


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
    tap.frameTime = GetFrameTime();
    if (writeTap) {
        WriteTap(me->save, tap);
    }
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

Vector2 WriteTapInputService_getMouseDelta(InputService *_me) {
    WriteTapInputService *me = (WriteTapInputService*)_me;

    return me->inner.getMouseDelta(&me->inner);
}

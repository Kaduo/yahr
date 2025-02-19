#pragma once
#include "stdint.h"
#include "vec.h"
#include "stdbool.h"
#include "ringbuffer.h"
#include "stdio.h"
#include "input.h"

typedef enum SignalShape {
    SignalShape_STEADY = 1,
    SignalShape_SINE = 2,
    SignalShape_TRIANGLE = 3,
    SignalShape_FRONT_TEETH = 4,
    SignalShape_BACK_TEETH = 5
} SignalShape;


typedef struct Signal {
    SignalShape signalShape;
    uint16_t period;
    uint8_t amplitude;
    uint8_t duty;
    uint8_t offset;
    uint16_t phase;
    uint8_t angle; // maxes out at 128, no idea why... After 128, seems like it doesn't care about the "global angle"
    uint8_t pulses; // maxes out at 128, again don't know why, duration of the signal kinda. I guess the unit is the number of periods?
    float duration; // mine
} Signal;


typedef struct HapticDriver HapticDriver;

struct HapticDriver {
    void (*setSignal)(HapticDriver *me, Signal signal);
    void (*clearSignal)(HapticDriver *me);
    void (*setDirection)(HapticDriver *me, uint8_t angle, uint16_t speed);
    InputService *inputService; // Needs to know the speed and direction of the mouse.
};


typedef struct PhysicalHapticDriver {
    HapticDriver super;
    int fd;
} PhysicalHapticDriver;

PhysicalHapticDriver NewPhysicalHapticDriver(InputService *inputService);
typedef struct HapticService HapticService;

struct HapticService {
    HapticDriver *driver;
    RingBuffer signalQueue;
    float duration;
    bool currentlyPlaying;
    bool shouldPlay;
};

void HapticService_update(HapticService *me, float frameTime);
void HapticService_playSignal(HapticService *me, Signal signal);
void HapticService_playQueue(HapticService *me);
void HapticService_addSignalToQueue(HapticService *me, Signal signal);
void HapticService_popQueue(HapticService *me);
void HapticService_clearAndPauseQueue(HapticService *me);
void HapticService_pauseQueue(HapticService *me);



void PrintSignal(Signal sig);

void MockHapticDriver_setSignal(HapticDriver *me, Signal signal);
void MockHapticDriver_clearSignal(HapticDriver *me);

HapticService NewHapticService(HapticDriver *driver);
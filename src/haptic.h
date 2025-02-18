#pragma once
#include "stdint.h"
#include "vec.h"
#include "stdbool.h"
#include "ringbuffer.h"
#include "stdio.h"

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
    uint8_t angle;
    uint8_t pulses;
    float duration;
} Signal;


typedef struct HapticDriver HapticDriver;

struct HapticDriver {
    void (*setSignal)(HapticDriver *me, Signal signal);
    void (*clearSignal)(HapticDriver *me);
};


typedef struct PhysicalHapticDriver {
    HapticDriver super;
    int fd;
} PhysicalHapticDriver;


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
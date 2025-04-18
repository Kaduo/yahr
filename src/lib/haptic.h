#pragma once
#include "stdint.h"
#include "ringbuffer.h"
#include "input.h"


typedef struct HapticService HapticService;
typedef struct Signal Signal;



typedef enum SignalShape {
    SignalShape_NO_SIGNAL = 0,
    SignalShape_STEADY = 1,
    SignalShape_SINE = 2,
    SignalShape_TRIANGLE = 3,
    SignalShape_FRONT_TEETH = 4,
    SignalShape_BACK_TEETH = 5
} SignalShape;


struct Signal {
    SignalShape signalShape;
    uint16_t period; // 0 is a bugged value and therefore should never be used
    uint8_t amplitude;
    uint8_t duty;
    uint8_t offset;
    uint16_t phase;
    uint8_t angle; // For values greater than 128, it's an isotropic (?) signal (doesn't care about the direction of the finger's movements)
    uint8_t pulses; // Number of periods that the signal plays out before stopping.  For values greater than 128, the signal lasts indefinitely
    float duration; // mine
};


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

HapticDriver NewMockHapticDriver(InputService *inputService);

HapticService NewHapticService(HapticDriver *driver);
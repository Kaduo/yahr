#include "consts.h"
#include "raylib.h"
#include "haptic.h"
#include "math.h"


const Color ROD_COLOR_FOR_LENGTH[10] = {
                                        WHITE,                  // WHITE
                                        {253, 46, 99, 255},     // RED 
                                        {41, 222, 182, 255},    // LIGHT GREEN
                                        {253, 74, 185, 255},    // FUSCHIA
                                        {238, 211, 121, 255},   // JAUNE
                                        {17, 163, 155, 255},    // DARK GREEN
                                        {62, 64, 69, 255},      // BLACK
                                        {177,82,75, 255},       // BROWN
                                        {50, 144, 202, 255},    // BLUE
                                        {216, 157, 118, 255}    // ORANGE
                                    };
const float ROD_WIDTH_FOR_LENGTH[NB_ROD_LENGTHS] = {
                                            1 * UNIT_ROD_HEIGHT,
                                            2 * UNIT_ROD_HEIGHT,
                                            3 * UNIT_ROD_HEIGHT,
                                            4 * UNIT_ROD_HEIGHT,
                                            5 * UNIT_ROD_HEIGHT,
                                            6 * UNIT_ROD_HEIGHT,
                                            7 * UNIT_ROD_HEIGHT,
                                            8 * UNIT_ROD_HEIGHT,
                                            9 * UNIT_ROD_HEIGHT,
                                            10 * UNIT_ROD_HEIGHT,
                                        };

typedef enum RodGroup {
    ROD_GROUP_1_7,
    ROD_GROUP_2_4_8,
    ROD_GROUP_3_6_9,
    ROD_GROUP_5_10
} RodGroup;

const RodGroup ROD_GROUP_FOR_LENGTH[NB_ROD_LENGTHS] = {
    ROD_GROUP_1_7,
    ROD_GROUP_2_4_8,
    ROD_GROUP_3_6_9,
    ROD_GROUP_2_4_8,
    ROD_GROUP_5_10,
    ROD_GROUP_3_6_9,
    ROD_GROUP_1_7,
    ROD_GROUP_2_4_8,
    ROD_GROUP_3_6_9,
    ROD_GROUP_5_10
};

uint8_t NormalizeBikeShedSqrtLog(int l, float min, float max) {
    printf("blablabla %f\n",(max-min)/(sqrt(-log(1./10.))));
    printf("hophop %f\n", sqrt(-log((l+1)/10))*(max-min)/(sqrt(-log(1./10))) + min);
    return (uint8_t)(sqrt(-log((l+1.)/10.))*(max-min)/(sqrt(-log(1./10.))) + min);
}

uint8_t ComputeOffset(int l, int maxAmplitude) {
    printf("amplitude : %f\n", (sqrt(-log((l+1.)/10.))*88.9 + 120.));
    // return 255 - (uint8_t)(sqrt(-log((l+1)/10.))*102. + 50.);
    return NormalizeBikeShedSqrtLog(l, 0, 255 - maxAmplitude);
}


uint8_t ComputeAmplitude(int l) {
    // printf("hmm %f\n", log(1./10.));
    // printf("amplitude : %f\n", (sqrt(-log((l+1.)/10.))*88.9 + 120.));
    // return (uint8_t)(sqrt(-log((l+1)/10.))*102. + 100.);
    // return 255 - ComputeOffset(l);
    return 130;
}



uint16_t ComputePeriod(RodGroup rodGroup) {
    switch (rodGroup) {
        case ROD_GROUP_1_7:
            return 4;
            break;
        case ROD_GROUP_2_4_8:
            return 28;
            break;
        case ROD_GROUP_3_6_9:
            return 70;
            break;
        case ROD_GROUP_5_10:
            return 200;
            break;  
    };
}

uint16_t ComputePhase(uint16_t period) {
    return (uint16_t)(0.9 * period);
}

const SignalShape ROD_SIGNAL_SHAPE = SignalShape_FRONT_TEETH;
const uint8_t ROD_SIGNAL_OFFSET = 0;
const uint8_t ROD_SIGNAL_DUTY = 0;
const uint8_t ROD_SIGNAL_PULSES = 128;
const uint8_t ROD_SIGNAL_ANGLE = 128;
const float ROD_SIGNAL_DURATION = 0.;

Signal RodSignalForLength(int l) {
    return (Signal){
        .amplitude = ComputeAmplitude(l),
        .angle = ROD_SIGNAL_ANGLE,
        .duration = ROD_SIGNAL_DURATION,
        .duty = ROD_SIGNAL_DUTY,
        .offset = ComputeOffset(l, ComputeAmplitude(0)),
        .period = ComputePeriod(ROD_GROUP_FOR_LENGTH[l]),
        .phase = ComputePhase(ComputePeriod(ROD_GROUP_FOR_LENGTH[l])),
        .pulses = ROD_SIGNAL_PULSES,
        .signalShape = ROD_SIGNAL_SHAPE,
    };
}

#ifndef DEBUG
const char *TABLET_IP = "192.168.1.24";
#else
const char *TABLET_IP = "localhost";
#endif

const char *TABLET_PORT = "44611";

const Signal IMPULSE_SIGNAL = {
    .amplitude = 255,
    .angle = 128,
    .duration = 0.0001,
    .duty = 0,
    .offset = 255,
    .period = 1,
    .pulses = 1,
    .signalShape = SignalShape_BACK_TEETH,
};


const Signal COLLISION_SIGNAL = {
    .amplitude = 0,
    .angle = 128,
    .duration = 0,
    .duty = 0,
    .offset = 0,
    .period = 1,
    .pulses = 128,
    .signalShape = SignalShape_STEADY,
};

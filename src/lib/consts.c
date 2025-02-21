#include "consts.h"
#include "raylib.h"
#include "haptic.h"
#include "math.h"


const Color ROD_COLOR_FOR_LENGTH[10] = {
                                        WHITE,                  // WHITE
                                        {253, 46, 99, 255},     // RED 
                                        {41, 222, 182, 255},    // LIGHT GREEN
                                        {253, 74, 185, 255},    // FUSCHIA,
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

uint8_t ComputeAmplitude(int l) {
    // printf("hmm %f\n", log(1./10.));
    printf("amplitude : %f\n", (sqrt(-log((l+1.)/10.))*114. + 100.));
    return (uint8_t)(sqrt(-log((l+1)/10.))*102. + 100.);
}

uint16_t ComputePeriod(RodGroup rodGroup) {
    switch (rodGroup) {
        case ROD_GROUP_1_7:
            return 10;
            break;
        case ROD_GROUP_2_4_8:
            return 25;
            break;
        case ROD_GROUP_3_6_9:
            return 50;
            break;
        case ROD_GROUP_5_10:
            return 100;
            break;  
    };
}

const SignalShape ROD_SIGNAL_SHAPE = SignalShape_SINE;
const uint8_t ROD_SIGNAL_OFFSET = 0;
const uint8_t ROD_SIGNAL_DUTY = 0;
const uint8_t ROD_SIGNAL_PULSES = 128;
const uint16_t ROD_SIGNAL_PHASE = 0;
const uint8_t ROD_SIGNAL_ANGLE = 128;
const float ROD_SIGNAL_DURATION = 0.;

Signal RodSignalForLength(int l) {
    return (Signal){
        .amplitude = ComputeAmplitude(l),
        .angle = ROD_SIGNAL_ANGLE,
        .duration = ROD_SIGNAL_DURATION,
        .duty = ROD_SIGNAL_DUTY,
        .offset = ROD_SIGNAL_OFFSET,
        .period = ComputePeriod(ROD_GROUP_FOR_LENGTH[l]),
        .phase = ROD_SIGNAL_PHASE,
        .pulses = ROD_SIGNAL_PULSES,
        .signalShape = ROD_SIGNAL_SHAPE,
    };
}

#ifndef DEBUG
const char *TABLET_IP = "192.168.1.24";
#else
const char *TABLET_IP = "localhost";
#endif
const char *TABLET_PORT = "34989";
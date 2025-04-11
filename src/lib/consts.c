#include "consts.h"
#include "rodsystem.h"
#include "stdlib.h"

const Color ROD_COLOR_FOR_LENGTH[10] = {
                                        WHITE,                  // WHITE
                                        {253, 46, 99, 255},     // RED 
                                        {41, 222, 182, 255},    // LIGHT GREEN
                                        {253, 74, 185, 255},    // MAUVE
                                        {238, 211, 121, 255},   // JAUNE
                                        {17, 163, 155, 255},    // DARK GREEN
                                        {62, 64, 69, 255},      // BLACK
                                        {177,82, 75, 255},       // BROWN
                                        {50, 144, 202, 255},    // BLUE
                                        {216, 157, 118, 255}    // ORANGE
                                    };


bool IsEqualColors(Color color1, Color color2) {

    return color1.a == color2.a && color1.g == color2.g && color1.b == color2.b && color1.r == color2.r;
}

char *GetColorName(Color color) {
    int i;
    for (i = 0; i < 10; i++) {
        if (IsEqualColors(ROD_COLOR_FOR_LENGTH[i], color)) {
            break;
        }
    }
    switch (i)
    {
    case 0:
        return "blanche";
        break;
    case 1:
        return "rouge";
        break;
    case 2:
        return "vert clair";
        break;
    case 3:
        return "mauve";
    case 4:
        return "jaune";
    case 5:
        return "vert foncé";
    case 6:
        return "noire";
    case 7:
        return "marron";
    case 8:
        return "bleue";
    case 9:
        return "orange";
    default:
        printf("WTFFFF  %d\n", color.g);
        exit(1);
        break;
    }
}

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
    return (uint8_t)(sqrt(-log((l+1.)/10.))*(max-min)/(sqrt(-log(1./10.))) + min);
}

uint8_t ComputeOffset(int l, int maxAmplitude) {
    l += 1; // Because l ranges from 0 to 9
    if (l == 1 || l == 2 || l == 3 || l == 5) {
        return 255 - maxAmplitude;
    } else if (l == 4 || l == 6 || l == 7 || l == 10) {
        return (255 - maxAmplitude)/2;
    } else {
        return 0;
    }
    // printf("amplitude : %f\n", (sqrt(-log((l+1.)/10.))*88.9 + 120.));
    // // return 255 - (uint8_t)(sqrt(-log((l+1)/10.))*102. + 50.);
    // return NormalizeBikeShedSqrtLog(l, 0, 255 - maxAmplitude);
}


uint8_t ComputeAmplitude(int l) {
    // printf("hmm %f\n", log(1./10.));
    // printf("amplitude : %f\n", (sqrt(-log((l+1.)/10.))*88.9 + 120.));
    // return (uint8_t)(sqrt(-log((l+1)/10.))*102. + 100.);
    // return 255 - ComputeOffset(l);
    return 201;
}



uint16_t ComputePeriod(RodGroup rodGroup) {
    switch (rodGroup) {
        case ROD_GROUP_1_7:
            return 4;
            break;
        case ROD_GROUP_2_4_8:
            return 20;
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
// const char *TABLET_IP = "192.168.1.18";
const char *TABLET_IP = "192.168.43.235";
// const char *TABLET_IP = "localhost";
#else
const char *TABLET_IP = "localhost";
#endif

const char *TABLET_PORT = "52123";

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

Rod2 GetRod2ForN(int n) {
    return (Rod2){
        .n = n,
        .rect = (Rectangle){.height = UNIT_ROD_HEIGHT, .width = UNIT_ROD_HEIGHT*n, .x =0, .y= 0},
        .signal = RodSignalForLength(n - 1),
        .color = ROD_COLOR_FOR_LENGTH[n - 1],
    };
}

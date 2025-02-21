/*******************************************************************************************
*
*   LayoutName v1.0.0 - Tool Description
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include "lib/haptic.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 1000;
    int screenHeight = 600;


    SetTraceLogLevel(LOG_ERROR);
    InitWindow(screenWidth, screenHeight, "layout_name");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);


    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    Vector2 anchor = { 200, 50 };

    Signal sig = (Signal){.signalShape = SignalShape_SINE};

    InputService inputService = NewPhysicalInputService();

    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver(&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);

    uint8_t globalAngle = 0;
    uint16_t globalSpeed = 100;
    hapticDriver.super.setDirection(&hapticDriver.super, globalAngle, globalSpeed);

    float amplitudeSliderValue = 0.0f;
    float periodSliderValue = 1.0f;
    float dutySliderValue = 0.0f;
    float offsetSliderValue = 0.0f;
    float phaseSliderValue = 0.0f;
    float angleSliderValue = 128.0f;
    float pulsesSliderValue = 128.0f;

    int signalToggleValue = 0;

    char amplitudeValueDisplay[10];
    char periodValueDisplay[10];
    char dutyValueDisplay[10];
    char offsetValueDisplay[10];
    char phaseValueDisplay[10];
    char angleValueDisplay[10];
    char pulsesValueDisplay[10];
    char globalSpeedValueDisplay[10];
    char globalAngleValueDisplay[10];

    const int SLIDER_WIDTH = 300;
    const int SLIDER_HEIGHT = 20;
    const int SLIDER_GAP = 10;

    const SignalShape signalShapes[] = {SignalShape_SINE, SignalShape_STEADY, SignalShape_FRONT_TEETH, SignalShape_BACK_TEETH, SignalShape_TRIANGLE};

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    ToggleFullscreen();
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update


        bool sigChanged = false;
        if ((unsigned)amplitudeSliderValue != sig.amplitude) {
            sig.amplitude = (unsigned)amplitudeSliderValue;
            sigChanged = true;
        }
        if ((unsigned)periodSliderValue != sig.period) {
            sig.period = (unsigned)periodSliderValue;
            sigChanged = true;
        }
        if ((unsigned)dutySliderValue != sig.duty) {
            sig.duty = (unsigned)dutySliderValue;
            sigChanged = true;
        }
        if ((unsigned)offsetSliderValue != sig.offset) {
            sig.offset = (unsigned)offsetSliderValue;
            sigChanged = true;
        }
        if ((unsigned)phaseSliderValue != sig.phase) {
            sig.phase = (unsigned)phaseSliderValue;
            sigChanged = true;
        }
        if ((unsigned)angleSliderValue != sig.angle) {
            sig.angle = (unsigned)angleSliderValue;
            sigChanged = true;
        }
        if ((unsigned)pulsesSliderValue != sig.pulses) {
            sig.pulses = (unsigned)pulsesSliderValue;
            sigChanged = true;
        }
        if (signalShapes[signalToggleValue] != sig.signalShape) {
            sig.signalShape = signalShapes[signalToggleValue];
            sigChanged = true;
        }

        if (sigChanged) {
            HapticService_playSignal(&hapticService, sig);
        }

        HapticService_update(&hapticService, GetFrameTime());

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 
            if (GetFrameTime() > 0) {
                float floatSpeed = Vector2Length(GetMouseDelta())/GetFrameTime();
                if (floatSpeed > 65535) {
                    globalSpeed = 65535;
                } else {
                    globalSpeed = (uint16_t)floatSpeed;
                }
            }
            sprintf(amplitudeValueDisplay, "%d", sig.amplitude);
            sprintf(periodValueDisplay, "%d", sig.period);
            sprintf(dutyValueDisplay, "%d", sig.duty);
            sprintf(offsetValueDisplay, "%d", sig.offset);
            sprintf(phaseValueDisplay, "%d", sig.phase);
            sprintf(angleValueDisplay, "%d", sig.angle);
            sprintf(pulsesValueDisplay, "%d", sig.pulses);
            sprintf(globalSpeedValueDisplay, "%d", globalSpeed);
            sprintf(globalAngleValueDisplay, "%d", globalAngle);
                    // raygui: controls drawing
            //----------------------------------------------------------------------------------
            GuiSlider((Rectangle){ anchor.x, anchor.y + 0*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "amplitude", amplitudeValueDisplay, &amplitudeSliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 1*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "period", periodValueDisplay, &periodSliderValue, 1, 200); // TODO FIXME ?
            GuiSlider((Rectangle){ anchor.x, anchor.y + 2*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "duty", dutyValueDisplay, &dutySliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 3*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "offset", offsetValueDisplay, &offsetSliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 4*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "phase", phaseValueDisplay, &phaseSliderValue, 0, 65535);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 5*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "angle", angleValueDisplay, &angleSliderValue, 0, 128);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 6*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "pulses", pulsesValueDisplay, &pulsesSliderValue, 0, 128);
            GuiToggleGroup((Rectangle){ anchor.x - 100, anchor.y + 8*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH * 0.8, 2*SLIDER_HEIGHT}, "SINE;STEADY;FRONT_TEETH;BACK_TEETH;TRIANGLE", &signalToggleValue);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------


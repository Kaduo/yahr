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

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "haptic.h"

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


    InitWindow(screenWidth, screenHeight, "layout_name");

    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);


    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    Vector2 anchor = { 200, 50 };

    Signal sig = (Signal){0};

    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver();
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);

    int8_t globalAngle = -1;
    int16_t globalSpeed = 100;
    hapticDriver.super.setDirection(&hapticDriver.super, globalAngle, globalSpeed);

    float amplitudeSliderValue = 0.0f;
    float periodSliderValue = 0.0f;
    float dutySliderValue = 0.0f;
    float offsetSliderValue = 0.0f;
    float phaseSliderValue = 0.0f;
    float angleSliderValue = 0.0f;
    float pulsesSliderValue = 0.0f;
    float globalSpeedSliderValue = 0.0f;
    float globalAngleSliderValue = 0.0f;
    //----------------------------------------------------------------------------------

    // float *SliderValues[] = {
    //     &amplitudeSliderValue,
    //     &periodSliderValue,
    //     &dutySliderValue,
    //     &offsetSliderValue,
    //     &phaseSliderValue,
    //     &angleSliderValue,
    //     &pulsesSliderValue,
    //     &globalSpeedSliderValue,
    //     &globalAngleSliderValue};

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        const int SLIDER_WIDTH = 300;
        const int SLIDER_HEIGHT = 20;
        const int SLIDER_GAP = 10;
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            GuiSlider((Rectangle){ anchor.x, anchor.y + 0*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "amplitude", NULL, &amplitudeSliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 1*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "period", NULL, &periodSliderValue, 0, 65535);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 2*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "duty", NULL, &dutySliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 3*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "offset", NULL, &offsetSliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 4*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "phase", NULL, &phaseSliderValue, 0, 65535);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 5*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "angle", NULL, &angleSliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 6*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "pulses", NULL, &pulsesSliderValue, 0, 255);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 7*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "global speed", NULL, &globalSpeedSliderValue, -32768, 32767);
            GuiSlider((Rectangle){ anchor.x, anchor.y + 8*(SLIDER_HEIGHT+SLIDER_GAP), SLIDER_WIDTH, SLIDER_HEIGHT }, "global angle", NULL, &globalAngleSliderValue, -128, 127);
            //----------------------------------------------------------------------------------

            bool globalDirChanged = false;
            if ((int)globalSpeedSliderValue != globalSpeed) {
                globalSpeed = (int)globalSpeedSliderValue;
                globalDirChanged = true;
            }
            if ((int)globalAngleSliderValue != globalAngle) {
                globalAngle = (int)globalAngleSliderValue;
                globalDirChanged = true;
            }

            if (globalDirChanged) {
                hapticDriver.super.setDirection(&hapticDriver.super, globalAngle, globalSpeed);
            }


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

            if (sigChanged) {
                HapticService_playSignal(&hapticService, sig);
            }

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


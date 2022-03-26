#include <3ds.h>
#include <citro2d.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

const char* color_name[] = {"Red", "Green", "Blue", "White", "Black"};
const float erase_size = 10.f;
const float erase_border = 1.f;
// Create colors
//clrRed, clrGreen, clrBlue, clrWhite, clrBlack with values saved in the order ABGR
const u32 colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFFFF, 0xFF000000};
#define clrRed colors[0]
#define clrGreen colors[1]
#define clrBlue colors[2]
#define clrWhite colors[3]
#define clrBlack colors[4]

float float_abs(float n) { return n < 0 ? -n : n; }

float get_thickness(float x1, float y1, float x2, float y2, float l) {
    float x;
    if (x1 > x2)
        x = x1 - x2;
    else
        x = x2 - x1;

    float y;
    if (y1 > y2)
        y = y1 - y2;
    else
        y = y2 - y1;

    float dist = sqrtf(powf(x, 2.f) + powf(y, 2.f));
    float cos_gamma =
        (powf(y, 2.f) + powf(dist, 2.f) - powf(x, 2.f)) / (2.f * y * dist);
    float sin_gamma = sqrtf(1.f - powf(cos_gamma, 2.f));
    return float_abs(l * sin_gamma) + float_abs(l * cos_gamma);
    // // double cos_gamma = (pow(x, 2) + pow(dist, 2) - pow(y, 2))/ (2*y*dist);
    // double sq_sin_gamma = 1 - pow(cos_gamma, 2);
}

void draw_erase(touchPosition* touch, float e_size, float e_border,
                u32 c_border, u32 c_background) {
    C2D_DrawRectSolid(touch->px - (e_size / 2.f), touch->py - (e_size / 2.f),
                      0.f, e_size, e_size, c_border);
    C2D_DrawRectSolid(touch->px - (e_size / 2.f) + e_border,
                      touch->py - (e_size / 2.f) + e_border, 0.f,
                      erase_size - (erase_border * 2.f),
                      erase_size - (erase_border * 2.f), c_background);
}

void draw(u32 kDown, C3D_RenderTarget* screen, u8 len,
          u8 clrBgInd) {
// defines
// #define clrWhite colors[3]
// #define clrBlack colors[4]
    // save background color in a variable
    u32 clrBackground = colors[clrBgInd];
    static u8 index = 0;
    static bool eraser = 0;
    static touchPosition last_touch = {0, 0};
    if (kDown & KEY_X) C2D_TargetClear(screen, clrBackground);
    if (kDown & KEY_RIGHT) {
        index = (index + 1) % len;
    } else if (kDown & KEY_LEFT) {
        if (index == 0x00) {
            index = len - 1;
        } else {
            index = index - 1;
        }
    }
    bool remove_erase_drawing = 0;
    if (kDown & KEY_B) {
        remove_erase_drawing =
            eraser && (last_touch.px != 0 || last_touch.py != 0);
        eraser = !eraser;
    }

    // prints
    printf("\x1b[1;1HSimple citro2d shapes example");
    printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime() * 6.0f);
    printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime() * 6.0f);
    printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage() * 100.0f);
    printf("\x1b[6;1HColor:   %s\x1b[K", color_name[index]);
    char* str;
    if (eraser) {
        str = "On";
    } else {
        str = "Off";
    }
    printf("\x1b[7;1HEraser:   %s\x1b[K", str);

    // Render the scene
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    // C2D_TargetClear(top, clrClear);
    C2D_SceneBegin(screen);

    // if touch = {0,0} user is not pressing touch screen
    touchPosition touch;
    hidTouchRead(&touch);
    if (touch.px != 0 || touch.py != 0) {
        if (last_touch.px != 0 || last_touch.py != 0) {
            if (eraser) {
                float thickness = get_thickness(last_touch.px, last_touch.py,
                                                touch.px, touch.py, erase_size);
                C2D_DrawLine(last_touch.px, last_touch.py, clrBackground,
                             touch.px, touch.py, clrBackground, thickness, 0);
                // draw_erase(&last_touch, erase_size, erase_border, clrWhite,
                //            clrWhite);
                C2D_DrawRectSolid(last_touch.px - (erase_size / 2.f),
                                  last_touch.py - (erase_size / 2.f), 0.f,
                                  erase_size, erase_size, clrWhite);
                draw_erase(&touch, erase_size, erase_border, clrBlack,
                           clrWhite);
            } else {
                if (remove_erase_drawing) {
                    C2D_DrawRectSolid(last_touch.px - (erase_size / 2.f),
                                      last_touch.py - (erase_size / 2.f), 0.f,
                                      erase_size, erase_size, clrWhite);
                }
                C2D_DrawLine(last_touch.px, last_touch.py, colors[index],
                             touch.px, touch.py, colors[index], 2.0, 0);
            }
            printf("\x1b[5;1HDrawing:  Y\x1b[K");
        }
        last_touch = touch;
    } else {
        if (eraser && (last_touch.px != 0 || last_touch.py != 0)) {
            // draw_erase(&last_touch, erase_size, erase_border, clrBackground,
            //            clrBackground);
            C2D_DrawRectSolid(last_touch.px - erase_size / 2.f,
                              last_touch.py - erase_size / 2.f, 0.f, erase_size,
                              erase_size, clrBackground);
        }
        last_touch.px = 0;
        last_touch.py = 0;
        printf("\x1b[5;1HDrawing:  N\x1b[K");
    }
// C3D_FrameEnd(0);
// #undef clrWhite
// #undef clrBlack
}

int main(int argc, char* argv[]) {
    //---------------------------------------------------------------------------------
    // Init libs
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_TOP, NULL);

    // Create screens
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Create colors
    // u32 transparent = C2D_Color32(0x00, 0x00, 0x00, 0x00);
    // u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
    // u32 clrGreen = C2D_Color32(0x00, 0xFF, 0x00, 0xFF);
    // u32 clrRed = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
    // u32 clrBlue = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);
    // u32 clrBlack = C2D_Color32(0x00, 0x00, 0x00, 0xFF);

#define clrBackground clrWhite
    // u32 colors[] = {clrRed, clrGreen, clrBlue, clrWhite, clrBlack};
    u8 len = sizeof(colors) / sizeof(u32);
    time_t max = 60;
    time_t begin = time(NULL);
    C2D_TargetClear(bottom, clrBackground);
    // Main loop
    while (aptMainLoop()) {
        hidScanInput();

        // TODO code to execute when time finished
        time_t timer = max - (time(NULL) - begin);
        printf("\x1b[8;1HTimer:    %lld\x1b[K", timer);
        if (timer < 1) {
            C2D_TargetClear(bottom, clrBackground);
            timer = max;
            begin = time(NULL);
        }

        // Respond to user input
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;  // break in order to return to hbmenu
        
        draw(kDown, bottom, len, 3);
        C3D_FrameEnd(0);
        C3D_FrameEnd(0);
    }

    // Deinit libs
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}

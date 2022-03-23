#include <3ds.h>
#include <citro2d.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

const char* color_name[] = {"Red", "Green", "Blue", "White", "Black"};

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
    C3D_RenderTarget* test = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Create colors
    u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
    u32 clrGreen = C2D_Color32(0x00, 0xFF, 0x00, 0xFF);
    u32 clrRed = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
    u32 clrBlue = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);
    u32 clrBlack = C2D_Color32(0x00, 0x00, 0x00, 0xFF);

	#define clrBackground clrWhite
	u32 colors[] = {clrRed, clrGreen, clrBlue, clrWhite, clrBlack};
	u8 len = sizeof(colors)/sizeof(u32);
	u8 index = 0;
	time_t max = 60;
	time_t begin = time(NULL);
    C2D_TargetClear(bottom, clrBackground);

	bool eraser = 0;

	//use 0,0 when there is no touch input in previous frame
    touchPosition last_touch = {0, 0};
    // Main loop
    while (aptMainLoop()) {
        hidScanInput();

        // Respond to user input
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;  // break in order to return to hbmenu
        if (kDown & KEY_X) C2D_TargetClear(bottom, clrBackground);
		if(kDown & KEY_RIGHT){
			index = (index+1)%len;
		}else if(kDown & KEY_LEFT){
			if(index == 0x00) {
				index = len -1;
			} else {
				index = index - 1;
			}
		}
		if(kDown & KEY_B) eraser = !eraser;

		//prints
        printf("\x1b[1;1HSimple citro2d shapes example");
        printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K",
               C3D_GetProcessingTime() * 6.0f);
        printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime() * 6.0f);
        printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K",
               C3D_GetCmdBufUsage() * 100.0f);
		printf("\x1b[6;1HColor:   %d\x1b[K", index);
		//printf("\x1b[6;1HColor:   %s\x1b[K", color_name[index]);

		char* str;
		if(eraser){
			str = "On";
		}else{
			str = "Off";
		}
		printf("\x1b[7;1HEraser:   %s\x1b[K", str);
		time_t timer = max - (time(NULL) - begin);
		//TODO code to execute when time finished
		if(timer<1){
			C2D_TargetClear(bottom, clrBackground);
			timer = max;
			begin = time(NULL);
		}
		printf("\x1b[8;1HTimer:    %lld\x1b[K", timer);
		
        // Render the scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        // C2D_TargetClear(top, clrClear);
        C2D_SceneBegin(bottom);

		//if touch = {0,0} user is not pressing touch screen
        touchPosition touch;
        hidTouchRead(&touch);

        if (touch.px != 0 || touch.py != 0) {
            if (last_touch.px != 0 || last_touch.py != 0) {
				u32 color;
				float thickness;
				if(eraser){
					color = clrBackground;
					thickness = 8.0;
				}else{
					color = colors[index];
					thickness = 2.0;
				}
                C2D_DrawLine(last_touch.px, last_touch.py, color, touch.px,
                             touch.py, color, thickness, 0);
                printf("\x1b[5;1HDrawing:  Y\x1b[K");
            } else {
                printf("\x1b[5;1HDrawing:  R\x1b[K");
            }
            last_touch = touch;
        } else {
            last_touch.px = 0;
            last_touch.py = 0;
            printf("\x1b[5;1HDrawing:  N\x1b[K");
        }
        C3D_FrameEnd(0);
        // gspWaitForVBlank();
    }

    // Deinit libs
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}

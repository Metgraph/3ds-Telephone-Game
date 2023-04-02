#include <3ds.h>
#include <citro2d.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "button.h"
#include "menu.h"
#include "draw.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240



Button buttons_list[3];
const u8 len_buttons = sizeof(buttons_list) / sizeof(Button);


static step act_step = MAIN_MENU;

static bool has_step_changed(step curr_step){
	static step old_step = UNDEFINED;
	bool ret = old_step != curr_step;
	old_step=curr_step;
	return ret;
}

void generate_buttons()
{
	set_buttons(buttons_list, 3, 30, 10, SCREEN_WIDTH, SCREEN_HEIGHT);
	buttons_list[0].text = "Unisciti";
	// buttons_list[0].func = set_step;
	buttons_list[1].text = "Crea";
}

int main(int argc, char *argv[])
{
	PrintConsole top_print, bottom_print;
	const char* main_menu_options[]={"option 1", "option 2", "option 3"};
	const step main_menu_actions[]={DRAWING, MAIN_MENU, MAIN_MENU};
	const u8 main_menu_len=sizeof(main_menu_options)/sizeof(char*);

	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	// consoleInit(GFX_TOP, NULL);
	consoleInit(GFX_TOP, &top_print);
	consoleInit(GFX_BOTTOM, &bottom_print);

	// act_step=DRAWING;

	// Get screen target
	C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	// C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget *top = NULL;
	// svcSetTimer
	time_t max = 60;
	time_t begin = time(NULL);

	// calculate buttons dimension
	buttons_list[0].h = buttonh(2, 30, 1, 0, SCREEN_HEIGHT);
	buttons_list[0].w = SCREEN_WIDTH - 40;
	C2D_TargetClear(bottom, clrBackground);

	set_screen(BOTTOMSCREEN, &bottom_print);
	// Main loop
	while (aptMainLoop())
	{
		// default screen for print
		consoleSelect(&top_print);
		hidScanInput();

		// TODO code to execute when time finished
		time_t timer = max - (time(NULL) - begin);
		printf("\x1b[8;1HTimer:    %lld\x1b[K", timer);
		if (timer < 1)
		{
			C2D_TargetClear(bottom, clrBackground);
			timer = max;
			begin = time(NULL);
		}

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		bool step_changed = has_step_changed(act_step);
		switch (act_step)
		{
		case MAIN_MENU:
			if(step_changed){

				set_menu_options(main_menu_options, main_menu_actions, main_menu_len, MAIN_MENU);
			}
			// generate_buttons();
			act_step=run_menu(kDown);
			break;

		case DRAWING:
			draw(kDown, bottom, 3);

		default:
			break;
		}

		C3D_FrameEnd(0);
		// C3D_FrameEnd(0);
	}

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}

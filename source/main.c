#include <3ds.h>
#include <citro2d.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "button.h"
#include "common.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define clrRed colors[0]
#define clrGreen colors[1]
#define clrBlue colors[2]
#define clrWhite colors[3]
#define clrBlack colors[4]
// define background color
#define clrBackground clrWhite

// global variables
const char *color_name[] = {"Red", "Green", "Blue", "White", "Black"};
const float erase_size = 10.f;
const float erase_border = 1.f;
// Create colors
// clrRed, clrGreen, clrBlue, clrWhite, clrBlack with values saved in the order
// ABGR
const u32 colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFFFF,
					  0xFF000000};

Button buttons_list[3];
const u8 len_buttons = sizeof(buttons_list) / sizeof(Button);


static u8 act_step = MENU;

float float_abs(float n) { return n < 0 ? -n : n; }

bool is_touching(touchPosition *touch) { return touch->px != 0 || touch->py != 0; }

float get_thickness(float x1, float y1, float x2, float y2, float l)
{
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

void draw_erase(touchPosition *touch, float e_size, float e_border,
				u32 c_border, u32 c_background)
{
	C2D_DrawRectSolid(touch->px - (e_size / 2.f), touch->py - (e_size / 2.f),
					  0.f, e_size, e_size, c_border);
	C2D_DrawRectSolid(touch->px - (e_size / 2.f) + e_border,
					  touch->py - (e_size / 2.f) + e_border, 0.f,
					  erase_size - (erase_border * 2.f),
					  erase_size - (erase_border * 2.f), c_background);
}

void draw(u32 kDown, C3D_RenderTarget *screen, u32 len, u32 clrBgInd)
{
	// save background color in a variable
	// u32 clrBackground = colors[clrBgInd];

	static u32 index = 0;
	static bool eraser = 0;
	static touchPosition last_touch = {0, 0};

	// key down related actions
	if (kDown & KEY_X)
		C2D_TargetClear(screen, clrBackground);
	if (kDown & KEY_RIGHT)
	{
		index = (index + 1) % len;
	}
	else if (kDown & KEY_LEFT)
	{
		if (index == 0x00)
		{
			index = len - 1;
		}
		else
		{
			index = index - 1;
		}
	}

	bool remove_erase_drawing = 0;
	if (kDown & KEY_B)
	{
		// check if is needed to clean eraser drawing
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
	char *str;
	if (eraser)
	{
		str = "On";
	}
	else
	{
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
	if (touch.px != 0 || touch.py != 0)
	{
		if (last_touch.px != 0 || last_touch.py != 0)
		{
			// eraser
			if (eraser)
			{
				// calculate the thickness so the line will fit the eraser
				// dimension
				float thickness = get_thickness(last_touch.px, last_touch.py,
												touch.px, touch.py, erase_size);
				// draw line between last_touch and touch
				C2D_DrawLine(last_touch.px, last_touch.py, clrBackground,
							 touch.px, touch.py, clrBackground, thickness, 0);
				// remove old eraser drawing
				C2D_DrawRectSolid(last_touch.px - (erase_size / 2.f),
								  last_touch.py - (erase_size / 2.f), 0.f,
								  erase_size, erase_size, clrWhite);
				// draw new eraser drawing
				draw_erase(&touch, erase_size, erase_border, clrBlack,
						   clrWhite);

				// pencil
			}
			else
			{
				// if needed clear the area of last touch because there is
				// eraser drawing to be removed
				if (remove_erase_drawing)
				{
					C2D_DrawRectSolid(last_touch.px - (erase_size / 2.f),
									  last_touch.py - (erase_size / 2.f), 0.f,
									  erase_size, erase_size, clrWhite);
				}
				// draw pencil
				C2D_DrawLine(last_touch.px, last_touch.py, colors[index],
							 touch.px, touch.py, colors[index], 2.0, 0);
			}
			printf("\x1b[5;1HDrawing:  Y\x1b[K");
		}
		// save touch for the next itereation
		last_touch = touch;
	}
	else
	{
		if (eraser && is_touching(&last_touch))
		{
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
	// #undef clrWhiteshowUnpublishedCommitsButton
}

void main_menu(u32 kDown, PrintConsole *bottom,
			   PrintConsole *top)
{
	const char* menu_options[]={"button 1", "button 2", "button 3"};
	const u8 len_options=sizeof(menu_options)/sizeof(char*);
	static u8 i = 0;
	if (kDown & KEY_A)
	{
		// TODO select
	}else{
		if (kDown & KEY_DOWN)
		{
			i = (i + 1) % len_options;
		}
		else if (kDown & KEY_UP)
		{
			i = (i-1) >= 0 ? i-1 : len_options - 1;
		}
		printf("\x1b[9;1Hselected:    %hhd\x1b[K", i);
		consoleSelect(bottom);
		printf("\x1b[5;1H");
		for (u8 line = 0; line < len_options; line++)
		{
			//colors in sgr section, colors, range 30-37
		 //http://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
			if(line==i){
				printf("\x1b[4C\x1b[32m%s\x1b[0m\x1b[K\n", menu_options[line]);
			}else{
				printf("\x1b[4C%s\x1b[K\n", menu_options[line]);

			}
		}
		

	}
	
}

// void set_step(){
//     act_step=step;
// }

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
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	// consoleInit(GFX_TOP, NULL);
	consoleInit(GFX_TOP, &top_print);
	consoleInit(GFX_BOTTOM, &bottom_print);

	// act_step=drawing;

	// Get screen target
	C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	// C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget *top = NULL;

	u32 len = sizeof(colors) / sizeof(u32);
	// svcSetTimer
	time_t max = 60;
	time_t begin = time(NULL);

	// calculate buttons dimension
	buttons_list[0].h = buttonh(2, 30, 1, 0, SCREEN_HEIGHT);
	buttons_list[0].w = SCREEN_WIDTH - 40;
	C2D_TargetClear(bottom, clrBackground);
	// Main loop
	while (aptMainLoop())
	{
		//default screen for print
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
		switch (act_step)
		{
		case menu:
			generate_buttons();
			main_menu(kDown, &bottom_print, &top_print);
			break;

		case drawing:
			draw(kDown, bottom, len, 3);

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

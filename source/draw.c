#include <3ds.h>
#include "draw.h"
#include "common.h"
#include "utils.h"

const float erase_size = 10.f;
const float erase_border = 1.f;

const char *my_color_name[] = {"Red", "Green", "Blue", "White", "Black"};
const u32 my_colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFFFF, 0xFF000000};
const u32 my_colors_len = sizeof(my_colors) / sizeof(u32);

static u32 curr_color=0;
static bool eraser=0;
static touchPosition last_touch = {0, 0};

static float get_thickness(float x1, float y1, float x2, float y2, float l)
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

static void draw_erase(touchPosition *touch, float e_size, float e_border,
				u32 c_border, u32 c_background)
{
	C2D_DrawRectSolid(touch->px - (e_size / 2.f), touch->py - (e_size / 2.f),
					  0.f, e_size, e_size, c_border);
	C2D_DrawRectSolid(touch->px - (e_size / 2.f) + e_border,
					  touch->py - (e_size / 2.f) + e_border, 0.f,
					  erase_size - (erase_border * 2.f),
					  erase_size - (erase_border * 2.f), c_background);
}

void draw(u32 kDown, C3D_RenderTarget *screen, u32 clrBgInd)
{
	// save background color in a variable
	// u32 clrBackground = colors[clrBgInd];

	// key down related actions
	if (kDown & KEY_X)
		C2D_TargetClear(screen, clrBackground);
	if (kDown & KEY_RIGHT)
	{
		curr_color = (curr_color + 1) % my_colors_len;
	}
	else if (kDown & KEY_LEFT)
	{
		if (curr_color == 0x00)
		{
			curr_color = my_colors_len - 1;
		}
		else
		{
			curr_color = curr_color - 1;
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
	printf("\x1b[6;1HColor:   %s\x1b[K", my_color_name[curr_color]);
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
				C2D_DrawLine(last_touch.px, last_touch.py, my_colors[curr_color],
							 touch.px, touch.py, my_colors[curr_color], 2.0, 0);
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
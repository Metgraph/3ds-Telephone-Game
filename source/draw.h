#ifndef draw_h
#define draw_h
#include <citro2d.h>

#define clrRed my_colors[0]
#define clrGreen my_colors[1]
#define clrBlue my_colors[2]
#define clrWhite my_colors[3]
#define clrBlack my_colors[4]
// define background color
#define clrBackground clrWhite
// Create colors
// clrRed, clrGreen, clrBlue, clrWhite, clrBlack with values saved in the order
// ABGR
extern const char *my_color_name[];
extern const u32 my_colors[];
extern const u32 my_colors_len;

void draw(u32 kDown, C3D_RenderTarget *screen, u32 clrBgInd);

#endif
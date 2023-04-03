#ifndef menu_h
#define menu_h
#include <3ds.h>
#include <citro2d.h>
#include "common.h"
#define P_TOP_ROW 30
#define P_TOP_COL 50
#define P_BOTTOM_ROW 30
#define P_BOTTOM_COL 40

void init_menu();
void destroy_menu();
void set_screen(screenID screen_id, C3D_RenderTarget *screen);
void set_menu_options(const char* new_menu_options[], const step new_menu_actions[], u8 options_sz, step default_action);
step run_menu(u32 kDown);
u8 get_curr_opt();

#endif
#include "menu.h"
static PrintConsole *curr_screen;
static screenID curr_screen_id;
static u8 curr_opt;
static const char **menu_options;
static const step *menu_actions;
static step default_action;
static u8 len_options;

void set_screen(screenID screen_id, PrintConsole *screen){
    curr_screen_id=screen_id;
    curr_screen=screen;
}

void set_menu_options(const char* new_menu_options[], const step new_menu_actions[], u8 options_sz, step new_default_action){
    len_options = options_sz;
    menu_options=new_menu_options;
    menu_actions=new_menu_actions;
    default_action=new_default_action;
    curr_opt=0;
}

u8 get_curr_opt(){
    return curr_opt;
}

static void cursor_to_first_row(){
    u8 rows= curr_screen_id==TOPSCREEN ? P_TOP_ROW : P_BOTTOM_ROW;
    u8 first_row=(rows-len_options)/2;
    printf("\x1b[%hhd;1H", first_row);
}

//calculate new marked option
static void get_new_curr_opt(u32 kDown){
    if (kDown & KEY_DOWN)
    {
        curr_opt = (curr_opt + 1) % len_options;
    }
    else if (kDown & KEY_UP)
    {
        curr_opt = (curr_opt - 1) >= 0 ? curr_opt - 1 : len_options - 1;
    }
    printf("\x1b[9;1HTimer:    %hhd\x1b[K", curr_opt);
}

step run_menu(u32 kDown)
{
    step next_step=default_action;
	const char *menu_options[] = {"button 1", "button 2", "button 3"};
	const u8 len_options = sizeof(menu_options) / sizeof(char *);
	const step menu_actions[] = {DRAWING, MAIN_MENU, MAIN_MENU};
	if (kDown & KEY_A)
	{
		next_step = menu_actions[curr_opt];
	}
	else
	{
		get_new_curr_opt(kDown);
        consoleSelect(curr_screen);
        cursor_to_first_row();
		for (u8 line = 0; line < len_options; line++)
		{
			// colors in sgr section, colors, range 30-37
			// http://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
			if (line == curr_opt)
			{
				printf("\x1b[4C\x1b[32m%s\x1b[0m\x1b[K\n", menu_options[line]);
			}
			else
			{
				printf("\x1b[4C%s\x1b[K\n", menu_options[line]);
			}
		}
	}
    return next_step;
}
#include "menu.h"
static C3D_RenderTarget *curr_screen;
static screenID curr_screen_id;
static u8 curr_opt;
static const char **menu_options;
static C2D_TextBuf text_buf;
static C2D_Text text;
static C2D_TextBuf marked_text_buf;
static C2D_Text marked_text;
static const step *menu_actions;
static step default_action;
static u8 len_options;

void init_menu(){
    text_buf= C2D_TextBufNew(30*(50-1));
    marked_text_buf = C2D_TextBufNew(30);
}

void destroy_menu(){
    C2D_TextBufDelete(text_buf);
    C2D_TextBufDelete(marked_text_buf);
}

void set_screen(screenID screen_id, C3D_RenderTarget *screen){
    curr_screen_id=screen_id;
    curr_screen=screen;
}

void set_menu_options(const char* new_menu_options[], const step new_menu_actions[], u8 options_sz, step new_default_action){
    C2D_TextBufClear(text_buf);
    len_options = options_sz;
    menu_options=new_menu_options;
    C2D_TextParseLine(&marked_text, marked_text_buf, menu_options[0], 0);
    for(u32 i=1; i<len_options; i++){
        C2D_TextParseLine(&text, text_buf, menu_options[i], i);
    }
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
    u8 old_curr=curr_opt;
    if (kDown & KEY_DOWN)
    {
        curr_opt = (old_curr + 1) % len_options;
    }
    else if (kDown & KEY_UP)
    {
        curr_opt = (old_curr - 1) >= 0 ? old_curr - 1 : len_options - 1;
    }
    C2D_TextParseLine(&text, text_buf, menu_options[old_curr], old_curr);
    C2D_TextParseLine(&text, text_buf, "\0", curr_opt);
    C2D_TextParseLine(&marked_text, marked_text_buf, menu_options[curr_opt], curr_opt);
    C2D_TextParseLine(&marked_text, marked_text_buf, "\0", old_curr);
    C2D_TextOptimize(&text);
    C2D_TextOptimize(&marked_text);
    printf("\x1b[9;1HCurrOpt:    %hhd\x1b[K", curr_opt);
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
        C2D_TargetClear(curr_screen, 0xFF00FFFF);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        // C2D_TargetClear(top, clrClear);
        C2D_SceneBegin(curr_screen);
		get_new_curr_opt(kDown);
        C2D_DrawText(&text, 0, 0.f, 0.f, 0.f, 1.f, 1.f);
        C2D_DrawText(&marked_text, C2D_AlignCenter&C2D_WithColor, 0.f, 0.f, 0.f, 1.f, 1.f, 0xFF0000FF);
	}
    return next_step;
}
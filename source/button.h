#ifndef button_h
#define button_h
#include <3ds/types.h>
#include <3ds/services/hid.h>

typedef struct Button{
    u16 x;
    u16 y;
    u16 w;
    u16 h;
    char* text;
    void (*func) (); 
} Button;

u16 buttonh(u8 n, u16 paddingx, bool border, u16 minh, u16 height);
u16 buttonh_perc(u8 n, float perc_pad_x, bool border);
int8_t get_clicked_button(touchPosition* touch, Button buttons[], u8 size);
void set_buttons(Button buttons[], u8 size, u16 paddingx, u16 paddingy, u16 screen_w, u16 screen_h);
void draw_button(Button *button);

#endif
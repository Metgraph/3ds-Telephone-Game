#include "button.h"
#include <3ds.h>
#include <citro2d.h>

//get height of each button
u16 buttonh(u8 n, u16 paddingx, bool border, u16 minh, u16 height) {
	n = border ? n + 1 : n - 1;
    u16 ret = (height - (n)*paddingx) / n;
	if (minh != 0) {
		return ret * (ret >= minh);
	}
	return ret;
}

//get height of each button based on perc
u16 buttonh_perc(u8 n, float perc_pad_x, bool border) {
	float sum = border ? perc_pad_x : -perc_pad_x;
    float den = 1 / perc_pad_x;
	printf("Denominatore: %f\n", den);
	u16 ret = (u16)240 / ((den + 1) / den * n + sum);
	// h = (x+(x/y)) * n -> (xy+x)/y*n = h ->
	return ret;
}

void set_buttons(Button buttons[], u8 size, u16 paddingx, u16 paddingy, u16 screen_w, u16 screen_h){
    u16 h=buttonh(size, paddingy, 1, 0, screen_h);
    if(h==0){
        return;
    }
    u16 y=paddingy;
    u16 w=screen_w - paddingx*2;
    for (size_t i = 0; i < size; i++)
    {
        Button temp={paddingx, y, w, h};
        y+=h+paddingy;
        buttons[i]=temp;
    }

}

void draw_button(Button *button){
    C2D_DrawRectSolid(button->x, button->y,0,button->w, button->h, 0xFF0000FF);
}

//get which button is pressed, return -1 if no one is pressed
int8_t get_clicked_button(touchPosition* touch, Button buttons[], u8 size){
    for (size_t i = 0; i < size; i++)
    {
        u16 xw= buttons[i].x + buttons[i].w;
        u16 yh= buttons[i].y + buttons[i].h;
        if(buttons[i].x <= touch->px && touch->px <= xw && buttons[i].y <= touch->py && touch->py <= yh){
            return i;
        }
    }
    return -1;
}
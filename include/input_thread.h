#ifndef input_thread_h
#define input_thread_h
#include <3ds/types.h>
#include <3ds/services/hid.h>
#include <stdlib.h>

typedef struct 
{
    u32 k_down;
    touchPosition touch;
} Input;

#define CIRC_BUFFER_LEN 32
struct Circular_buffer
{
    Input inputs[CIRC_BUFFER_LEN];
    u8 begin;
    u8 len;
};

void read_input(struct Circular_buffer* buffer);

#endif
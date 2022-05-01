#include <3ds.h>
#include <include/input_thread.h>

static bool runThread = true;

void read_input(struct Circular_buffer* buffer) {
    buffer->len = 0;
    buffer->begin = 0;
    u8 ind = 0;
    while (runThread) {
        hidWaitForEvent(HIDEVENT_PAD0, false);
        hidScanInput();
        buffer->inputs[ind].k_down = hidKeysDown();
        hidTouchRead(&buffer->inputs[ind].touch);

        if (++buffer->len >= CIRC_BUFFER_LEN) {
            buffer->begin = ++buffer->begin % CIRC_BUFFER_LEN;
        }
        ind = (ind + 1) % CIRC_BUFFER_LEN;
    }
}

// no required mutual exclusion
void stop_input_thread() { runThread = false; }

Input* get_buff_var(struct Circular_buffer* buffer, u8 i) {
    // it doesn't matter if i>CIRC_BUFFER_LEN, it will be a problem of who will
    // use it
    return &buffer->inputs[(i + buffer->begin) % CIRC_BUFFER_LEN];
}
void clear_buff(struct Circular_buffer* buffer, u8 len) {
    //mutex on
    buffer->begin = (len + buffer->begin) % CIRC_BUFFER_LEN;
    buffer->len = 0;
    //mutex off
}

void clear_buff(struct Circular_buffer* buffer) {
    clear_buff(buffer, buffer->len);
}
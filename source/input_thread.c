#include <3ds.h>

#include <include/input_thread.h>

static bool runThread=true;

void read_input(struct Circular_buffer* buffer){
    buffer->len=0;
    buffer->begin=0;
    u8 ind=0;
    while(runThread){
        hidWaitForEvent(HIDEVENT_PAD0, false);
        hidScanInput();
        //mutex begin
        buffer->inputs[ind].k_down=hidKeysDown();
        hidTouchRead(&buffer->inputs[ind].touch);
        //mutex end
        if(++buffer->len >= CIRC_BUFFER_LEN){
            buffer->begin=(buffer->begin+1)%CIRC_BUFFER_LEN;
        }
        ind = (ind + 1)%CIRC_BUFFER_LEN;
    }
}

//no required mutual exclusion
void stop_input_thread(){
    runThread=false;
}


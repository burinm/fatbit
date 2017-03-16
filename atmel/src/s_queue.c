#include "s_queue.h"

/* prototype - Currently stack, need FIFO.... */

s_queue S_Q;

void s_queue_init(void) {
    S_Q.index=0;
    for (int i=0;i<QUEUE_LENGTH;i++) {
        S_Q.m[i]=(s_message){0};
    }
}

void s_enqueue(s_message m) {
    if (S_Q.index < QUEUE_LENGTH) {
        S_Q.index++;
        S_Q.m[S_Q.index]=m;
    }
}

s_message s_dequeue(void) {
    if (S_Q.index > 0) {
        return S_Q.m[S_Q.index--];
    }

return (s_message){0};
}

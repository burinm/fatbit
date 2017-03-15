#include "s_queue.h"

/* prototype - Currently LIFO, need FIFO.... */

s_queue S_Q;

static uint8_t s_atoi(char c);

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

e_sm_type s_get_message_type(s_message m) {
    if (m.message[0] != '#') {
        return S_NONE;
    }

    switch (m.message[1]) {
        case    'A':
            return S_LED_ON;
            break;
        case    'B':
            return S_LED_OFF;
            break;
        case    'T':
            return S_TEMP;
            break;
        default:
            return S_NONE;
    };

}

uint8_t s_message_get_value(s_message m) {
uint8_t v=0;
uint8_t power_10=1;
    for (int i=S_QUEUE_VALUE_OFFSET_END;i>=S_QUEUE_VALUE_OFFSET_START;i--) {
        v+= power_10 * s_atoi(m.message[i]);
        power_10 *= 10;
    }

return v;
}

#define ASCII_9_VALUE   '9'
#define ASCII_0_VALUE   '0'
static uint8_t s_atoi(char c) {
    if ( c > ASCII_9_VALUE || c < ASCII_0_VALUE) { return 0; }
    return (c - ASCII_0_VALUE);
}

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "s_message.h"

/* El Cheapo queue implementation */

#include <stdint.h>


#define QUEUE_LENGTH    25

typedef struct {
    s_message m[QUEUE_LENGTH];
    uint8_t index;
} s_queue;

extern s_queue S_Q;

void s_queue_init(void);
void s_enqueue(s_message m);
s_message s_dequeue(void);

#endif

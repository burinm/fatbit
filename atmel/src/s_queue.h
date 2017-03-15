#ifndef __QUEUE_H__
#define __QUEUE_H__

/* El Cheapo queue implementation */

#include <stdint.h>

typedef enum { S_NONE=0, S_LED_ON, S_LED_OFF, S_TEMP } e_sm_type; 

#define QUEUE_LENGTH    25
#define SOURCE_MESSAGE_LENGTH   (8) 

typedef struct {
    uint8_t message[SOURCE_MESSAGE_LENGTH];
} s_message;

typedef struct {
    s_message m[QUEUE_LENGTH];
    uint8_t index;
} s_queue;

extern s_queue S_Q;

void s_queue_init(void);
void s_enqueue(s_message m);
s_message s_dequeue(void);
e_sm_type s_get_message_type(s_message m);

#endif

#ifndef __S_MESSAGE_H__
#define __S_MESSAGE_H__

#include <stdint.h>

typedef enum { S_NONE=0, S_LED_ON, S_LED_OFF, S_TEMP } e_sm_type; 

#define QUEUE_LENGTH    25
#define SOURCE_MESSAGE_LENGTH   (8)
#define SOURCE_MESSAGE_BUF_LENGTH   (SOURCE_MESSAGE_LENGTH + 1)
#define S_MESSAGE_START_OFFSET          0 
#define S_MESSAGE_TYPE_OFFSET           1
#define S_MESSAGE_VALUE_OFFSET_START    2
#define S_MESSAGE_VALUE_OFFSET_END      4

typedef struct {
    uint8_t message[SOURCE_MESSAGE_BUF_LENGTH];
} s_message;

e_sm_type s_get_message_type(s_message *m);
s_message* s_message_new(e_sm_type t);
uint8_t s_message_get_value(s_message *m);
uint8_t s_message_set_value(s_message *m, uint8_t v);


#endif

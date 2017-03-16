#include "s_message.h"

static uint8_t s_atoi(char c);

e_sm_type s_get_message_type(s_message *m) {
    // Messages must start with '#"
    if (m->message[0] != '#') {
        return S_NONE;
    }

    // Messages type t, is stored in "#Tnnnxxx" 
    switch (m->message[1]) {
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

uint8_t s_message_get_value(s_message *m) {
uint8_t v=0;
uint8_t power_10=1;
    // Message value is an ascii number 0-255 , nnn, stored in "#Tnnnxxx"
    for (int i=S_MESSAGE_VALUE_OFFSET_END;i>=S_MESSAGE_VALUE_OFFSET_START;i--) {
        v+= power_10 * s_atoi(m->message[i]);
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

#include "s_message.h"
#include <stdlib.h>
#include <string.h>

#ifdef S_GLOBAL_IDS
    static uint8_t s_m_global_id=0;
    static void s_message_set_id(s_message *m);
#endif

static uint8_t s_atoi(char c);
static char s_itoa(uint8_t i);

e_sm_type s_get_message_type(s_message *m) {
    // Messages must start with '#"
    if (m->message[S_MESSAGE_START_OFFSET] != '#') {
        return S_NONE;
    }

    // Messages type t, is stored in "#Tnnnxxx" 
    switch (m->message[S_MESSAGE_TYPE_OFFSET]) {
        case    'A':
            return S_LED_ON;
            break;
        case    'B':
            return S_LED_OFF;
            break;
        case    'T':
            return S_TEMP;
            break;
        case    'S':
            return S_SUN;
            break;
        case    'P':
            return S_PULSE;
            break;
        case    'N':
            return S_NOTIFY;
            break;
        default:
            return S_NONE;
    };

}

s_message * s_message_new(e_sm_type t) {

    s_message *m = (s_message *)malloc(sizeof(s_message));
    /* initialize new message string
        "#0000000(NULL)"
    */
    m->message[SOURCE_MESSAGE_LENGTH] = 0;
    memset(m->message,'0',SOURCE_MESSAGE_LENGTH);
    m->message[S_MESSAGE_START_OFFSET] = '#';

    switch (t) {
        case    S_LED_ON:
            m->message[S_MESSAGE_TYPE_OFFSET] = 'A';
            break;
        case    S_LED_OFF:
            m->message[S_MESSAGE_TYPE_OFFSET] = 'B';
            break;
        case    S_TEMP:
            m->message[S_MESSAGE_TYPE_OFFSET] = 'T';
            break;
        case    S_SUN:
            m->message[S_MESSAGE_TYPE_OFFSET] = 'S';
            break;
        case    S_PULSE:
            m->message[S_MESSAGE_TYPE_OFFSET] = 'P';
            break;
        case    S_NOTIFY:
            m->message[S_MESSAGE_TYPE_OFFSET] = 'N';
            break;
        default:
            m->message[S_MESSAGE_TYPE_OFFSET] = '0';
    }
#ifdef S_GLOBAL_IDS
    s_message_set_id(m);
#endif
return m;
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

uint8_t s_message_set_value(s_message *m, uint8_t v) {
uint8_t tmp=0;
uint8_t power_10=100;
    // Message value is an ascii number 0-255 , nnn, stored in "#Tnnnxxx"
    for (int i=S_MESSAGE_VALUE_OFFSET_START;i<=S_MESSAGE_VALUE_OFFSET_END;i++) {
        tmp=v/power_10;
        m->message[i]=s_itoa(tmp);
        v=v-(tmp*power_10);
        power_10 /=10;
    }

return v;
}

#ifdef S_GLOBAL_IDS
    static void s_message_set_id(s_message *m) {
    uint8_t tmp=0;
    uint8_t power_10=100;
    uint8_t v=s_m_global_id;
        // Id value is an ascii number 0-255 , nnn, stored in "#Txxxiii"
        for (int i=S_MESSAGE_ID_OFFSET_START;i<=S_MESSAGE_ID_OFFSET_END;i++) {
            tmp=v/power_10;
            m->message[i]=s_itoa(tmp);
            v=v-(tmp*power_10);
            power_10 /=10;
        }

        s_m_global_id++;
    }
#endif

#define ASCII_9_VALUE   '9'
#define ASCII_0_VALUE   '0'
static uint8_t s_atoi(char c) {
    if ( c > ASCII_9_VALUE || c < ASCII_0_VALUE) { return 0; }
    return (c - ASCII_0_VALUE);
}

static char s_itoa(uint8_t i) {
    if ( i> 9 ) { return ASCII_0_VALUE;}
    return (ASCII_0_VALUE + i);
}

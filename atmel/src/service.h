#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "at_ble_api.h"

#define SERVICE_ADVERTISING_INTERVAL    1000    // .625 * 1000 = 625ms
#define SERVICE_ADVERTISING_TIMEOUT     0       // Disable adverftisement timeout

#define SERVICE_EVENT_LOOP_TIMEOUT      100     // milliseconds, controls how often to wake up 
                                                //  and check the UART message Q

// 7.5ms <-> 4 seconds
#define SERVICE_CONN_INT_MIN            1000
#define SERVICE_CONN_INT_MAX            1000    // 1.25ms * 600 = 750ms

#define SERVICE_CONN_LAT                0      // Number of events
#define SERVICE_CONN_SUP_TIMOUT         300    // 10ms * 300 = 3s
#define SERVICE_CONN_CE_MIN             3200
#define SERVICE_CONN_CE_MAX             3200   // .625ms * 3200 = 2 seconds

extern at_ble_handle_t master_connection_handle;

//Command processing
void process_command_messages(void);


#endif

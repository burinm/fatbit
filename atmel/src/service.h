#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "at_ble_api.h"

#define SERVICE_ADVERTISING_INTERVAL    1000    // .625 * 1000 = 625ms
#define SERVICE_ADVERTISING_TIMEOUT     0       // Disable adverftisement timeout

#define SERVICE_EVENT_LOOP_TIMEOUT      100     // milliseconds

#define SERVICE_CONN_INT_MIN            100 
#define SERVICE_CONN_INT_MAX            100    // 1.25ms * 100 = 125ms 
#define SERVICE_CONN_LAT                5      // Number of events ((5 + 1) * 100ms = 600ms)
#define SERVICE_CONN_SUP_TIMOUT         300      // 10ms * 300 = 3s 
#define SERVICE_CONN_CE_MIN             5000
#define SERVICE_CONN_CE_MAX             5000    // .625ms * 1600 = 3.125 seconds

extern at_ble_handle_t master_connection_handle;

//Command processing
void process_command_messages(void);


#endif

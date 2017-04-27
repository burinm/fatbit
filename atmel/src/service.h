#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "at_ble_api.h"

#define SERVICE_TX_POWER_LEVEL          AT_BLE_TX_PWR_LVL_NEG_07_DB

#define SERVICE_ADVERTISING_INTERVAL    1000    // .625ms * 1000 = 625ms
#define SERVICE_ADVERTISING_TIMEOUT     0       // Disable adverftisement timeout

#define SERVICE_EVENT_LOOP_TIMEOUT      100     // milliseconds, controls how often to wake up 
                                                //  and check the UART message Q

// 7.5ms <-> 4 seconds
/* The sensor array only sends updates of sunlight and pulse every 3.75 seconds
    
    So, in reality, the connection interval only needs to be less than 3.75 seconds.
*/

#if 0
#define SERVICE_CONN_INT_MIN           200   
#define SERVICE_CONN_INT_MAX           240   // 1.25ms * 240 = 300ms 
#define SERVICE_CONN_LAT               4     // 300ms * (4 + 1) = 1.5 second 
#define SERVICE_CONN_SUP_TIMOUT        330   // 10ms * 330 = 3.3s , Need 1.5s + (6 * 300ms) = 3.3
#define SERVICE_CONN_CE_MIN            0     // Not used
#define SERVICE_CONN_CE_MAX            0     // Not used
#endif


/*
    If we were to update the heart rate monitor every pulse (instead of sending and
    average), we would need to update at this rate:

    Fastest pulse = 220BPM = 3.7BPS = 272ms (this would be the connection interval max)
*/

#if 1
#define SERVICE_CONN_INT_MIN           200 
#define SERVICE_CONN_INT_MAX           217    // 1.25ms * 272 = 271ms 
#define SERVICE_CONN_LAT               3      // 271ms * (1+3) = 1 second
#define SERVICE_CONN_SUP_TIMOUT        260    // 10ms * 260 = 2.6s , Need 1s + (271ms * 6) = 2.6s 
#define SERVICE_CONN_CE_MIN            0      // Not used
#define SERVICE_CONN_CE_MAX            0      // Not used
#endif

extern at_ble_handle_t master_connection_handle;

//Command processing
void process_command_messages(void);


#endif

#ifndef __MAIN_H__
#define __MAIN_H__

//#define DEBUG_ON

#define LOWEST_POWER_MODE           EM3
#define CALIBRATE_LE_ULFRCO         true

// If defined, you DMA to read internal temp, else poll
#define USING_DMA_FOR_TEMP

// If defined, use Gecko internal sensor, else use TSL2651
//#define INTERNAL_LIGHT_SENSOR

// Enable/Disable sending serial messages about status
#define SEND_EXTERNAL_NOTIFICATIONS

#include "circbuf_tiny.h"

//outgoing message queue, circular buffer
extern circbuf_tiny_t O_Q;

#endif

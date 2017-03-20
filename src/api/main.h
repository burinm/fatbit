#ifndef __MAIN_H__
#define __MAIN_H__

//#define DEBUG_ON

#define LOWEST_POWER_MODE           EM3
#define CALIBRATE_LE_ULFRCO         true
#define USING_DMA_FOR_TEMP
//#define INTERNAL_LIGHT_SENSOR


#include "circbuf_tiny.h"

//outgoing message queue, circular buffer
extern circbuf_tiny_t O_Q;

#endif

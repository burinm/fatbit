#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "at_ble_api.h"

extern at_ble_handle_t master_connection_handle;

//Command processing
void process_command_messages(void);


#endif

#ifndef __SUNLIGHT_SERVICE_H__
#define __SUNLIGHT_SERVICE_H__

#include "ble_manager.h"

#define SUNLIGHT_SERVICE_UUID                 (0x1337)
#define SUNLIGHT_SERVICE_CHAR_UUID            (0x2A76) //UV Index
#define SUNLIGHT_SERVICE_DESC                 "Exposure to light"
#define SUNLIGHT_SERVICE_DESC_LEN            17 

#define SUNLIGHT_TOTAL_CHARATERISTIC_NUM  1
typedef struct sunlight_gatt_service_handler {
    /** service uuid */
    at_ble_uuid_t serv_uuid;
    /** service handle */
    at_ble_handle_t serv_handle;
    /** characteristic handle */
    at_ble_characteristic_t serv_chars[SUNLIGHT_TOTAL_CHARATERISTIC_NUM];
} sunlight_gatt_service_handler_t;


void sunlight_service_init(sunlight_gatt_service_handler_t *);
void sunlight_service_define(sunlight_gatt_service_handler_t *);

//Internal
//Sunlight Service
#define SUNLIGHT_NOTIFICATION_ON_IND        2       //No idea why this is 2...
#define SUNLIGHT_NOTIFICATION_OFF_IND       0
bool sunlight_send_notification(uint8_t sun);
extern sunlight_gatt_service_handler_t sunlight_service_handle;
extern volatile bool Sunlight_Notification_Flag;
at_ble_status_t sunlight_char_changed_handler(void *params);
at_ble_status_t mystery1(void *params);
at_ble_status_t mystery2(void *params);
at_ble_status_t mystery3(void *params);
at_ble_status_t mystery4(void *params);
at_ble_status_t mystery5(void *params);
at_ble_status_t mystery6(void *params);
at_ble_status_t mystery7(void *params);
at_ble_status_t mystery8(void *params);
at_ble_status_t mystery9(void *params);

#endif  

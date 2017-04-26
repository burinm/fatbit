#ifndef __HEART_RATE_SERVICE_H__
#define __HEART_RATE_SERVICE_H__

#include "ble_manager.h"
#include "at_ble_api.h"

#define HEART_RATE_SERVICE_UUID                 (0x180D)
#define HEART_RATE_MEASUREMENT_CHAR_UUID        (0x2A37)

#define HEART_RATE_TOTAL_CHARATERISTIC_NUM  1
typedef struct heart_rate_gatt_service_handler {
    /** service uuid */
    at_ble_uuid_t serv_uuid;
    /** service handle */
    at_ble_handle_t serv_handle;
    /** characteristic handle */
    at_ble_characteristic_t serv_chars[HEART_RATE_TOTAL_CHARATERISTIC_NUM];
} heart_rate_gatt_service_handler_t;

#define HEART_RATE_CHAR_MAX_LEN    6

#define HEART_RATE_NOTIFY_ON       1
#define HEART_RATE_NOTIFY_OFF      0 

extern volatile bool Heart_Rate_Notification_Flag;
extern heart_rate_gatt_service_handler_t heart_rate_service_handle;


void heart_rate_service_init(heart_rate_gatt_service_handler_t *);
void heart_rate_service_define(heart_rate_gatt_service_handler_t *);
void heart_rate_set_value(uint8_t value);

//Heart Rate GATT
extern const ble_event_callback_t gatt_heart_rate_cbs[];

#endif  

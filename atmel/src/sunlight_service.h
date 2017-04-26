#ifndef __SUNLIGHT_SERVICE_H__
#define __SUNLIGHT_SERVICE_H__

#include "ble_manager.h"
#include "at_ble_api.h"

#define SUNLIGHT_SERVICE_UUID                 (0x1337)
#define SUNLIGHT_SERVICE_CHAR_UUID            (0x2A76) //UV Index
#define SUNLIGHT_SERVICE_DESC                 (uint8_t*)"Exposure to light"
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

#define SUNLIGHT_NOTIFY_ON       2
#define SUNLIGHT_NOTIFY_OFF      0 

extern volatile bool Sunlight_Notification_Flag;
extern sunlight_gatt_service_handler_t sunlight_service_handle;


void sunlight_service_init(sunlight_gatt_service_handler_t *);
void sunlight_service_define(sunlight_gatt_service_handler_t *);
void sunlight_set_value(uint8_t value);

//Sunlight GATT
extern const ble_event_callback_t gatt_sunlight_cbs[];
#if 0
static at_ble_status_t sunlight_notification_confirmed_app_event(void *param);
static at_ble_status_t sunlight_char_changed_app_event(void *param);
static at_ble_status_t gatt1_event(void *param);
static at_ble_status_t gatt2_event(void *param);
static at_ble_status_t gatt3_event(void *param);
static at_ble_status_t gatt4_event(void *param);
static at_ble_status_t gatt5_event(void *param);
static at_ble_status_t gatt6_event(void *param);
static at_ble_status_t gatt7_event(void *param);
static at_ble_status_t gatt8_event(void *param);
#endif


#endif  

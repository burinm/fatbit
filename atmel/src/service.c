#include <asf.h>
#include "platform.h"
#include "console_serial.h"
#include "service.h"
#include "sunlight_service.h"

static at_ble_handle_t master_connection_handle=0;

//APIs
void sunlight_set_value(uint8_t value);

//BLE GAP
static at_ble_status_t ble_connected_cb(void *param);
static at_ble_status_t ble_disconnected_cb(void *param);
static at_ble_status_t ble_paired_cb(void *param);

static const ble_event_callback_t gap_app_cbs[] = {
    NULL, // AT_BLE_UNDEFINED_EVENT
    NULL, // AT_BLE_SCAN_INFO
    NULL, // AT_BLE_SCAN_REPORT
    NULL, // AT_BLE_ADV_REPORT
    NULL, // AT_BLE_RAND_ADDR_CHANGED
    ble_connected_cb, // AT_BLE_CONNECTED
    ble_disconnected_cb, // AT_BLE_DISCONNECTED
    NULL, // AT_BLE_CONN_PARAM_UPDATE_DONE
    NULL, // AT_BLE_CONN_PARAM_UPDATE_REQUEST
    ble_paired_cb, // AT_BLE_PAIR_DONE
    NULL, // AT_BLE_PAIR_REQUEST
    NULL, // AT_BLE_SLAVE_SEC_REQUEST
    NULL, // AT_BLE_PAIR_KEY_REQUEST
    NULL, // AT_BLE_ENCRYPTION_REQUEST
    NULL, // AT_BLE_ENCRYPTION_STATUS_CHANGED
    NULL, // AT_BLE_RESOLV_RAND_ADDR_STATUS
    NULL, // AT_BLE_SIGN_COUNTERS_IND
    NULL, // AT_BLE_PEER_ATT_INFO_IND
    NULL // AT_BLE_CON_CHANNEL_MAP_IND
};

//Sunlight GATT
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

static const ble_event_callback_t gatt_sunlight_cbs[] = {
    sunlight_notification_confirmed_app_event,
    gatt1_event,
    sunlight_char_changed_app_event,
    gatt2_event,
    gatt3_event,
    gatt4_event,
    gatt5_event,
    gatt6_event,
    gatt7_event,
    gatt8_event
};

void ble_advertise(void);

int main(void) {

    platform_driver_init();
    acquire_sleep_lock();

    serial_console_init();

    ble_device_init(NULL);
    ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE,
                                     gap_app_cbs);

    sunlight_service_init(&sunlight_service_handle);
    sunlight_service_define(&sunlight_service_handle);
    ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE,
                                     gatt_sunlight_cbs); 

    ble_advertise();

    //ble_set_ulp_mode(BLE_ULP_MODE_SET);

    while(1) {
        ble_event_task(655);
        //sunlight_set_value(123);
    }
                

}

void ble_advertise(void) {
at_ble_status_t status;

    printf("Start Advertising (initial)\n\r");
    status = ble_advertisement_data_set();
    if(status != AT_BLE_SUCCESS)
    {
        printf("## Advertisement data set failed : error %x\n\r",status);
        while(1);
    }

    /* Start of advertisement */
    status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,
    AT_BLE_ADV_GEN_DISCOVERABLE,
    NULL,
    AT_BLE_ADV_FP_ANY,
    1000,
    655,
    0);

    if(status != AT_BLE_SUCCESS)
    {
        printf("## Advertisement data set failed : error %x\n\r",status);
        while(1);
    }
}

//BLE GAP events

at_ble_status_t ble_connected_cb(void *param) {
at_ble_connected_t *connected = (at_ble_connected_t *)param;

    master_connection_handle=connected->handle;

    printf("ble_connected_cb\n\r");
    return AT_BLE_SUCCESS;
}

static at_ble_status_t ble_disconnected_cb(void *param)
{
    printf("ble_disconnected_cb\n\r");
    ble_advertise();
    return AT_BLE_SUCCESS;
}

static at_ble_status_t ble_paired_cb(void *param)
{
    printf("ble_paired_cb\n\r");
    return AT_BLE_SUCCESS;
}

//Sunlight GATT events
static at_ble_status_t sunlight_notification_confirmed_app_event(void *param)
{
    at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
    if(!notification_status->status)
    {
        printf("Sunlight sending notification to the peer success\n\r");
    }
    return AT_BLE_SUCCESS;
}

static at_ble_status_t sunlight_char_changed_app_event(void *param)
{
    at_ble_characteristic_changed_t *char_handle = (at_ble_characteristic_changed_t *)param;
    printf("Sunlight sunlight_char_changed_app_event\n\r");

    return AT_BLE_SUCCESS;
}

static at_ble_status_t gatt1_event(void *param) {
    printf("gatt1_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t gatt2_event(void *param) {
    printf("gatt2_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t gatt3_event(void *param) {
    printf("gatt3_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t gatt4_event(void *param) {
    printf("gatt4_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t gatt5_event(void *param) {
    printf("gatt5_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t gatt6_event(void *param) {
    printf("gatt6_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t gatt7_event(void *param) {
    printf("gatt7_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t gatt8_event(void *param) {
    printf("gatt8_event\n\r");
    return AT_BLE_SUCCESS;
}

// APIs
void sunlight_set_value(uint8_t value) {
at_ble_status_t status;

    if ((status = at_ble_characteristic_value_set(sunlight_service_handle.serv_chars[0].char_val_handle, &value, sizeof(uint8_t))) != AT_BLE_SUCCESS){
        printf("Sunlight updating the characteristic failed%d\n\r",status);
    } else {
        printf("Sunlight updating the characteristic value is successful\n\r");
    }   

#if 1
  //  if (master_connection_handle) { 
        //if((status = at_ble_notification_send(master_connection_handle,
        if((status = at_ble_indication_send(master_connection_handle,
               sunlight_service_handle.serv_chars[0].char_val_handle)) != AT_BLE_SUCCESS) {
            printf("Sunlight sending notification failed%d\n\r",status);
        }           
        else {  
            printf("Sunlight sending notification successful\n\r");
        }       
   // } 
#endif
}

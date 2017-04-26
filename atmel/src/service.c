#include <asf.h>
#include "platform.h"
#include "console_serial.h"
#include "service.h"
#include "sunlight_service.h"
#include "heart_rate_service.h"

at_ble_handle_t master_connection_handle=0;

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

    heart_rate_service_init(&heart_rate_service_handle);
    heart_rate_service_define(&heart_rate_service_handle);
    ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE,
                                     gatt_heart_rate_cbs); 

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

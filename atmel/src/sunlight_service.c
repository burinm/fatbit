#include "sunlight_service.h"

sunlight_gatt_service_handler_t sunlight_service_handle;
volatile bool Sunlight_Notification_Flag = false;

void sunlight_service_init(sunlight_gatt_service_handler_t * sunlight_service) {

    uint8_t sunlight_uv_init_value = 100;

    //UUID of sunlight service
    sunlight_service->serv_handle = 0;
    sunlight_service->serv_uuid.type = AT_BLE_UUID_16;
    sunlight_service->serv_uuid.uuid[0] = (uint8_t)SUNLIGHT_SERVICE_UUID;
    sunlight_service->serv_uuid.uuid[1] = (uint8_t)(SUNLIGHT_SERVICE_UUID >> 8);

    //Characteristic - Sunlight exposure

    sunlight_service->serv_chars[0].char_val_handle = 0;
    sunlight_service->serv_chars[0].uuid.type = AT_BLE_UUID_16;

    sunlight_service->serv_chars[0].uuid.uuid[0]
        = (uint8_t)SUNLIGHT_SERVICE_CHAR_UUID;
    sunlight_service->serv_chars[0].uuid.uuid[1]
        = (uint8_t)(SUNLIGHT_SERVICE_CHAR_UUID >> 8);

    /* Properties */
    //sunlight_service->serv_chars[0].properties = AT_BLE_CHAR_INDICATE;
    sunlight_service->serv_chars[0].properties = AT_BLE_CHAR_INDICATE;

    sunlight_service->serv_chars[0].init_value = (uint8_t *)&sunlight_uv_init_value;
    sunlight_service->serv_chars[0].value_init_len = sizeof(uint8_t);
    sunlight_service->serv_chars[0].value_max_len = sizeof(uint8_t);
    
    /* Permissions */
    sunlight_service->serv_chars[0].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
    
    /* user defined name */
    sunlight_service->serv_chars[0].user_desc = SUNLIGHT_SERVICE_DESC;
    sunlight_service->serv_chars[0].user_desc_len = SUNLIGHT_SERVICE_DESC_LEN;
    sunlight_service->serv_chars[0].user_desc_max_len = SUNLIGHT_SERVICE_DESC_LEN;
    /*user description permissions*/
    sunlight_service->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;

    /* client config permissions */
    sunlight_service->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;

    /*server config permissions*/
    sunlight_service->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*user desc handles*/
    sunlight_service->serv_chars[0].user_desc_handle = 0;
    /*client config handles*/
    sunlight_service->serv_chars[0].client_config_handle = 0;
    /*server config handles*/
    sunlight_service->serv_chars[0].server_config_handle = 0;

    /* presentation format */
    sunlight_service->serv_chars[0].presentation_format = NULL;


}

void sunlight_service_define(sunlight_gatt_service_handler_t * service) {
  at_ble_status_t status;

    status = at_ble_primary_service_define(&service->serv_uuid,
        &service->serv_handle, NULL, 0,
        service->serv_chars, SUNLIGHT_TOTAL_CHARATERISTIC_NUM);

    if (status != AT_BLE_SUCCESS) {
        printf("Sunlight Service definition Failed,reason: %x\n\r",
                status);
    } else {
        printf("Sunlight service defined succesfully\n\r");
        printf(" Handle %d \n\r",service->serv_handle);
    } 
}

//Functions
bool sunlight_send_notification(uint8_t sun)
{
at_ble_status_t status;

printf("sunlight_send_notification %d\n\r",sun);
#if 1
    /** Updating the new characteristic value */
    if ((status = at_ble_characteristic_value_set(
                            sunlight_service_handle.serv_chars[0].char_val_handle,
                            &sun, sizeof(uint8_t))) != AT_BLE_SUCCESS) {
        printf("Write value for sunlight failed,reason %d",
                status);
        return false;
    }
#endif


#if 0
    /** Sending the indication for the updated characteristic */
    if ((status = at_ble_indication_send(sunlight_service_handle.serv_handle,
                                        sunlight_service_handle.serv_chars[0].char_val_handle
                                        ))) {
        printf("Send sunlight indication failed,reason %d", status);
        return false;
    }
#endif

#if 0
    /** Sending the notification for the updated characteristic */
    if ((status = at_ble_notification_send(sunlight_service_handle.serv_handle,
                                       sunlight_service_handle.serv_chars[0].char_val_handle 
                                        ))) {
        printf("Send notification failed,reason %d", status);
        return false;   
    }
#endif


    return true;
}

at_ble_status_t sunlight_char_changed_handler(void *params)
{

    uint8_t action_event;

    at_ble_characteristic_changed_t change_params;
    memcpy((uint8_t *)&change_params, params,
            sizeof(at_ble_characteristic_changed_t));

//    printf("hr_sensor_char_changed_handler handle=%d handle2 %d length %d offset %d\n\r",change_params.conn_handle,change_params.char_handle,change_params.char_len,change_params.char_offset);

    //This is not obvious in any way...
    if (change_params.char_handle == sunlight_service_handle.serv_chars[0].client_config_handle) {
        //printf("sunlight char changed\n\r");
    printf("char_new_value %d\n\r",change_params.char_new_value[0]);

        if (change_params.char_new_value[0] == SUNLIGHT_NOTIFICATION_ON_IND) {
            printf("Sunlight notifications on\n\r");
            Sunlight_Notification_Flag = true;
        } else if (change_params.char_new_value[0] == SUNLIGHT_NOTIFICATION_OFF_IND) {
            printf("Sunlight notifications off\n\r");
            Sunlight_Notification_Flag = false;
        }

    }

    return AT_BLE_SUCCESS;
}

at_ble_status_t mystery_function(void *params) {
    printf("mystery\n");
return AT_BLE_SUCCESS;
}

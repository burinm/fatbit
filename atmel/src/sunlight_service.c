#include "sunlight_service.h"
#include "service.h"

sunlight_gatt_service_handler_t sunlight_service_handle;
volatile bool Sunlight_Notification_Flag = false;

//These need to be persistant, pointers are kept to them
uint8_t sunlight_uv_init_value = 100;

void sunlight_service_init(sunlight_gatt_service_handler_t * sunlight_service) {


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
    //sunlight_service->serv_chars[0].properties = AT_BLE_CHAR_NOTIFY;
    sunlight_service->serv_chars[0].properties = AT_BLE_CHAR_INDICATE;

    sunlight_service->serv_chars[0].init_value = (uint8_t *)&sunlight_uv_init_value;
    sunlight_service->serv_chars[0].value_init_len = sizeof(uint8_t);
    sunlight_service->serv_chars[0].value_max_len = sizeof(uint8_t);
    
    /* Permissions */
    sunlight_service->serv_chars[0].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;
    
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

//Sunlight GATT events
static at_ble_status_t sunlight_notification_confirmed_app_event(void *param)
{
    at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
    if(!notification_status->status)
    {
        //printf("Sunlight sending notification to the peer success\n\r");
    }
    return AT_BLE_SUCCESS;
}

static at_ble_status_t sunlight_char_changed_app_event(void *param)
{
    //printf("sunlight_char_changed_app_event\n\r");
    at_ble_characteristic_changed_t *change_param = (at_ble_characteristic_changed_t *)param;

    if (change_param->char_handle == sunlight_service_handle.serv_chars[0].client_config_handle) {
        //printf("This *is* the Sunlight sunlight_char_changed_app_event\nmr");
        //printf(" newvalue = %d\n\r",change_param->char_new_value[0]);
        if (change_param->char_new_value[0] == SUNLIGHT_NOTIFY_ON) {
            Sunlight_Notification_Flag = true;            
            printf("Sunlight_Notification_Flag (true)\n\r");
        }
        if (change_param->char_new_value[0] == SUNLIGHT_NOTIFY_OFF) {
            Sunlight_Notification_Flag = false;
            printf("Sunlight_Notification_Flag (false)\n\r");
        }
    }

    return AT_BLE_SUCCESS;
}

static at_ble_status_t gatt1_event(void *param) {
    //printf("indication sent success\n\r");
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

const ble_event_callback_t gatt_sunlight_cbs[] = {
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

// APIs
void sunlight_set_value(uint8_t value) {
at_ble_status_t status;

    if ((status = at_ble_characteristic_value_set(sunlight_service_handle.serv_chars[0].char_val_handle, &value, sizeof(uint8_t))) != AT_BLE_SUCCESS){
        printf("Sunlight updating the characteristic failed%d\n\r",status);
    } else {
        //printf("Sunlight updating the characteristic value is successful\n\r");
    }

#if 1
  //  if (master_connection_handle) { 
        //if((status = at_ble_notification_send(master_connection_handle,
        if((status = at_ble_indication_send(master_connection_handle,
               sunlight_service_handle.serv_chars[0].char_val_handle)) != AT_BLE_SUCCESS) {
            printf("Sunlight sending indication failed%d\n\r",status);
        }
        else {
            printf("Sunlight sending indication successful\n\r");
        }
   // } 
#endif
}

#include "heart_rate_service.h"
#include "service.h"
#include "peripheral.h"

heart_rate_gatt_service_handler_t heart_rate_service_handle;
volatile bool Heart_Rate_Notification_Flag = false;

//These need to be persistant, pointers are kept to them
uint8_t heart_rate_sensor_location_value = WRIST;
uint8_t heart_rate_measurement_value=0;
uint8_t heart_rate_control_point_value = 0;

void heart_rate_service_init(heart_rate_gatt_service_handler_t * heart_rate_service) {


    //UUID of heart rate service
    heart_rate_service->serv_handle = 0;
    heart_rate_service->serv_uuid.type = AT_BLE_UUID_16;
    heart_rate_service->serv_uuid.uuid[0] = (uint8_t)HEART_RATE_SERVICE_UUID;
    heart_rate_service->serv_uuid.uuid[1] = (uint8_t)(HEART_RATE_SERVICE_UUID >> 8);

    /*Characteristic Info for Heart Rate Measurement*/

    /* handle stored here */
    heart_rate_service->serv_chars[0].char_val_handle = 0;
    heart_rate_service->serv_chars[0].uuid.type = AT_BLE_UUID_16;

    /* UUID : Heart Rate Measurement Characteristic */
    heart_rate_service->serv_chars[0].uuid.uuid[0] = (uint8_t)HEART_RATE_MEASUREMENT_CHAR_UUID;
    heart_rate_service->serv_chars[0].uuid.uuid[1] = (uint8_t)(HEART_RATE_MEASUREMENT_CHAR_UUID >> 8);

    /* Properties */
    heart_rate_service->serv_chars[0].properties = AT_BLE_CHAR_NOTIFY;

    heart_rate_service->serv_chars[0].init_value = (uint8_t *)&heart_rate_measurement_value;
    heart_rate_service->serv_chars[0].value_init_len = sizeof(uint16_t);

    //heart_rate_service->serv_chars[0].value_max_len = HR_MM_FLAGS_SIZE +
    //        HR_MM_VAL_SIZE + HR_MM_EX_SIZE + HR_MM_RR_SIZE;
    heart_rate_service->serv_chars[0].value_max_len = HEART_RATE_CHAR_MAX_LEN; 

    /* Permissions */
    heart_rate_service->serv_chars[0].value_permissions = AT_BLE_ATTR_NO_PERMISSIONS;

    /* user defined name */
    heart_rate_service->serv_chars[0].user_desc = NULL;
    heart_rate_service->serv_chars[0].user_desc_len = 0;
    heart_rate_service->serv_chars[0].user_desc_max_len = 0;
    /*user description permissions*/
    heart_rate_service->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;

    /* client config permissions */
    heart_rate_service->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;

    /*server config permissions*/
    heart_rate_service->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*user desc handles*/
    heart_rate_service->serv_chars[0].user_desc_handle = 0;
    /*client config handles*/
    heart_rate_service->serv_chars[0].client_config_handle = 0;
    /*server config handles*/
    heart_rate_service->serv_chars[0].server_config_handle = 0;
    /* presentation format */
    heart_rate_service->serv_chars[0].presentation_format = NULL;

    /* Characteristic Info for Body Sensor Location */
    heart_rate_service->serv_chars[1].char_val_handle = 0;
    heart_rate_service->serv_chars[1].uuid.type = AT_BLE_UUID_16;
    /* UUID : Body Sensor Location*/
    heart_rate_service->serv_chars[1].uuid.uuid[0] = (uint8_t)BODY_SENSOR_LOCATION_CHAR_UUID;
    /* UUID : Body Sensor location*/
    heart_rate_service->serv_chars[1].uuid.uuid[1] = (uint8_t)(BODY_SENSOR_LOCATION_CHAR_UUID >> 8);
    /* Properties */
    heart_rate_service->serv_chars[1].properties = AT_BLE_CHAR_READ;

    heart_rate_service->serv_chars[1].init_value = &heart_rate_sensor_location_value;

    heart_rate_service->serv_chars[1].value_init_len = sizeof(uint8_t);
    heart_rate_service->serv_chars[1].value_max_len = sizeof(uint8_t);

    /* permissions */

    heart_rate_service->serv_chars[1].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR);

    /* user defined name */
    heart_rate_service->serv_chars[1].user_desc = NULL;
    heart_rate_service->serv_chars[1].user_desc_len = 0;
    heart_rate_service->serv_chars[1].user_desc_max_len = 0;
    /*user description permissions*/
    heart_rate_service->serv_chars[1].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*client config permissions*/
    heart_rate_service->serv_chars[1].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*server config permissions*/
    heart_rate_service->serv_chars[1].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*user desc handles*/
    heart_rate_service->serv_chars[1].user_desc_handle = 0;
    /*client config handles*/
    heart_rate_service->serv_chars[1].client_config_handle = 0;
    /*server config handles*/
    heart_rate_service->serv_chars[1].server_config_handle = 0;
    /* presentation format */
    heart_rate_service->serv_chars[1].presentation_format = NULL;

    /* Characteristic Info for Heart Rate Control Point */
    /* handle stored here */
    heart_rate_service->serv_chars[2].char_val_handle = 0;
    heart_rate_service->serv_chars[2].uuid.type = AT_BLE_UUID_16;
    /* UUID : Heart Rate Control Point*/
    heart_rate_service->serv_chars[2].uuid.uuid[0] = (uint8_t)HEART_RATE_CONTROL_POINT_CHAR_UUID;
    /* UUID : Heart Rate Control Point*/
    heart_rate_service->serv_chars[2].uuid.uuid[1] = (uint8_t)(HEART_RATE_CONTROL_POINT_CHAR_UUID >> 8);
    /* Properties */
    heart_rate_service->serv_chars[2].properties = AT_BLE_CHAR_WRITE;

    /* Initial Value */
    heart_rate_service->serv_chars[2].init_value = &heart_rate_control_point_value;

    heart_rate_service->serv_chars[2].value_init_len = sizeof(uint8_t);
    heart_rate_service->serv_chars[2].value_max_len = sizeof(uint8_t);

    /* permissions */
    heart_rate_service->serv_chars[2].value_permissions = (AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);

    /* user defined name */
    heart_rate_service->serv_chars[2].user_desc = NULL;
    heart_rate_service->serv_chars[2].user_desc_len = 0;
    heart_rate_service->serv_chars[2].user_desc_max_len = 0;
    /*user description permissions*/
    heart_rate_service->serv_chars[2].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*client config permissions*/
    heart_rate_service->serv_chars[2].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*server config permissions*/
    heart_rate_service->serv_chars[2].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    /*user desc handles*/
    heart_rate_service->serv_chars[2].user_desc_handle = 0;
    /*client config handles*/
    heart_rate_service->serv_chars[2].client_config_handle = 0;
    /*server config handles*/
    heart_rate_service->serv_chars[2].server_config_handle = 0;
    /* presentation format */
    heart_rate_service->serv_chars[2].presentation_format = NULL;
}

void heart_rate_service_define(heart_rate_gatt_service_handler_t * service) {
  at_ble_status_t status;

    status = at_ble_primary_service_define(&service->serv_uuid,
        &service->serv_handle, NULL, 0,
        service->serv_chars, HEART_RATE_TOTAL_CHARATERISTIC_NUM);

    if (status != AT_BLE_SUCCESS) {
        printf("Heart Rate Service definition Failed,reason: %x\n\r",
                status);
    } else {
        printf("Heart Rate service defined succesfully\n\r");
        printf(" Handle %d \n\r",service->serv_handle);
    } 
}

//Heart Rate GATT events
static at_ble_status_t heart_rate_notification_confirmed_app_event(void *param)
{
    at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
    if(!notification_status->status)
    {
        //printf("Heart Rate sending notification to the peer success\n\r");
    }
    return AT_BLE_SUCCESS;
}

static at_ble_status_t heart_rate_char_changed_app_event(void *param)
{
    //printf("heart_rate_char_changed_app_event\n\r");
    at_ble_characteristic_changed_t *change_param = (at_ble_characteristic_changed_t *)param;

    if (change_param->char_handle == heart_rate_service_handle.serv_chars[0].client_config_handle) {
        //printf("This *is* the Heart Rate heart_rate_char_changed_app_event\nmr");
        //printf(" newvalue = %d\n\r",change_param->char_new_value[0]);
         if (change_param->char_new_value[0] == HEART_RATE_NOTIFY_ON) {
            Heart_Rate_Notification_Flag = true;
            printf("Heart_Rate_Notification_Flag (true)\n\r");
        }
        if (change_param->char_new_value[0] == HEART_RATE_NOTIFY_OFF) {
            Heart_Rate_Notification_Flag = false;
            printf("Heart_Rate_Notification_Flag (false)\n\r");
        }
    }

    if (change_param->char_handle == heart_rate_service_handle.serv_chars[2].char_val_handle) {
        if (change_param->char_new_value[0] == true) {
            buzz_start(2);
        }
    }


    return AT_BLE_SUCCESS;
}

static at_ble_status_t hr_gatt1_event(void *param) {
    //printf("indication sent success\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t hr_gatt2_event(void *param) {
    printf("hr_gatt2_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t hr_gatt3_event(void *param) {
    printf("hr_gatt3_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t hr_gatt4_event(void *param) {
    printf("hr_gatt4_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t hr_gatt5_event(void *param) {
    printf("hr_gatt5_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t hr_gatt6_event(void *param) {
    printf("hr_gatt6_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t hr_gatt7_event(void *param) {
    printf("hr_gatt7_event\n\r");
    return AT_BLE_SUCCESS;
}
static at_ble_status_t hr_gatt8_event(void *param) {
    printf("hr_gatt8_event\n\r");
    return AT_BLE_SUCCESS;
}

const ble_event_callback_t gatt_heart_rate_cbs[] = {
    heart_rate_notification_confirmed_app_event,
    hr_gatt1_event,
    heart_rate_char_changed_app_event,
    hr_gatt2_event,
    hr_gatt3_event,
    hr_gatt4_event,
    hr_gatt5_event,
    hr_gatt6_event,
    hr_gatt7_event,
    hr_gatt8_event
};

// APIs
void heart_rate_set_value(uint8_t value) {
at_ble_status_t status;

    uint8_t hr_data[HEART_RATE_CHAR_MAX_LEN];
    memset(hr_data,0,HEART_RATE_CHAR_MAX_LEN);

    #define RR_INTERVAL_VALUE_PRESENT       (0x1 << 4)
    hr_data[0] = RR_INTERVAL_VALUE_PRESENT; //flags
    hr_data[1] = value;
  //  hr_data[2] = 0; hr_data[3] = 0;         //Energy expended
    hr_data[2] = 0; hr_data[3] = 0;         //RR value 1
    hr_data[4] = 0; hr_data[5] = 0;         //RR value 2


    if ((status = at_ble_characteristic_value_set(heart_rate_service_handle.serv_chars[0].char_val_handle, hr_data, HEART_RATE_CHAR_MAX_LEN)) != AT_BLE_SUCCESS){
        printf("Heart Rate updating the characteristic failed%d\n\r",status);
    } else {
        //printf("Heart Rate updating the characteristic value is successful\n\r");
    }

#if 1
  //  if (master_connection_handle) { 
        if((status = at_ble_notification_send(master_connection_handle,
        //if((status = at_ble_indication_send(master_connection_handle,
               heart_rate_service_handle.serv_chars[0].char_val_handle)) != AT_BLE_SUCCESS) {
            printf("Heart Rate sending notification failed%d\n\r",status);
        }
        else {
            printf("Heart Rate sending notification successful\n\r");
        }
   // } 
#endif
}

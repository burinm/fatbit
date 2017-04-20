#include <asf.h>
#include "interrupt_sam_nvic.h"
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "button.h"
#include "uart_samb11.h"
#include "samb11_xplained_pro.h"
#include "hr_sensor.h"

//#include "at30tse75x.h"

#include "startup_template_app.h"
#include "s_message.h"
#include "circbuf_tiny.h"

//Heart Rate defines
#define HR_CHAR_VALUE_LEN                                                       (10)
#define HR_VALUE_FORMAT_UINT16                                              (0x1 << 0)
#define SENSOR_CONTACT_FTR_NOT_SPRTD_NOT_CONTACT                            (0x1 << 2)
#define SENSOR_CONTACT_FTR_NOT_SPRTD_CONTACT_DTD                            (0x3 << 1)
#define ENERGY_EXPENDED_FIELD_PRESENT                                       (0x1 << 3)
#define RR_INTERVAL_VALUE_PRESENT                                           (0x1 << 4)


//incoming message queue, circular buffer
//Needs critical protection
circbuf_tiny_t M_Q;


volatile bool Temp_Notification_Flag = false;
static void htp_temperature_send(float);
static void hr_measurment_send(uint8_t rate);

static at_ble_status_t app_htpt_cfg_indntf_ind_handler(void *params);
static at_ble_status_t app_hr_cfg_indntf_ind_handler(void *params);

static void app_reset_handler(void);
static void app_notification_handler(uint8_t notification_enable);

//at_ble_status_t app_notification_cfm_handler(void *params);



static const ble_event_callback_t app_htpt_handle[] = {
    NULL, // AT_BLE_HTPT_CREATE_DB_CFM
    NULL, // AT_BLE_HTPT_ERROR_IND
    NULL, // AT_BLE_HTPT_DISABLE_IND
    NULL, // AT_BLE_HTPT_TEMP_SEND_CFM
    NULL, // AT_BLE_HTPT_MEAS_INTV_CHG_IND
    app_htpt_cfg_indntf_ind_handler, // AT_BLE_HTPT_CFG_INDNTF_IND
    NULL, // AT_BLE_HTPT_ENABLE_RSP
    NULL, // AT_BLE_HTPT_MEAS_INTV_UPD_RSP
    NULL // AT_BLE_HTPT_MEAS_INTV_CHG_REQ
};

static const ble_event_callback_t app_hr_handle[] = {
    NULL, // AT_BLE_HTPT_CREATE_DB_CFM
    NULL, // AT_BLE_HTPT_ERROR_IND
    NULL, // AT_BLE_HTPT_DISABLE_IND
    NULL, // AT_BLE_HTPT_TEMP_SEND_CFM
    NULL, // AT_BLE_HTPT_MEAS_INTV_CHG_IND
    app_hr_cfg_indntf_ind_handler, // AT_BLE_HTPT_CFG_INDNTF_IND
    NULL, // AT_BLE_HTPT_ENABLE_RSP
    NULL, // AT_BLE_HTPT_MEAS_INTV_UPD_RSP
    NULL // AT_BLE_HTPT_MEAS_INTV_CHG_REQ
};


volatile bool Timer_Flag = false;
static void timer_callback_handler(void);

volatile at_ble_status_t status;

static void ble_advertise (void);

at_ble_handle_t htpt_conn_handle;
static void htp_init (void);

//GAP callbacks
static at_ble_status_t ble_paired_cb (void *param);
static at_ble_status_t ble_disconnected_cb (void *param);
static void register_ble_callbacks (void);

static const ble_event_callback_t app_gap_cb[] = {
    NULL, // AT_BLE_UNDEFINED_EVENT
    NULL, // AT_BLE_SCAN_INFO
    NULL, // AT_BLE_SCAN_REPORT
    NULL, // AT_BLE_ADV_REPORT
    NULL, // AT_BLE_RAND_ADDR_CHANGED
    NULL, // AT_BLE_CONNECTED
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

#if 0
//New for heart rate - 
static const ble_event_callback_t app_gatt_server_handle[] = {
    app_notification_cfm_handler,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif

void configure_gpio(void);

int main (void)
{

#if 1
    platform_driver_init();
    acquire_sleep_lock();
    /* Initialize serial console */
    serial_console_init();
    /* Hardware timer */
    hw_timer_init();
    /* Register the callback */
    hw_timer_register_callback(timer_callback_handler);
    /* Start timer */
    hw_timer_start(1);

    printf("\n\rSAMB11 BLE Application");
    /* initialize the BLE chip and Set the Device Address */
    ble_device_init(NULL);
    
    /* Initialize the temperature service */
    htp_init();
    /* Initialize the heart rate service */
    hr_sensor_init(NULL);
    /* Registering the app_notification_handler with the profile */
    register_hr_notification_handler(app_notification_handler);
    /* Registering the app_reset_handler with the profile */
    register_hr_reset_handler(app_reset_handler);

    /* Register Bluetooth events Callbacks */
    register_ble_callbacks();

    
    /* Start Advertising process */
    ble_advertise();
#endif

    configure_gpio();
    LED_Off(LED0);
    LED_On(LED0);


    /* UART with DMA */
    circbuf_tiny_init(&M_Q);
    system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);
    //SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    uart_setup();
    //uart_write_wait(&my_uart_instance, 'a');
    const char *hello_string="\r\nInitialized UART1\r\n";
    uart_write_buffer_wait(&my_uart_instance,hello_string,strlen(hello_string));
    dma_start_transfer_job(&uart_dma_resource_rx);

#if 0
    uint8_t c;
    while(1) {

        //Does not work - buffering issue?
        if (uart_read_wait(&my_uart_instance, &c) == STATUS_OK) {
            while (uart_write_wait(&my_uart_instance, c) != STATUS_OK) {
            }
        }

    }
#endif
    
    while(true) {
        ble_event_task(655);
        if (Timer_Flag & Temp_Notification_Flag)
        {
            //htp_temperature_send(at30tse_read_temperature());
        }


    }
}

static void ble_advertise (void)
{
    printf("\nAssignment 2.1 : Start Advertising");
    status = ble_advertisement_data_set();
    if(status != AT_BLE_SUCCESS)
    {
        printf("\n\r## Advertisement data set failed : error %x",status);
        while(1);
    }
    /* Start of advertisement */
    status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,\
    AT_BLE_ADV_GEN_DISCOVERABLE,\
    NULL,\
    AT_BLE_ADV_FP_ANY,\
    1000,\
    655,\
    0);
    if(status != AT_BLE_SUCCESS)
    {
        printf("\n\r## Advertisement data set failed : error %x",status);
        while(1);
    }
}

/* Callback registered for AT_BLE_CONNECTED event*/
static at_ble_status_t ble_paired_cb (void *param)
{
    at_ble_pair_done_t *pair_params = param;
    printf("\nAssignment 3.2: Application paired ");
    /* Enable the HTP Profile */
    printf("\nAssignment 4.1: enable health temperature service ");
    status = at_ble_htpt_enable(pair_params->handle,
    HTPT_CFG_INTERM_MEAS_NTF);
    if(status != AT_BLE_SUCCESS){
        printf("*** Failure in HTP Profile Enable");
        while(true);
    }

    ALL_UNUSED(param);
    return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event */
static at_ble_status_t ble_disconnected_cb (void *param)
{
    printf("\nAssignment 3.2: Application disconnected ");
    ble_advertise();
    ALL_UNUSED(param);return AT_BLE_SUCCESS;
}

/* Register GAP callbacks at BLE manager level*/
static void register_ble_callbacks (void)
{
    /* Register GAP Callbacks */
    printf("\nAssignment 3.2: Register bluetooth events callbacks");
    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,\
    BLE_GAP_EVENT_TYPE,app_gap_cb);
    if (status != true) {
        printf("\n##Error when Registering SAMB11 gap callbacks");
    }

#if 0
    printf("\nAssignment 3.2: Register bluetooth gat callbacks");
    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,\
    BLE_GATT_SERVER_EVENT_TYPE,app_gatt_server_handle);
    if (status != true) {
        printf("\n##Error when Registering gatt callbacks");
    }
#endif
    
    // Temperature service
    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,\
    BLE_GATT_HTPT_EVENT_TYPE,app_htpt_handle);
    if (status != true) {
        printf("\n##Error when Registering SAMB11 htpt callbacks");
    }

    // Heart Rate service
    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,\
    BLE_GATT_HTPT_EVENT_TYPE,app_hr_handle);
    if (status != true) {
        printf("\n##Error when Registering SAMB11 htpt callbacks");
    }
}

static void htp_init (void)
{
    printf("\nAssignment 4.1: Init Health temperature service ");
    /* Create htp service in GATT database*/
    status = at_ble_htpt_create_db(
    HTPT_TEMP_TYPE_CHAR_SUP,
    HTP_TYPE_ARMPIT,
    1,
    30,
    1,
    //HTPT_AUTH,
    HTPT_UNAUTH,
    &htpt_conn_handle);
    if (status != AT_BLE_SUCCESS){
        printf("HTP Data Base creation failed");
        while(true);
    }
}

/* Timer callback */
static void timer_callback_handler(void)
{
    /* Stop timer */
    hw_timer_stop();
printf("(?)");

        //Check for incoming messages
        s_message *message=NULL;
        uint8_t is_entry;
cpu_irq_enter_critical();
        is_entry = circbuf_tiny_read(&M_Q,(uint32_t**)&message);
cpu_irq_leave_critical();
        if (is_entry) {
            if (message) {
printf("message pulled off queue [%s], ",message->message);

                switch(s_get_message_type(message)) {
                case S_LED_ON:
                    printf("[Turn on LED]\n");
                    LED_On(LED0);
                break;

                case S_LED_OFF:
                    printf("[Turn off LED]\n");
                    LED_Off(LED0);
                break;

                case S_TEMP:
                    printf("[Set temp]\n");
                    //htp_temperature_send(s_message_get_value(message));
                break;

                case S_SUN:
                    printf("[Sunlight Level]\n");
                    htp_temperature_send(s_message_get_value(message));
                break;

                case S_PULSE:
                    printf("[Pulse Rate]\n");
                    hr_measurment_send(s_message_get_value(message));
                break;

                case S_NONE:
                    printf("[S_NONE]\n");
                break;
    
                default:
                    printf("[UNKOWN!!]\n");

            }

            //Note, the pointers in the circular buffer still exist
            // do not double free
            free(message);

            } else {
                printf("NULL message pulled off queue!!\n");
            }
        }

    /* Set timer Alarm flag */
    Timer_Flag = true;
    /* Restart Timer */
    hw_timer_start(1);
}

/* Send the temperature value */
static void htp_temperature_send(float temperature)
{
    at_ble_prf_date_time_t timestamp;
    #ifdef HTPT_FAHRENHEIT
    temperature = (((temperature * 9.0)/5.0) + 32.0);
    #endif
    /* Read Temperature Value from IO1 Xplained Pro */
    timestamp.day = 1;
    timestamp.hour = 9;
    timestamp.min = 2;
    timestamp.month = 8;
    timestamp.sec = 36;
    timestamp.year = 15;
    /* Read Temperature Value from IO1 Xplained Pro */
    if(at_ble_htpt_temp_send(convert_ieee754_ieee11073_float((float)temperature),
    &timestamp,
    #ifdef HTPT_FAHRENHEIT
    (at_ble_htpt_temp_flags)(HTPT_FLAG_FAHRENHEIT | HTPT_FLAG_TYPE),
    #else
    (at_ble_htpt_temp_flags)(HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE),
    #endif
    HTP_TYPE_ARMPIT,
    1
    ) == AT_BLE_SUCCESS)
    {
        #ifdef HTPT_FAHRENHEIT
        printf("\n(sending) Temperature: %d Fahrenheit\r\n", (uint16_t)temperature);
        #else
        printf("\n(sending) Temperature: %d Deg Celsius\r\n", (uint16_t)temperature);
        #endif
    }
}

/* Sent heartrate value */
static void hr_measurment_send(uint8_t rate)
{
    uint8_t hr_data[HR_CHAR_VALUE_LEN];
    uint8_t idx = 0;

    memset(hr_data,0,HR_CHAR_VALUE_LEN);

    hr_data[0] = RR_INTERVAL_VALUE_PRESENT; //flags
    hr_data[1] = rate; 
    hr_data[2] = 0; hr_data[3] = 0;         //Energy expended
    hr_data[4] = 0; hr_data[5] = 0;         //RR value 1
    hr_data[6] = 0; hr_data[7] = 0;         //RR value 2
    
    
  /* Sending the data for notifications*/
    hr_sensor_send_notification(hr_data, 8);
}

static at_ble_status_t app_htpt_cfg_indntf_ind_handler(void *params)
{
    at_ble_htpt_cfg_indntf_ind_t htpt_cfg_indntf_ind_params;
    memcpy((uint8_t *)&htpt_cfg_indntf_ind_params, params,
    sizeof(at_ble_htpt_cfg_indntf_ind_t));
    if (htpt_cfg_indntf_ind_params.ntf_ind_cfg == 0x03) {
        printf("Started HTP Temperature Notification");
        Temp_Notification_Flag = true;
    }
    else {
        printf("HTP Temperature Notification Stopped");
        Temp_Notification_Flag = false;
    }
    return AT_BLE_SUCCESS;
}

//Heart rate callback
static at_ble_status_t app_hr_cfg_indntf_ind_handler(void *params)
{
    at_ble_htpt_cfg_indntf_ind_t htpt_cfg_indntf_ind_params;
    memcpy((uint8_t *)&htpt_cfg_indntf_ind_params, params,
    sizeof(at_ble_htpt_cfg_indntf_ind_t));
    if (htpt_cfg_indntf_ind_params.ntf_ind_cfg == 0x03) {
        printf("Started HTP Heart Rate Notification");
        Temp_Notification_Flag = true;
    }
    else {
        printf("HTP Heart Rate Notification Stopped");
        Temp_Notification_Flag = false;
    }
    return AT_BLE_SUCCESS;
}


void configure_gpio(void) { 
    struct gpio_config config_gpio;
    gpio_get_config_defaults(&config_gpio);
    config_gpio.direction  = GPIO_PIN_DIR_OUTPUT;
    config_gpio.input_pull = GPIO_PIN_PULL_NONE;
    gpio_pin_set_config(LED0_GPIO, &config_gpio);

    //#define LED0_PINMUX PINMUX_LP_GPIO_22_GPIO this throws overflow warning
    #define LED0_PINMUX MUX_LP_GPIO_22_GPIO
    gpio_pinmux_cofiguration(LED0_GPIO,LED0_PINMUX);
}

// Heart Rate callbacks
/** Notification handler function called by the profile
 *  notification_enable which will tell the state of the
 *  application
 */
static void app_notification_handler(uint8_t notification_enable)
{
    if (notification_enable == true) {
        printf("app_notification_handler (true)\n\r");
        //DBG_LOG("Notification Enabled");
        //hw_timer_start(NOTIFICATION_INTERVAL);
    } else {
        printf("app_notification_handler (false)\n\r");
        //hw_timer_stop();
        //notification_flag = false;
        //DBG_LOG("Notification Disabled");
    }
}

/** Energy expended handler called by profile to reset the energy values
 *
 */
static void app_reset_handler(void)
{

    printf("app_reset_handler\n\r");
#if 0
    energy_expended_val = 0;
    DBG_LOG("Energy Expended is made '0'on user Reset");
#endif
}

#if 0
/** hr_notification_confirmation_handler called by ble manager 
 *  to give the status of notification sent
 *  at_ble_cmd_complete_event_t address of the cmd completion
 */
at_ble_status_t app_notification_cfm_handler(void *params)
{
    at_ble_cmd_complete_event_t event_params;
    memcpy(&event_params,params,sizeof(at_ble_cmd_complete_event_t));
    if (event_params.status == AT_BLE_SUCCESS) {
        printf("App Notification Successfully sent over the air");
        //notification_sent = true;
    } else {
        printf("Sending Notification over the air failed");
        //notification_sent = false;
    }
    return AT_BLE_SUCCESS;
}
#endif


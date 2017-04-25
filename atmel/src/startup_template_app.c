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
#include "sunlight_service.h"

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

//GPIO
#define LED_GPIO        LED0_GPIO
#define LED_PINMUX      MUX_LP_GPIO_22_GPIO
#define BUZZER_GPIO     EXT3_PIN_5
#define BUZZER_GPIO_MUX MUX_LP_GPIO_20_GPIO

#define GPIO_ON(x)      gpio_pin_set_output_level(x,true)
#define GPIO_OFF(x)     gpio_pin_set_output_level(x,false)


void ble_advertise(void);

//incoming message queue, circular buffer
//Needs critical protection
circbuf_tiny_t M_Q;


void process_command_messages(void);

//Leave these volatile if they are modified in interrupt routine
static volatile uint8_t Timer_loop_count = 0;
volatile bool Temperature_Notification_Flag = false;
volatile bool HeartRate_Notification_Flag = false;

static void htp_temperature_send(float);
static void htp_sunlight_send(float);
static void hr_measurment_send(uint8_t rate);
static void buzz_start(uint8_t times);


static void heart_rate_reset_handler(void);
static void app_notification_handler(uint8_t notification_enable);

void hw_timer_start_ms(uint32_t delay);
static void timer_callback_handler(void);
static void htp_init(void);

//App handler
static at_ble_status_t app_cfg_indntf_ind_handler(void *params);

//GAP callbacks
static at_ble_status_t ble_paired_cb (void *param);
static at_ble_status_t ble_disconnected_cb (void *param);
static void register_ble_callbacks (void);

void configure_gpio(void);


at_ble_handle_t htpt_conn_handle;

static at_ble_status_t post_heart_rate(void *params);

at_ble_status_t gatt_notification_cfm_handler(void *params);

static const ble_event_callback_t app_gap_handle[] = {
    NULL, // AT_BLE_UNDEFINED_EVENT
    NULL, // AT_BLE_SCAN_INFO
    NULL, // AT_BLE_SCAN_REPORT
    NULL, // AT_BLE_ADV_REPORT
    NULL, // AT_BLE_RAND_ADDR_CHANGED
    hr_sensor_connected_state_handler, // AT_BLE_CONNECTED
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

static const ble_event_callback_t gatt_server_handle[] = {
    gatt_notification_cfm_handler,
    NULL,
    hr_sensor_char_changed_handler,
    NULL,
    NULL,
    hr_sensor_char_write_request,
    NULL,
    NULL,
    NULL,
    NULL
};

static const ble_event_callback_t gatt_server_handle2[] = {
    mystery1,
    mystery2,
    sunlight_char_changed_handler,
    mystery3,
    mystery4,
    mystery5,
    mystery6,
    mystery7,
    mystery8,
    mystery9
};

static const ble_event_callback_t custom_event_handle[] = {
   post_heart_rate
}; 

at_ble_status_t gatt_notification_cfm_handler(void *params)
{
printf("gatt_notification_cfm_handler\n\r");

    at_ble_cmd_complete_event_t event_params;
    memcpy(&event_params,params,sizeof(at_ble_cmd_complete_event_t));
    if (event_params.status == AT_BLE_SUCCESS) {
        printf("(sent) App Notification Successfully sent over the air\n\r");
    } else {
        printf("Sending Notification over the air failed\n\r");
    }
    return AT_BLE_SUCCESS;
}


static at_ble_status_t post_heart_rate(void *params) {
    printf("Restart timer post (Timer_loop_count=%d)\n\r",Timer_loop_count);
    buzz_start(2); 
    return AT_BLE_SUCCESS;
}

static const ble_event_callback_t app_handle[] = {
    NULL, // AT_BLE_HTPT_CREATE_DB_CFM
    NULL, // AT_BLE_HTPT_ERROR_IND
    NULL, // AT_BLE_HTPT_DISABLE_IND
    NULL, // AT_BLE_HTPT_TEMP_SEND_CFM
    NULL, // AT_BLE_HTPT_MEAS_INTV_CHG_IND
    app_cfg_indntf_ind_handler, // AT_BLE_HTPT_CFG_INDNTF_IND
    NULL, // AT_BLE_HTPT_ENABLE_RSP
    NULL, // AT_BLE_HTPT_MEAS_INTV_UPD_RSP
    NULL // AT_BLE_HTPT_MEAS_INTV_CHG_REQ
};

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

    printf("\n\rSAMB11 BLE Application");
    /* initialize the BLE chip and Set the Device Address */
    ble_device_init(NULL);
    printf("\n\r");

    /* Initialize the temperature service */
    htp_init();

    
    /* Initialize the heart rate service */
    hr_sensor_init(NULL);

    /* Initialize the sunlight service */
    //sunlight_service_init(&sunlight_service_handle);
    //sunlight_service_define(&sunlight_service_handle);
    

    /* Registering the app_notification_handler with the profile */
    register_hr_notification_handler(app_notification_handler);
    /* Registering the heart_rate_reset_handler with the profile */
    register_hr_reset_handler(heart_rate_reset_handler);


    /* Register Bluetooth events Callbacks */
    register_ble_callbacks();

    /* Start Advertising process */
    ble_advertise();
#endif

    configure_gpio();
    LED_Off(LED_GPIO);
    GPIO_OFF(BUZZER_GPIO);

    /* UART with DMA */
    circbuf_tiny_init(&M_Q);
    system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);
    uart_setup();
    const char *hello_string="\r\nInitialized UART1\r\n";
    uart_write_buffer_wait(&my_uart_instance,hello_string,strlen(hello_string));
    dma_start_transfer_job(&uart_dma_resource_rx);

    while(true) {
        ble_event_task(100);
//sunlight_send_notification(123);
        process_command_messages();
    }
}

void process_command_messages() {
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
                    LED_On(LED_GPIO);
                break;

                case S_LED_OFF:
                    printf("[Turn off LED]\n");
                    LED_Off(LED_GPIO);
                break;

                case S_TEMP:
                    printf("[Set temp]\n");
                    #if 0
                    if (Temperature_Notification_Flag) {
                        htp_temperature_send(s_message_get_value(message));
                    } else {
                        printf("(temp discarded)\n");
                    }
                    #endif
                break;

                case S_SUN:
                    printf("[Sunlight Level]\n");
                    if (Temperature_Notification_Flag) {
                        htp_sunlight_send(s_message_get_value(message));
                    } else {
                        printf("(sunlight discarded)\n");
                    }
                    #if 0
                    if (Sunlight_Notification_Flag) {
                        sunlight_send_notification(s_message_get_value(message));
                        //htp_sunlight_send(s_message_get_value(message));
                    } else {
                        printf("(sun discarded)\n");
                    }
                    #endif
                break;

                case S_PULSE:
                    printf("[Pulse Rate]\n");
                    if (HeartRate_Notification_Flag) {
                        hr_measurment_send(s_message_get_value(message));
                    } else {
                        printf("(hr discarded)\n");
                    }
                break;

                case S_NOTIFY:
                    printf("[NOTIFY ALERT]\n");
                    buzz_start(s_message_get_value(message));
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
}

void ble_advertise (void)
{
at_ble_status_t status;

    printf("Start Advertising (initial)\n\r");
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
at_ble_status_t status;

    at_ble_pair_done_t *pair_params = param;
    printf("\n\rApplication paired\n\r");

    /* Enable the HTP Profile */
    printf("Enable health temperature service\n\r");
    status = at_ble_htpt_enable(pair_params->handle,
    HTPT_CFG_INTERM_MEAS_NTF);
    if(status != AT_BLE_SUCCESS){
        printf("*** Failure in HTP Profile Enable");
        while(true);
    }

    return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event */
static at_ble_status_t ble_disconnected_cb (void *param)
{
    printf("\n\rAT_BLE_DISCONNECTED\n\r");
    ble_advertise();
    return AT_BLE_SUCCESS;
}

/* Register GAP callbacks at BLE manager level*/
static void register_ble_callbacks (void)
{
at_ble_status_t status;

    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
    BLE_GAP_EVENT_TYPE,
    app_gap_handle);
    if (status != true) {
        printf("\n##Error when Registering gap callbacks");
    }

#if 1
    //Gatt callbacks
    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
    BLE_GATT_SERVER_EVENT_TYPE,
    gatt_server_handle);
    if (status != true) {
        printf("\n##Error when Registering gatt callbacks");
    }
#endif

#if 0
    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
    BLE_GATT_SERVER_EVENT_TYPE,
    gatt_server_handle2);
    if (status != true) {
        printf("\n##Error when Registering gatt callbacks");
    }
#endif

    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
    BLE_CUSTOM_EVENT_TYPE,
    custom_event_handle);
    if (status != true) {
        printf("\n##Error when Registering custom callback");
    }

    // App callback, may only register one of these
    status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
    BLE_GATT_HTPT_EVENT_TYPE, app_handle);
    if (status != true) {
        printf("\n##Error when registering app callbacks");
    }

}

static void htp_init (void)
{
at_ble_status_t status;

    printf("Init Health temperature service\n\r");
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

static void buzz_start(uint8_t times) {
cpu_irq_enter_critical();
    //Add alerts
    Timer_loop_count += (times * 2) + 1;
cpu_irq_leave_critical();

    hw_timer_start_ms(100);
}

/* Timer callback - Buzzer control*/
static void timer_callback_handler(void)
{
    /* Stop timer */
    hw_timer_stop();

    if (Timer_loop_count == 1) {
        GPIO_OFF(BUZZER_GPIO);
        LED_Off(LED_GPIO);
        return;
    } 

    switch(Timer_loop_count % 2) { //Even or Odd
        case 0:
            GPIO_OFF(BUZZER_GPIO);
            LED_Off(LED_GPIO);
            /* Start timer */
            hw_timer_start_ms(100);
            break;
        case 1:
            GPIO_ON(BUZZER_GPIO);
            LED_On(LED_GPIO);
            /* Start timer */
            hw_timer_start_ms(200);
            break;
    };

    if (Timer_loop_count) {
        Timer_loop_count--;
    }
}

void configure_gpio(void) { 
    struct gpio_config config_gpio;
    gpio_get_config_defaults(&config_gpio);
    config_gpio.direction  = GPIO_PIN_DIR_OUTPUT;
    config_gpio.input_pull = GPIO_PIN_PULL_NONE;

    //On Board LED
    gpio_pin_set_config(LED_GPIO, &config_gpio);
    gpio_pinmux_cofiguration(LED_GPIO,LED_PINMUX);

    //Haptic Feedback Buzzer
    gpio_pin_set_config(BUZZER_GPIO, &config_gpio);
    gpio_pinmux_cofiguration(BUZZER_GPIO,BUZZER_GPIO_MUX);
}


/* Send the temperature value */
#if 1
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
#endif

/* Send the sunlight value in the temperature field*/
static void htp_sunlight_send(float sun)
{
    at_ble_prf_date_time_t timestamp;
    timestamp.day = 1;
    timestamp.hour = 9;
    timestamp.min = 2;
    timestamp.month = 8;
    timestamp.sec = 36;
    timestamp.year = 15;
    if(at_ble_htpt_temp_send(convert_ieee754_ieee11073_float((float)sun),
    &timestamp,
    (at_ble_htpt_temp_flags)(HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE),
    HTP_TYPE_ARMPIT,
    1
    ) == AT_BLE_SUCCESS)
    {
        printf("\n(sending) Sunlight: %d\r\n", (uint16_t)sun);
    }
}

/* Send heartrate value */
static void hr_measurment_send(uint8_t rate)
{
    uint8_t hr_data[HR_CHAR_VALUE_LEN];
    memset(hr_data,0,HR_CHAR_VALUE_LEN);

    hr_data[0] = RR_INTERVAL_VALUE_PRESENT; //flags
    hr_data[1] = rate; 
  //  hr_data[2] = 0; hr_data[3] = 0;         //Energy expended
    hr_data[2] = 0; hr_data[3] = 0;         //RR value 1
    hr_data[4] = 0; hr_data[5] = 0;         //RR value 2
    
  /* Sending the data for notifications*/
    hr_sensor_send_notification(hr_data, 6);
}

static at_ble_status_t app_cfg_indntf_ind_handler(void *params)
{
 
    at_ble_htpt_cfg_indntf_ind_t app_cfg_indntf_ind_params;
    memcpy((uint8_t *)&app_cfg_indntf_ind_params, params, sizeof(at_ble_htpt_cfg_indntf_ind_t));

printf("app_cfg_indntf_ind_handler 0x%x\n\r",app_cfg_indntf_ind_params.conhdl);
printf("app_cfg_indntf_ind_handler config %d\n\r",app_cfg_indntf_ind_params.ntf_ind_cfg);

    if (app_cfg_indntf_ind_params.ntf_ind_cfg == 0x03) {
        printf("Started Temperature Notification\n\r");
        Temperature_Notification_Flag = true;
    }
    else {
        printf("Temperature Notification Stopped\n\r");
        Temperature_Notification_Flag = false;
    }
    return AT_BLE_SUCCESS;
}

// Heart Rate notification callback
static void app_notification_handler(uint8_t notification_enable)
{
    if (notification_enable == true) {
        printf("Heart Rate notification (true)\n\r");
        HeartRate_Notification_Flag = true;
    } else {
        printf("Heart Rate notification (false)\n\r");
        HeartRate_Notification_Flag = false;
    }
}

/* Write notification */
static void heart_rate_reset_handler(void)
{
    printf("heart_rate_reset_handler\n\r");
    //This will blink LED/BUZZ haptic
    buzz_start(2); 
}

void hw_timer_start_ms(uint32_t delay)
{       
    if(delay <= 0) {
        delay = 1;
    }

    dualtimer_set_counter(DUALTIMER_TIMER1,DUALTIMER_SET_CURRUNT_REG,26000*delay);
    dualtimer_enable(DUALTIMER_TIMER1);
}

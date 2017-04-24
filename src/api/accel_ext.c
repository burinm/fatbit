#include "accel_ext.h"
#include "../../../drivers/src/driver/mma8452q.h"
#include "em_gpio.h"
#include "em_core.h"
#include "main.h"

#ifdef LCD_MESSAGES
    #include "segmentlcd.h"
    #include "letimer.h"
#endif

/* Global indicates if I2C communication successfully 
    opened accelerometer device. 0=no, 1=yes
*/
uint8_t accel_connected = 0;

void accel_sensor_power_on() {
//Turn power on
GPIO_PinModeSet(ACCEL_SENSOR_POWER_PORT, ACCEL_SENSOR_POWER_PORT_NUM,
    gpioModePushPullDrive, 1);
GPIO_DriveModeSet(ACCEL_SENSOR_POWER_PORT,gpioDriveModeLow);

//Need to wait for power up
}

void accel_sensor_program() {
//Initialize GPIO PINS/I2C reset
mma8452q_open();

//Power up register on
if (mma8452q_on(1) > 0) {
    accel_connected = 1;
} else {
    //Abort could not reach device
    return;
}

/* Setup Double Tap detection */
    //single tap, latch on, Z axis
mma8452q_write_register(MMA8452Q_REG_PULSE_CFG,
    MMA8452Q_PULSE_CFG_ELE | MMA8452Q_PULSE_CFG_ZDPEFE);

    //Set threshold, g force
mma8452q_write_register(MMA8452Q_REG_PULSE_THSZ,
    SINGTAP_SENSOR_G_THRESH_Z);

    //Set length of tap
mma8452q_write_register(MMA8452Q_REG_PULSE_TMLT,
    SINGTAP_TIME_LIMIT);

    //Set length until next tap
mma8452q_write_register(MMA8452Q_REG_PULSE_LTCY,
    SINGTAP_LATENCY);

    //Set window until double tap
mma8452q_write_register(MMA8452Q_REG_PULSE_WIND,
    DOUBLETAP_WINDOW);

/* Setup Motion detection */

    //motion, latch on, Z axis
mma8452q_write_register(MMA8452Q_REG_FF_MT_CFG,
    MMA8452Q_FF_MT_CFG_ELE | MMA8452Q_FF_MT_CFG_OAE | MMA8452Q_FF_MT_CFG_ZEFE);
mma8452q_write_register(MMA8452Q_REG_FF_MT_THS,
    //Set debounce counter to clear on non-event, set G threshold
    (ACCEL_SENSOR_G_THRESH & MMA8452Q_FF_MT_THS_MASK) |
    (MMA8452Q_FF_MT_THS_DBCNTM & MMA8452Q_FF_MT_THS_DBCNTM_DECREMENT));

/* Set debounce counter */

    //With ODR=6.25, Normal mode step = 20ms, 20ms * 10 = 200ms
mma8452q_write_register(MMA8452Q_REG_FF_MT_COUNT,
    ACCEL_SENSOR_DEBOUNCE_CNT);

/* Setup interrupts for mma8452q */

    //Active High interrupts, push/pull
mma8452q_write_register(MMA8452Q_REG_CTRL_REG3,
    //MMA8452Q_CTRL_REG3_IPOL | MMA8452Q_CTRL_REG3_PP_OD);
    MMA8452Q_CTRL_REG3_IPOL | MMA8452Q_CTRL_REG3_WAKE_FF_MT); //Active high, push pull, Freefall/Motion can wake from sleep

    //Freefall/Motion interrupt on, Pulse/Tap interrupt on
mma8452q_write_register(MMA8452Q_REG_CTRL_REG4,
    MMA8452Q_CTRL_REG4_INT_EN_FF_MT | MMA8452Q_CTRL_REG4_INT_EN_PULSE);
    //Route Freefall/Motion, Pulse/Tap  interrupt to INT1 pin
mma8452q_write_register(MMA8452Q_REG_CTRL_REG5,
    MMA8452Q_CTRL_REG5_INT_CFG_FF_MT | MMA8452Q_CTRL_REG5_INT_CFG_PULSE);

/* Setup mma8452q power on mode */

//Need to power on active last?

    //Set output data rate (ODR) 6.25Hz, sleep rate 6.25Hz, and set to Active
    // 6.25Hz^-1 = 160ms, which should be snappy
mma8452q_write_register(MMA8452Q_REG_CTRL_REG1,
    (MMA8452Q_CTRL_REG1_DR_6_25 << MMA8452Q_CTRL_REG1_DR_SHIFT) |
    (MMA8452Q_CTRL_REG1_ASLP_RATE_6_25 << MMA8452Q_CTRL_REG1_ASLP_RATE_SHIFT)
    | MMA8452Q_CTRL_REG1_ACTIVE
    );

//Reading this should clear the interrupt
mma8452q_read_register(MMA8452Q_REG_INT_SOURCE);
mma8452q_read_register(MMA8452Q_REG_FF_MT_SRC);

/* Setup interrupt input on Gecko board */
GPIO_PinModeSet(ACCEL_SENSOR_INT_PORT, ACCEL_SENSOR_INT_PORT_NUM,
    gpioModeInputPull, 0);

//Rising edge
GPIO_ExtIntConfig(ACCEL_SENSOR_INT_PORT, ACCEL_SENSOR_INT_PORT_NUM, ACCEL_SENSOR_INT_PORT_NUM,
    true, false, true);

// GPIO interrupts on
NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

//Must be called when interrupts are disabled??
void accel_sensor_power_off() {

//Disable interrupts
NVIC_DisableIRQ(GPIO_ODD_IRQn);

//Disable GPIO PINS - I2C pins, interrupt pin
if (accel_connected) {
    mma8452q_on(0); //Soft power off first, before we yank the cord
    mma8452q_close();
}

//Disable external interrupt line
GPIO_PinModeSet(ACCEL_SENSOR_INT_PORT, ACCEL_SENSOR_INT_PORT_NUM,
    gpioModeDisabled, 0);

//Disable power
GPIO_PinOutClear(ACCEL_SENSOR_POWER_PORT, ACCEL_SENSOR_POWER_PORT_NUM);
}

uint8_t accel_sensor_is_active() {
return ( (GPIO_PortInGet(ACCEL_SENSOR_INT_PORT) & (1 << ACCEL_SENSOR_INT_PORT_NUM)) == 0);
}

void GPIO_ODD_IRQHandler() {
int intFlags;
uint8_t int_src;
uint8_t mt_reason;

CORE_CriticalDisableIrq();

    intFlags = GPIO_IntGet();
    GPIO_IntClear(1<<ACCEL_SENSOR_INT_PORT_NUM);

    if ( intFlags & (1<<ACCEL_SENSOR_INT_PORT_NUM) ) {
        int_src = mma8452q_read_register(MMA8452Q_REG_INT_SOURCE);
        if (int_src & MMA8452Q_INT_SRC_FF_MNT) { //Motion/Freefall event
            //Reading this register clears it, if ELE was set in FF_MT_CFG
            mt_reason = mma8452q_read_register(MMA8452Q_REG_FF_MT_SRC);
            if (mt_reason & MMA8452Q_FF_MT_SRC_EA) { // FF_MT_CFG prereqs satisfied
                #ifdef LCD_MESSAGES
                    SegmentLCD_LowerNumber(motion_ticks);
                    lcd_motion_keep_on=3;
                    lcd_keep_on=3;
                #endif
                //led1_toggle();
            }
        }

        if (int_src & MMA8452Q_INT_SRC_PULSE) { // Pulse/Tap event
            mt_reason =  mma8452q_read_register(MMA8452Q_REG_PULSE_SRC);
            if (mt_reason & MMA8452Q_PULSE_SRC_AXZ) { // Z-axis event 
                SegmentLCD_Write("Reset");
                led0_off();
                led1_off();
                motion_ticks=MOTION_TICKS_TOP;
                lcd_motion_keep_on=2;
                lcd_keep_on=2;
                //led0_toggle();
            }
        }
        //If we setup different kinds of interrupts, the corresponding registers must be read
        // to clear them.
        //mma8452q_int_clear();
    }

CORE_CriticalEnableIrq();
}

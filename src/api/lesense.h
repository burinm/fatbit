#ifndef __LESENSE_H__
#define __LESENSE_H__

/* Most of this code lifted from the Cap sense touch demo, modified for
    a single touch event - wake up */

/******************************************************************************
 * License
 * Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#define CAP_ACMP_EXTERNAL_PORT        gpioPortC
#define CAP_ACMP_EXTERNAL_PIN         1

#define LESENSE_ACMP_VDD_SCALE         0x25 //1.29 25/63, (61/63 prescale)
#define CAPLESENSE_SENSITIVITY_OFFS    1U

#define CAPLESENSE_CHANNEL_INT        LESENSE_IF_CH1
#define LESENSE_CHANNEL_INPUT         1

#define LESENSE_DISABLED_CH_CONF \
  { \
    false,                    /* Disable scan channel. */ \
    false,                    /* Disable the assigned pin on scan channel. */ \
    false,                    /* Disable interrupts on channel. */ \
    lesenseChPinExDis,        /* GPIO pin is disabled during the excitation period. */ \
    lesenseChPinIdleDis,      /* GPIO pin is disabled during the idle period. */ \
    false,                    /* Don't use alternate excitation pins for excitation. */ \
    false,                    /* Disabled to shift results from this channel to the decoder register. */ \
    false,                    /* Disabled to invert the scan result bit. */ \
    false,                    /* Disabled to store counter value in the result buffer. */ \
    lesenseClkLF,             /* Use the LF clock for excitation timing. */ \
    lesenseClkLF,             /* Use the LF clock for sample timing. */ \
    0x00U,                    /* Excitation time is set to 5(+1) excitation clock cycles. */ \
    0x00U,                    /* Sample delay is set to 7(+1) sample clock cycles. */ \
    0x00U,                    /* Measure delay is set to 0 excitation clock cycles.*/ \
    0x00U,                    /* ACMP threshold has been set to 0. */ \
    lesenseSampleModeCounter, /* ACMP output will be used in comparison. */ \
    lesenseSetIntNone,        /* No interrupt is generated by the channel. */ \
    0x00U,                    /* Counter threshold has been set to 0x01. */ \
    lesenseCompModeLess       /* Compare mode has been set to trigger interrupt on "less". */ \
  }

#define LESENSE_CAPSENSE_CH_CONF_SLEEP \
  { \
    true,                     /* Enable scan channel. */ \
    true,                     /* Enable the assigned pin on scan channel. */ \
    true,                     /* Enable interrupts on channel. */ \
    lesenseChPinExDis,        /* GPIO pin is disabled during the excitation period. */ \
    lesenseChPinIdleDis,      /* GPIO pin is disabled during the idle period. */ \
    false,                    /* Don't use alternate excitation pins for excitation. */ \
    false,                    /* Disabled to shift results from this channel to the decoder register. */ \
    false,                    /* Disabled to invert the scan result bit. */ \
    true,                     /* Enabled to store counter value in the result buffer. */ \
    lesenseClkLF,             /* Use the LF clock for excitation timing. */ \
    lesenseClkLF,             /* Use the LF clock for sample timing. */ \
    0x00U,                    /* Excitation time is set to 0 excitation clock cycles. */ \
    0x01U,                    /* Sample delay is set to 1(+1) sample clock cycles. */ \
    0x00U,                    /* Measure delay is set to 0 excitation clock cycles.*/ \
    LESENSE_ACMP_VDD_SCALE,   /* ACMP threshold has been set to LESENSE_ACMP_VDD_SCALE. */ \
    lesenseSampleModeACMP, /* Counter will be used in comparison. */ \
    lesenseSetIntLevel,       /* Interrupt is generated if the sensor triggers. */ \
    0x0EU,                   /* Counter threshold has been set to 0x0E. */ \
    lesenseCompModeGreaterOrEq       /* Compare mode has been set to trigger interrupt on "less". */ \
  }

void LESENSE_First();
void LESENSE_Setup();
void LESENSE_Calibrate();

#endif
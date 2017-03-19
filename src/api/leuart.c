#include "leuart.h"
#include "em_leuart.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"

void LEUART0_setup() {

    CMU_ClockEnable(cmuClock_LFB, true);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_LEUART0, true);
    CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1);

    GPIO_PinModeSet(LEUART_TX_PORT, LEUART_TX_PORT_NUM, gpioModePushPull, 1);
    GPIO_PinModeSet(LEUART_RX_PORT, LEUART_RX_PORT_NUM, gpioModeInput, 0);

    // 8N1 by default, DATABITS=0, PARITY=0, STOPBITS=0, LEUART0->CTRL
    const LEUART_Init_TypeDef leuartInit = {
        .enable = leuartEnable,
        .refFreq = 0,
        .baudrate = 9600,
        .databits = LEUART_CTRL_DATABITS_EIGHT,
        .parity = LEUART_CTRL_PARITY_NONE,
        .stopbits = LEUART_CTRL_STOPBITS_ONE
    };

    // Setup LEUART0 interrupts
    CORE_CriticalDisableIrq();

    // Interrupt on special start frame
    LEUART0->IFC   = LEUART_IFC_STARTF;
    LEUART0->IEN   |= LEUART_IEN_STARTF;
    NVIC_EnableIRQ(LEUART0_IRQn);
    CORE_CriticalEnableIrq();

    LEUART0->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | 0;
    LEUART_Init(LEUART0, &leuartInit);
}

void leuart0_setup_for_start(uint8_t key) {

// Block RX until special start Frame
LEUART0->STARTFRAME = key;
LEUART0->CTRL |= LEUART_CTRL_SFUBRX;
}

void leuart0_wait_for_start() {
while (LEUART0->STATUS & LEUART_STATUS_RXBLOCK);

}

void leuart0_txbyte(uint8_t b) {
    // Wait for transmitter idle
    // TODO: Add code for buffer also
    while ((LEUART0->IF & LEUART_IF_TXBL) == 0);  

    // Writing clears interrupt
    LEUART0->TXDATA=b;
}

void leuart0_tx_string(char* s) {
uint8_t i;
    for (i=0;i<LEUART_SEND_STRING_MAX;i++) {
        if (s) {
            while(*s) { 
                leuart0_txbyte((uint8_t)*s);
                s++;
            }
        }
    }
}

uint8_t leuart0_getbyte() {

    while((LEUART0->IF & LEUART_IF_RXDATAV) == 0);

// Reading clears interrupt
return (LEUART0->RXDATA);
}

void LEUART0_IRQHandler() {
int intFlags;

CORE_CriticalDisableIrq();
    intFlags = LEUART_IntGet(LEUART0);
    LEUART_IntClear(LEUART0, LEUART_IFS_STARTF);

    if (intFlags & LEUART_IFS_STARTF) {
        if (LEUART0->CTRL & LEUART_CTRL_SFUBRX) {
            // Start key received
        }
    }

CORE_CriticalEnableIrq();
}

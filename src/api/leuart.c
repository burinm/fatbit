#include "leuart.h"
#include "em_leuart.h"
#include "em_cmu.h"
#include "em_core.h"

void LEUART0_setup() {

    //CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_LFB, true);
    CMU_ClockEnable(cmuClock_LEUART0, true);
    CMU->LFBPRESC0 |= CMU_LFBPRESC0_LEUART0_DIV1; 

    // 8N1 by default, DATABITS=0, PARITY=0, STOPBITS=0, 
    //LEUART0->CTRL

    // 9600 Baud with 32768kHz clock
    LEUART0->CLKDIV = LEUART_9600_BAUD_DIV;

    // Clear Buffers
    LEUART0->CMD |= LEUART_CMD_CLEARTX;
    LEUART0->CMD |= LEUART_CMD_CLEARRX;

    // Setup LEUART0 interrupts
    CORE_CriticalDisableIrq();

    // Interrupt on special start frame
    LEUART0->IFC   = LEUART_IFC_STARTF;
    LEUART0->IEN   |= LEUART_IEN_STARTF;
    NVIC_EnableIRQ(LEUART0_IRQn);
    CORE_CriticalEnableIrq();

    //Can I do both at once?
     //   LEUART0->CMD |= (LEUART_CMD_TXEN | LEUART_CMD_RXEN);
    // Enable TX/RX
    LEUART_Enable(LEUART0, leuartEnable);

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
    while ((LEUART0->IF & LEUART_IF_TXC) == 0);  

    // Writing clears interrupt
    LEUART0->TXDATA=b;
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

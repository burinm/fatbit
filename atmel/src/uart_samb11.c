#include "uart_samb11.h"
#include <asf.h>

// http://asf.atmel.com/docs/latest/samb11/html/asfdoc_samb_dma_basic_use_case.html
#define SOURCE_BUFFER_LEN (8)
// Why is this uint8_t, not uint32_t?
static uint8_t source_memory[SOURCE_BUFFER_LEN];

static struct dma_descriptor example_descriptor_tx;
static struct dma_descriptor example_descriptor_rx;


// Modified from 
//  http://asf.atmel.com/docs/latest/samb11/html/asfdoc_samb_uart_dma_use_case.html

//external
struct uart_module my_uart_instance;
struct dma_resource uart_dma_resource_tx;
struct dma_resource uart_dma_resource_rx;

static void transfer_done_tx(struct dma_resource* const resource );
static void transfer_done_rx(struct dma_resource* const resource );
static void configure_dma_resource_tx(struct dma_resource *resource);
static void setup_transfer_descriptor_tx(struct dma_descriptor *descriptor);
static void configure_dma_resource_rx(struct dma_resource *resource);
static void setup_transfer_descriptor_rx(struct dma_descriptor *descriptor);
static void configure_usart(void);
static void configure_dma_callback(void);

static void transfer_done_tx(struct dma_resource* const resource )
{
    dma_start_transfer_job(&uart_dma_resource_rx);
}

static void transfer_done_rx(struct dma_resource* const resource )
{
    dma_start_transfer_job(&uart_dma_resource_tx);
}

static void configure_dma_resource_tx(struct dma_resource *resource)
{
    struct dma_resource_config config;
    dma_get_config_defaults(&config);
    //config.des.periph = UART0TX_DMA_PERIPHERAL;
    config.des.periph = UART1TX_DMA_PERIPHERAL;
    config.des.enable_inc_addr = false;
    //config.src.periph = UART0TX_DMA_PERIPHERAL;
    config.src.periph = UART1TX_DMA_PERIPHERAL;
    dma_allocate(resource, &config);
}

static void setup_transfer_descriptor_tx(struct dma_descriptor *descriptor)
{
    dma_descriptor_get_config_defaults(descriptor);
    descriptor->buffer_size = SOURCE_BUFFER_LEN;
    descriptor->read_start_addr = (uint32_t)source_memory;
    descriptor->write_start_addr = 
            (uint32_t)(&my_uart_instance.hw->TRANSMIT_DATA.reg);
}

static void configure_dma_resource_rx(struct dma_resource *resource)
{
    struct dma_resource_config config;
    dma_get_config_defaults(&config);
    //config.src.periph = UART0RX_DMA_PERIPHERAL;
    config.src.periph = UART1RX_DMA_PERIPHERAL;
    config.src.enable_inc_addr = false;
    config.src.periph_delay = 1;
    dma_allocate(resource, &config);
}

static void setup_transfer_descriptor_rx(struct dma_descriptor *descriptor)
{
    dma_descriptor_get_config_defaults(descriptor);
    descriptor->buffer_size = SOURCE_BUFFER_LEN;
    descriptor->read_start_addr =
            (uint32_t)(&my_uart_instance.hw->RECEIVE_DATA.reg);
    descriptor->write_start_addr = (uint32_t)source_memory;
}

static void configure_usart(void)
{
    struct uart_config config_uart;
    uart_get_config_defaults(&config_uart);
    config_uart.baud_rate = 115200;
    config_uart.pin_number_pad[0] = EDBG_CDC_PIN_PAD0;
    config_uart.pin_number_pad[1] = EDBG_CDC_PIN_PAD1;
    config_uart.pin_number_pad[2] = EDBG_CDC_PIN_PAD2;
    config_uart.pin_number_pad[3] = EDBG_CDC_PIN_PAD3;
    config_uart.pinmux_sel_pad[0] = EDBG_CDC_MUX_PAD0;
    config_uart.pinmux_sel_pad[1] = EDBG_CDC_MUX_PAD1;
    config_uart.pinmux_sel_pad[2] = EDBG_CDC_MUX_PAD2;
    config_uart.pinmux_sel_pad[3] = EDBG_CDC_MUX_PAD3;
    while (uart_init(&my_uart_instance,
            EDBG_CDC_MODULE, &config_uart) != STATUS_OK) {
    }
    uart_enable_transmit_dma(&my_uart_instance);
    uart_enable_receive_dma(&my_uart_instance);
}

static void configure_uart1(void)
{
    struct uart_config config_uart;
   // uart_get_config_defaults(&config_uart);

    //config_uart.baud_rate = 115200;
    config_uart.baud_rate = 9600;
    config_uart.data_bits = UART_8_BITS;
    config_uart.stop_bits = UART_1_STOP_BIT;
    config_uart.parity = UART_NO_PARITY;
    config_uart.flow_control = false;
         
/*
    config_uart.pin_number_pad[0] = PIN_LP_GPIO_6_MUX2_UART1_RXD;
    config_uart.pin_number_pad[1] = PIN_LP_GPIO_7_MUX2_UART1_TXD;
    config_uart.pin_number_pad[2] = PIN_LP_GPIO_14_MUX2_UART1_CTS;
    config_uart.pin_number_pad[3] = PIN_LP_GPIO_15_MUX2_UART1_RTS;

    config_uart.pinmux_sel_pad[0] = MUX_LP_GPIO_6_MUX2_UART1_RXD;
    config_uart.pinmux_sel_pad[1] = MUX_LP_GPIO_7_MUX2_UART1_TXD;
    config_uart.pinmux_sel_pad[2] = MUX_LP_GPIO_14_MUX2_UART1_CTS;
    config_uart.pinmux_sel_pad[3] = MUX_LP_GPIO_15_MUX2_UART1_RTS;
*/
    config_uart.pin_number_pad[0] = EXT3_UART_PIN_PAD0;
    config_uart.pin_number_pad[1] = EXT3_UART_PIN_PAD1;
//    config_uart.pin_number_pad[2] = PIN_LP_GPIO_14_MUX2_UART1_CTS;
//    config_uart.pin_number_pad[3] = PIN_LP_GPIO_15_MUX2_UART1_RTS;

    config_uart.pinmux_sel_pad[0] = EXT3_UART_MUX_PAD0;
    config_uart.pinmux_sel_pad[1] = EXT3_UART_MUX_PAD1;
//    config_uart.pinmux_sel_pad[2] = MUX_LP_GPIO_14_MUX2_UART1_CTS;
//    config_uart.pinmux_sel_pad[3] = MUX_LP_GPIO_15_MUX2_UART1_RTS;

    while (uart_init(&my_uart_instance,
            EXT3_UART_MODULE, &config_uart) != STATUS_OK) {
    }
    uart_enable_transmit_dma(&my_uart_instance);
    uart_enable_receive_dma(&my_uart_instance);
}

static void configure_uart0(void)
{
    struct uart_config config_uart;

    config_uart.baud_rate = 9600;
    config_uart.data_bits = UART_8_BITS;
    config_uart.stop_bits = UART_1_STOP_BIT;
    config_uart.parity = UART_NO_PARITY;
    config_uart.flow_control = false;
         
    config_uart.pin_number_pad[0] = EXT1_UART_PIN_PAD0;
    config_uart.pin_number_pad[1] = EXT1_UART_PIN_PAD1;
    config_uart.pin_number_pad[2] = PIN_LP_GPIO_4;
    config_uart.pin_number_pad[3] = PIN_LP_GPIO_5;

    config_uart.pinmux_sel_pad[0] = EXT1_UART_MUX_PAD0;
    config_uart.pinmux_sel_pad[1] = EXT1_UART_MUX_PAD1;
    config_uart.pinmux_sel_pad[2] = MUX_LP_GPIO_4_UART0_CTS;
    config_uart.pinmux_sel_pad[3] = MUX_LP_GPIO_5_UART0_RTS;

    while (uart_init(&my_uart_instance,
            EXT1_UART_MODULE, &config_uart) != STATUS_OK) {
    }
    uart_enable_transmit_dma(&my_uart_instance);
    uart_enable_receive_dma(&my_uart_instance);
}

static void configure_dma_callback(void)
{
    dma_register_callback(&uart_dma_resource_tx, transfer_done_tx, DMA_CALLBACK_TRANSFER_DONE);
    dma_register_callback(&uart_dma_resource_rx, transfer_done_rx, DMA_CALLBACK_TRANSFER_DONE);
    dma_enable_callback(&uart_dma_resource_tx, DMA_CALLBACK_TRANSFER_DONE);
    dma_enable_callback(&uart_dma_resource_rx, DMA_CALLBACK_TRANSFER_DONE);
    NVIC_EnableIRQ(PROV_DMA_CTRL0_IRQn);
}

void uart_setup() {

    configure_uart1();
/*
    configure_dma_resource_rx(&uart_dma_resource_rx);
    configure_dma_resource_tx(&uart_dma_resource_tx);

    setup_transfer_descriptor_tx(&example_descriptor_tx);
    setup_transfer_descriptor_rx(&example_descriptor_rx);

    configure_dma_callback();
*/
}

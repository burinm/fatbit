#include "uart_samb11.h"
#include <asf.h>

/* 3/10/2017 burin -  Modified from:
    Atmel online examples
    http://asf.atmel.com/docs/latest/samb11/html/asfdoc_samb_uart_dma_use_case.html
    http://asf.atmel.com/docs/latest/samb11/html/asfdoc_samb_dma_basic_use_case.html

*/

#define SOURCE_BUFFER_LEN (8)
static uint8_t source_memory[SOURCE_BUFFER_LEN];

static struct dma_descriptor example_descriptor_tx;
static struct dma_descriptor example_descriptor_rx;



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
static void configure_dma_callback(void);

static void transfer_done_tx(struct dma_resource* const resource )
{
    dma_start_transfer_job(&uart_dma_resource_rx);
}

static void transfer_done_rx(struct dma_resource* const resource )
{
 LED_Off(LED0);
    dma_start_transfer_job(&uart_dma_resource_tx);
}

static void configure_dma_resource_tx(struct dma_resource *resource)
{

    struct dma_resource_config config;
    dma_get_config_defaults(&config);
    config.des.periph = UART1TX_DMA_PERIPHERAL;
    config.des.enable_inc_addr = false;
    config.src.enable_inc_addr = true;

//    config.src.max_burst=1;
//    config.des.max_burst=1;
    dma_allocate(resource, &config);
}

static void setup_transfer_descriptor_tx(struct dma_descriptor *descriptor)
{
    dma_descriptor_get_config_defaults(descriptor);
    descriptor->buffer_size = SOURCE_BUFFER_LEN;
    descriptor->read_start_addr = (uint32_t)&source_memory[0];
    descriptor->write_start_addr = 
            (uint32_t)(&my_uart_instance.hw->TRANSMIT_DATA.reg);
}

static void configure_dma_resource_rx(struct dma_resource *resource)
{
    struct dma_resource_config config;
    dma_get_config_defaults(&config);
    config.src.periph = UART1RX_DMA_PERIPHERAL;
    config.src.enable_inc_addr = false;
    config.src.periph_delay = 1;

    config.des.enable_inc_addr = true;

//    config.src.max_burst=1;
//    config.des.max_burst=1;
    dma_allocate(resource, &config);
}

static void setup_transfer_descriptor_rx(struct dma_descriptor *descriptor)
{
    dma_descriptor_get_config_defaults(descriptor);
    descriptor->buffer_size = SOURCE_BUFFER_LEN;
    descriptor->read_start_addr =
            (uint32_t)(&my_uart_instance.hw->RECEIVE_DATA.reg);
    descriptor->write_start_addr = (uint32_t)&source_memory[0];
}


static void configure_dma_callback(void)
{
    dma_register_callback(&uart_dma_resource_tx, transfer_done_tx, DMA_CALLBACK_TRANSFER_DONE);
    dma_register_callback(&uart_dma_resource_rx, transfer_done_rx, DMA_CALLBACK_TRANSFER_DONE);
    dma_enable_callback(&uart_dma_resource_tx, DMA_CALLBACK_TRANSFER_DONE);
    dma_enable_callback(&uart_dma_resource_rx, DMA_CALLBACK_TRANSFER_DONE);
    NVIC_EnableIRQ(PROV_DMA_CTRL0_IRQn);
}

static void configure_uart1(void)
{
    struct uart_config config_uart;

    config_uart.baud_rate = 9600;
    config_uart.data_bits = UART_8_BITS;
    config_uart.stop_bits = UART_1_STOP_BIT;
    config_uart.parity = UART_NO_PARITY;
    config_uart.flow_control = false;
         
    config_uart.pin_number_pad[0] = PIN_LP_GPIO_6;
    config_uart.pin_number_pad[1] = PIN_LP_GPIO_7;
    //config_uart.pin_number_pad[2] = PIN_LP_GPIO_14_MUX2_UART1_CTS;
    //config_uart.pin_number_pad[3] = PIN_LP_GPIO_15_MUX2_UART1_RTS;

    config_uart.pinmux_sel_pad[0] = MUX_LP_GPIO_6_UART1_RXD;
    config_uart.pinmux_sel_pad[1] = MUX_LP_GPIO_7_UART1_TXD;
    //config_uart.pinmux_sel_pad[2] = MUX_LP_GPIO_14_MUX2_UART1_CTS;
    //config_uart.pinmux_sel_pad[3] = MUX_LP_GPIO_15_MUX2_UART1_RTS;

    while (uart_init(&my_uart_instance,
            EXT3_UART_MODULE, &config_uart) != STATUS_OK) {
    }
    uart_enable_transmit_dma(&my_uart_instance);
    uart_enable_receive_dma(&my_uart_instance);
}

void uart_setup() {

    configure_uart1();

    configure_dma_resource_tx(&uart_dma_resource_tx);
    setup_transfer_descriptor_tx(&example_descriptor_tx);
    dma_add_descriptor(&uart_dma_resource_tx,&example_descriptor_tx);

    configure_dma_resource_rx(&uart_dma_resource_rx);
    setup_transfer_descriptor_rx(&example_descriptor_rx);
    dma_add_descriptor(&uart_dma_resource_rx,&example_descriptor_rx);

    configure_dma_callback();
}

#include "snf_transport_app_uart.h"
#include "app_uart.h"
#include "boards.h"
#include "nrf.h"

#define UART_CMD_SHOW_CMD_LIST  'h'
#define UART_CMD_SHOW_RF_CONFIG 'd'
#define UART_CMD_START_RX       'r'
#define UART_CMD_STOP_RX        's'

static bool m_rx_started = false;

static void display_command_list(void);
static void parse_input_byte(uint8_t byte);
static void start_rx(void);
static void stop_rx(void);

static snf_trans_callback_t m_snf_trans_event_handler = 0;
static snf_trans_event_t m_callback_event;

static void uart_event_handler(app_uart_evt_t * p_app_uart_event)
{
    uint8_t new_byte;
    switch(p_app_uart_event->evt_type)
    {
        case APP_UART_DATA_READY:
            while(app_uart_get(&new_byte) == NRF_SUCCESS)
            {
                parse_input_byte(new_byte);
            }
            break;

        case APP_UART_FIFO_ERROR:
            break;

        case APP_UART_COMMUNICATION_ERROR:
            break;

        case APP_UART_TX_EMPTY:
            break;

        case APP_UART_DATA:  
            break;
    }
}

uint32_t snf_trans_app_uart_init(snf_trans_baseconfig_t *config)
{
    uint32_t err_code;
    
    m_snf_trans_event_handler = config->event_handler;

    app_uart_comm_params_t uart_params = {.rx_pin_no = RX_PIN_NUMBER,
                                          .tx_pin_no = TX_PIN_NUMBER,
                                          .rts_pin_no = RTS_PIN_NUMBER,
                                          .cts_pin_no = CTS_PIN_NUMBER,
                                          .flow_control = HWFC,
                                          .use_parity = 0,
                                          .baud_rate = UART_BAUDRATE_BAUDRATE_Baud115200};
    APP_UART_FIFO_INIT(&uart_params, 256, 256, uart_event_handler, 6, err_code);

    return err_code;
}

uint32_t snf_trans_sniffer_ready(void)
{
    printf("ESB Sniffer initialized. Press '%c' for command list.\r\n", UART_CMD_SHOW_CMD_LIST);
}

uint32_t snf_trans_on_rx_packet_received(nrf_esb_payload_t *packet)
{
    printf("RX L:%i\tP:%i\tPID:%i\t", packet->length, packet->pipe, packet->pid);
    for(int i = 0; i < packet->length; i++)
    {
        printf("0x%.2X ", packet->data[i]);
    }
    printf("\r\n");
}

// Internal functions

static void display_command_list(void)
{
    printf("Sniffer command list:\r\n");
    printf("  '%c' - Display RF configuration\r\n", UART_CMD_SHOW_RF_CONFIG);
    printf("  '%c' - Start RX\r\n", UART_CMD_START_RX);
    printf("  '%c' - Stop RX\r\n", UART_CMD_STOP_RX);
    printf("\r\n");
}

static void parse_input_byte(uint8_t byte)
{
    switch(byte)
    {
        case UART_CMD_SHOW_CMD_LIST:
            display_command_list();
            break;

        case UART_CMD_SHOW_RF_CONFIG:
            break;

        case UART_CMD_START_RX:
            if(!m_rx_started)
            {
                start_rx();
            }
            else 
            {
                printf("Error: RX already started.\r\n");
            }
            break;

        case UART_CMD_STOP_RX:
            if(m_rx_started)
            {
                stop_rx();
            }
            else 
            {
                printf("Error: RX not started.\r\n");
            }
            break;
    }
}

static void start_rx(void)
{
    m_rx_started = true;
    printf("Starting RX\r\n");
    SNF_TRANS_CALLBACK_RUN(SNF_TRANS_EVT_START_RX, m_callback_event, m_snf_trans_event_handler);
}

static void stop_rx(void)
{
    m_rx_started = false;
    printf("Stopping RX\r\n");
    SNF_TRANS_CALLBACK_RUN(SNF_TRANS_EVT_STOP_RX, m_callback_event, m_snf_trans_event_handler);
}
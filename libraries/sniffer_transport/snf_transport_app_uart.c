#include "snf_transport_app_uart.h"
#include "app_uart.h"
#include "boards.h"
#include "nrf.h"

#define UART_CMD_SHOW_CMD_LIST      'h'
#define UART_CMD_SHOW_RF_CONFIG     'd'
#define UART_CMD_CHANGE_RF_CONFIG   'm'
#define UART_CMD_START_RX           'r'
#define UART_CMD_STOP_RX            's'

#define MENU_ITEMS_MAX              128

console_menu_item_t console_menu_items[] = {{UART_CMD_SHOW_CMD_LIST,  "Show this menu"},
                                            {UART_CMD_SHOW_RF_CONFIG, "Display RF configuration"},
                                            {UART_CMD_CHANGE_RF_CONFIG, "Change RF configuration"},
                                            {UART_CMD_START_RX, "Start RX"},
                                            {UART_CMD_STOP_RX, "Stop RX"},
                                            {0, 0}};

enum {CFG_PRM_ID_NULL, CFG_PRM_ID_RF_CH, CFG_PRM_ID_RF_BITRATE, CFG_PRM_ID_MAX_PL_LENGTH, CFG_PRM_ID_CRC, CFG_PRM_ID_ESB_MODE, CFG_PRM_ID_RF_ADDRESS};
static uint8_t m_rf_address_buffer[5];
snf_cfg_parameter_t sniffer_cfg_params[] = {{CFG_PRM_ID_RF_CH,          "RF channel",   CFG_PRM_INT, 0, 125, 2, 0, 0},
                                            {CFG_PRM_ID_RF_BITRATE,     "RF bitrate",   CFG_PRM_ENUM, 0, 1, 1, "1 Mbps,2 Mbps", 0},
                                            {CFG_PRM_ID_MAX_PL_LENGTH,  "Max payload length", CFG_PRM_INT, 1, 252, 32, 0, 0},
                                            {CFG_PRM_ID_CRC,            "CRC mode",     CFG_PRM_ENUM, 0, 2, 2, "Off,8-bit,16-bit", 0},
                                            {CFG_PRM_ID_ESB_MODE,       "ESB mode",     CFG_PRM_ENUM, 0, 1, 1, "ESB,ESB DPL", 0},
                                            {CFG_PRM_ID_RF_ADDRESS,     "RF address",   CFG_PRM_BYTEBUF, 2, 5, 5, 0, m_rf_address_buffer},
                                            {CFG_PRM_ID_NULL,           0,              0,               0, 0, 0, 0, 0}};
static uint32_t m_cfg_prm_item_count;

static bool m_rx_started = false;

static void display_command_list(void);
static void default_input_parser(uint8_t byte);
static void start_rx(void);
static void stop_rx(void);

static snf_trans_callback_t m_snf_trans_event_handler = 0;
static snf_trans_event_t m_callback_event;

static void (*m_uart_input_parser)(uint8_t) = 0;

static void uart_event_handler(app_uart_evt_t * p_app_uart_event)
{
    uint8_t new_byte;
    switch(p_app_uart_event->evt_type)
    {
        case APP_UART_DATA_READY:
            while(app_uart_get(&new_byte) == NRF_SUCCESS)
            {
                m_uart_input_parser(new_byte);
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

    m_uart_input_parser = default_input_parser;
    
    // Find out how many config parameters there are and store it in m_cfg_prm_item_count
    for(m_cfg_prm_item_count = 0; 
        m_cfg_prm_item_count < MENU_ITEMS_MAX && sniffer_cfg_params[m_cfg_prm_item_count].index != CFG_PRM_ID_NULL; 
        m_cfg_prm_item_count++);
    
    app_uart_comm_params_t uart_params = {.rx_pin_no = RX_PIN_NUMBER,
                                          .tx_pin_no = TX_PIN_NUMBER,
                                          .rts_pin_no = RTS_PIN_NUMBER,
                                          .cts_pin_no = CTS_PIN_NUMBER,
                                          .flow_control = HWFC,
                                          .use_parity = 0,
                                          .baud_rate = UART_BAUDRATE_BAUDRATE_Baud460800};
    APP_UART_FIFO_INIT(&uart_params, 256, 256, uart_event_handler, 6, err_code);

    return err_code;
}

uint32_t snf_trans_sniffer_ready(void)
{
    printf("------------- ESB Sniffer initialized-------------\r\n", UART_CMD_SHOW_CMD_LIST);
    display_command_list();
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

static char *find_item_no(uint32_t index, char *buf)
{
    while(index--)
    {
        while(*buf && *buf != ',') buf++;
        buf++;
    }
    return buf;
}

static void display_command_list(void)
{
    printf("Sniffer command list:\r\n");
    for(int i = 0; i < MENU_ITEMS_MAX; i++)
    {
        if(console_menu_items[i].cmd_byte == 0) break;
        printf("  '%c' - %s\r\n", console_menu_items[i].cmd_byte, console_menu_items[i].desc);
    }
    printf("\r\n");
}

static void display_configuration_parameters(int32_t selected_item)
{
    printf("Sniffer configuration:\r\n");
    for(int i = 0; i < MENU_ITEMS_MAX; i++)
    {
        if(sniffer_cfg_params[i].index == CFG_PRM_ID_NULL) break;
        printf((i == selected_item) ? "-> " : "   ");
        printf("%s:\t", sniffer_cfg_params[i].desc);
        switch(sniffer_cfg_params[i].type)
        {
            case CFG_PRM_INT:
                printf("%i", sniffer_cfg_params[i].value);
                break;

            case CFG_PRM_ENUM:
                printf("%s", find_item_no(sniffer_cfg_params[i].value, sniffer_cfg_params[i].enum_descriptions));
                break;

            case CFG_PRM_BYTEBUF:
                if(sniffer_cfg_params[i].byte_buffer != 0)
                {
                    for(int b = 0; b < sniffer_cfg_params[i].value; b++)
                    {
                        printf((b < (sniffer_cfg_params[i].value-1) ? "%.2X-" : "%.2X"), sniffer_cfg_params[i].byte_buffer[b]);
                    }                    
                }
                break;
        
            default:
                printf("Invalid configuration type (%.8X)!", sniffer_cfg_params[i].type);
                break;
        }
        printf("\r\n");
    }
}

static void change_config_input_parser(uint8_t byte)
{
    static uint32_t selected_config_item = 0;
    static uint32_t change_config_input_state = 0;
    switch(change_config_input_state)
    {
        case 0:
            if(byte == 0x1B)
            {
                change_config_input_state = 1;
            }
            break;
        case 1:
            change_config_input_state = (byte == 0x5B) ? 2 : 0;
            break;
        case 2:
            switch(byte)
            {
                case 0x41:
                    // Up button pressed
                    if(selected_config_item > 0)
                    {
                        selected_config_item--;
                        display_configuration_parameters(selected_config_item);
                    }
                    break;
                case 0x42:
                    // Down button pressed
                    if(selected_config_item < (m_cfg_prm_item_count - 1))
                    {
                        selected_config_item++;
                        display_configuration_parameters(selected_config_item);
                    }
                    break;
                case 0x43:
                    printf("Right\n");
                    break;
                case 0x44:
                    printf("Left\n");
                    break;

            }
            change_config_input_state = 0;
            break;
        
            
    }
}

// Input parsers

static void default_input_parser(uint8_t byte)
{
    switch(byte)
    {
        case UART_CMD_SHOW_CMD_LIST:
            display_command_list();
            break;

        case UART_CMD_SHOW_RF_CONFIG:
            display_configuration_parameters(-1);
            break;

        case UART_CMD_CHANGE_RF_CONFIG:
            display_configuration_parameters(0);
            m_uart_input_parser = change_config_input_parser;
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

        default: 
            printf("Unknown character: %.2X\r\n", byte);
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
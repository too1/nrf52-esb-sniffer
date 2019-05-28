#include "snf_transport_app_uart.h"
#include "app_uart.h"
#include "boards.h"
#include "nrf.h"

static void uart_event_handler(app_uart_evt_t * p_app_uart_event)
{
    switch(p_app_uart_event->evt_type)
    {
        case APP_UART_DATA_READY:
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
    app_uart_comm_params_t uart_params = {.rx_pin_no = RX_PIN_NUMBER,
                                          .tx_pin_no = TX_PIN_NUMBER,
                                          .rts_pin_no = RTS_PIN_NUMBER,
                                          .cts_pin_no = CTS_PIN_NUMBER,
                                          .flow_control = HWFC,
                                          .use_parity = 0,
                                          .baud_rate = UART_BAUDRATE_BAUDRATE_Baud115200};
    APP_UART_FIFO_INIT(&uart_params, 128, 128, uart_event_handler, 6, err_code);
    if(err_code == NRF_SUCCESS)
    {
        printf("UART Transport initialized successfully.\r\n");
    }
    return err_code;
}

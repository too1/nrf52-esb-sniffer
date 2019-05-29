#ifndef __SNF_TRANSPORT_APP_UART_H
#define __SNF_TRANSPORT_APP_UART_H

#include "snf_transport.h"
#include "nrf_esb.h"

typedef struct
{
    char cmd_byte;
    char *desc;
}console_menu_item_t;

typedef enum {CFG_PRM_INT, CFG_PRM_BYTEBUF, CFG_PRM_ENUM} snf_config_param_type_t;

typedef struct
{
    uint32_t                index;
    int8_t                  *desc;
    snf_config_param_type_t type;
    int32_t                 min_val;
    int32_t                 max_val;
    int32_t                 value;
    int8_t                  *enum_descriptions;
    uint8_t                 *byte_buffer;
} snf_cfg_parameter_t;

uint32_t snf_trans_app_uart_init(snf_trans_baseconfig_t *config);

uint32_t snf_trans_sniffer_ready(void);

uint32_t snf_trans_on_rx_packet_received(nrf_esb_payload_t *packet);

#endif

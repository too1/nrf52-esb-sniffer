#ifndef __SNF_TRANSPORT_APP_UART_H
#define __SNF_TRANSPORT_APP_UART_H

#include "snf_transport.h"
#include "nrf_esb.h"

uint32_t snf_trans_app_uart_init(snf_trans_baseconfig_t *config);

uint32_t snf_trans_sniffer_ready(void);

uint32_t snf_trans_on_rx_packet_received(nrf_esb_payload_t *packet);

#endif

#ifndef __APP_SNIFFER_H
#define __APP_SNIFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "app_sniffer_types.h"
#include "nrf_esb.h"
#include "snf_transport.h"

#define SNF_RX_TIMESTAMP_TIMER              NRF_TIMER1
#define SNF_RX_TIMESTAMP_TIMER_IRQn         TIMER1_IRQn
#define SNF_RX_TIMESTAMP_TIMER_IRQHandler   TIMER1_IRQHandler
#define SNF_RX_TIMESTAMP_RELOAD_INTERVAL    (1 << 31)
#define SNF_PPI_RX_TIMESTAMP_CH             14

typedef enum {APP_SNIFFER_EVT_TYPE_RX_PACKET_RECEIVED} app_sniffer_evt_type_t;

typedef struct
{
    app_sniffer_evt_type_t  type;
    uint64_t                time_stamp;
    nrf_esb_payload_t       *rf_payload;
} app_sniffer_event_t;

typedef void (*app_sniffer_callback_t)(app_sniffer_event_t *app_sniffer_event);

typedef struct
{
    app_sniffer_callback_t event_handler;
} app_sniffer_config_t;

uint32_t app_sniffer_init(app_sniffer_config_t *config);

uint32_t app_sniffer_configure(snf_trans_sniffer_configuration_t *config);

uint32_t app_sniffer_start_rx(void);

uint32_t app_sniffer_stop_rx(void);

#endif

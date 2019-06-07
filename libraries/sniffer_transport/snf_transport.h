#ifndef __SNF_TRANSPORT_H
#define __SNF_TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>
#include "nrf_esb.h"

#define SNF_TRANS_CALLBACK_RUN(TYPE, EVENT, CALLBACK) if(CALLBACK != 0) \
                                                      {                 \
                                                         EVENT.type = TYPE; \
                                                         CALLBACK(&EVENT); \
                                                      }

typedef enum {SNF_TRANS_EVT_START_RX, SNF_TRANS_EVT_STOP_RX, SNF_TRANS_EVT_SET_CONFIGURATION, SNF_TRANS_EVT_ERROR} snf_trans_event_type_t;

typedef struct 
{
    nrf_esb_config_t esb_config;
    uint8_t addr_length;            /**< Length of the address including the prefix. */
    uint8_t address[5];
    uint8_t rf_channel;             /**< Channel to use (must be between 0 and 100). */
} snf_trans_sniffer_configuration_t;

typedef struct 
{
    snf_trans_event_type_t              type;
    snf_trans_sniffer_configuration_t   sniffer_config;
    uint32_t                            error_code;
} snf_trans_event_t;

typedef void (*snf_trans_callback_t)(snf_trans_event_t *event);

typedef struct
{
    snf_trans_callback_t event_handler;
} snf_trans_baseconfig_t;

#endif

#ifndef __SNF_TRANSPORT_H
#define __SNF_TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>

#define SNF_TRANS_CALLBACK_RUN(TYPE, EVENT, CALLBACK) if(CALLBACK != 0) \
                                                      {                 \
                                                         EVENT.type = TYPE; \
                                                         CALLBACK(&EVENT); \
                                                      }

typedef enum {SNF_TRANS_EVT_START_RX, SNF_TRANS_EVT_STOP_RX, SNF_TRANS_EVT_ERROR} snf_trans_event_type_t;

typedef struct 
{
    snf_trans_event_type_t type;
    uint32_t error_code;
} snf_trans_event_t;

typedef void (*snf_trans_callback_t)(snf_trans_event_t *event);

typedef struct
{
    snf_trans_callback_t event_handler;
} snf_trans_baseconfig_t;

#endif

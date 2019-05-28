#ifndef __SNF_TRANSPORT_H
#define __SNF_TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct 
{

} snf_trans_event_t;

typedef void (*snf_trans_callback_t)(snf_trans_event_t *event);

typedef struct
{
    snf_trans_callback_t event_handler;
} snf_trans_baseconfig_t;

#endif

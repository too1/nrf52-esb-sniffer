/**
 * Copyright (c) 2014 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include "sdk_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "boards.h"

#include "snf_transport_app_uart.h"
#include "app_sniffer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

uint8_t led_nr;


void clocks_start( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}


void gpio_init( void )
{
    bsp_board_init(BSP_INIT_LEDS);
}

static void sniffer_transport_callback(snf_trans_event_t *event)
{
    switch(event->type)
    {
        case SNF_TRANS_EVT_SET_CONFIGURATION:
            app_sniffer_configure(&event->sniffer_config);
            break;

        case SNF_TRANS_EVT_START_RX:
            app_sniffer_start_rx();
            break;

        case SNF_TRANS_EVT_STOP_RX:
            app_sniffer_stop_rx();
            break;

        case SNF_TRANS_EVT_ERROR:
            break;
    }
}

static void app_sniffer_callback(app_sniffer_event_t *event)
{
    switch(event->type)
    {
        case APP_SNIFFER_EVT_TYPE_RX_PACKET_RECEIVED:
            snf_trans_on_rx_packet_received(event->rf_payload);
            break;
    }
}

static void sniffer_init(void)
{
    // Configure the sniffer transport layer
    snf_trans_baseconfig_t config = {.event_handler = sniffer_transport_callback};
    APP_ERROR_CHECK(snf_trans_app_uart_init(&config));

    // Configure the app sniffer module
    app_sniffer_config_t app_sniffer_config = {.event_handler = app_sniffer_callback};
    APP_ERROR_CHECK(app_sniffer_init(&app_sniffer_config));
}

int main(void)
{
    uint32_t err_code;

    gpio_init();

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    clocks_start();

    sniffer_init();

    snf_trans_sniffer_ready();
    
    NRF_LOG_INFO("nRF52 ESB Sniffer (UART transport) started");

    while (true)
    {
        __WFE();
    }
}
/*lint -restore */

nrf52-esb-sniffer
#################

A small project to set up an nRF52 device as an ESB sniffer, allowing you to configure the ESB stack and listen for packets in the area. 

Current implementation uses a UART terminal interface, but the architecture is designed to allow different transport/control layers.

Default UART settings:
Baudrate:      460800
Parity:        No
Flow control:  Hardware flow control

Supported hardware:
- nRF52DK
- nRF52840DK

Supported SDK:
- nRF5 SDK v15.3.0
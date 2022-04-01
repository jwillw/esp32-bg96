
#ifndef CELLULAR_SETUP_H
#define CELLULAR_SETUP_H


#include "cellular_config.h"

#include "cellular_api.h"
#include "cellular_types.h"
#include "driver/uart.h"

CellularHandle_t setup_cellular(
	uint8_t comm_uart,
	uint8_t comm_tx,
	uint8_t comm_rx
);


#endif  /* CELLULAR_SETUP_H */

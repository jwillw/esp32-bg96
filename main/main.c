
#include "cellular_setup.h"
#include "driver/gpio.h"

#include <stdio.h>


static const char *const sim_card_state[] = {
	"REMOVED",
	"ISERTED",
	"STATUS MAX",
	"UNKNOWN"
};

void app_main(void){
	CellularHandle_t handle;
	handle = setup_cellular(
		UART_NUM_2,
		GPIO_NUM_17,
		GPIO_NUM_16
	);

	if(handle != NULL){
		CellularSimCardStatus_t sim_status = {0};
		(void) Cellular_GetSimCardStatus(handle, &sim_status);
		LogDebug( ("sim card state: %s", sim_card_state[sim_status.simCardState]) );
	}
}

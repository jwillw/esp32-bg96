
#include "cellular_setup.h"


#include "cellular_logging.h"
#include "cellular_comm_interface.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include <stdio.h>


#define COMM_RX_BUF_SIZE 1024

static uint8_t COMM_UART;
static uint8_t COMM_TX;
static uint8_t COMM_RX;


static CellularCommInterfaceError_t _comm_open(
	CellularCommInterfaceReceiveCallback_t recv_callback,
	void *user_data,
	CellularCommInterfaceHandle_t *handle
);

static CellularCommInterfaceError_t _comm_close(
	CellularCommInterfaceHandle_t handle
);

static CellularCommInterfaceError_t _comm_send(
	CellularCommInterfaceHandle_t handle,
	const uint8_t *data, uint32_t len,
	uint32_t timeout_ms,
	uint32_t *sent_len
);

static CellularCommInterfaceError_t _comm_recv(
	CellularCommInterfaceHandle_t handle,
	uint8_t *buffer, uint32_t len,
	uint32_t timeout_ms,
	uint32_t *recv_len
);


CellularHandle_t setup_cellular(
	uint8_t comm_uart,
	uint8_t comm_tx,
	uint8_t comm_rx
){
	COMM_UART = comm_uart;
	COMM_TX   = comm_tx;
	COMM_RX   = comm_rx;

	const CellularCommInterface_t comm_interface = {
		.open  = _comm_open,
		.close = _comm_close,
		.send  = _comm_send,
		.recv  = _comm_recv
	};
	CellularError_t cell_status = CELLULAR_SUCCESS;
	CellularHandle_t handle = NULL;

	cell_status = Cellular_Init(&handle, &comm_interface);
	if(cell_status != CELLULAR_SUCCESS){
		LogError( ("cellular setup failed %d", cell_status) );
		return NULL;
	}

	return handle;
}


static struct _uart_event_data{
	CellularCommInterfaceHandle_t          comm_handle;
	CellularCommInterfaceReceiveCallback_t recv_callback;
	QueueHandle_t                          event_queue;
	void *                                 user_data;
	TaskHandle_t                           handle;
} _uart_event_data;

static void _uart_event_task(void *param){
	(void) param;
	uart_event_t event;
	for(;;){
		if(
			xQueueReceive(
				_uart_event_data.event_queue, (void *) &event,
				(TickType_t) portMAX_DELAY
			) && event.type == UART_DATA
		){
			_uart_event_data.recv_callback(
				_uart_event_data.user_data,
				_uart_event_data.comm_handle
			);
		}
	}
}


static CellularCommInterfaceError_t _comm_open(
	CellularCommInterfaceReceiveCallback_t recv_callback,
	void *user_data,
	CellularCommInterfaceHandle_t *handle
){
	const uart_config_t uart_config = {
		.baud_rate  = 115200,
		.data_bits  = UART_DATA_8_BITS,
		.parity     = UART_PARITY_DISABLE,
		.stop_bits  = UART_STOP_BITS_1,
		.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB
	};
	uart_driver_install(COMM_UART, COMM_RX_BUF_SIZE, 0, 4, &_uart_event_data.event_queue, 0);
	uart_param_config(COMM_UART, &uart_config);
	uart_set_pin(COMM_UART, COMM_TX, COMM_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	_uart_event_data.recv_callback = recv_callback;
	_uart_event_data.user_data     = user_data;
	xTaskCreate(_uart_event_task, NULL, 2048, NULL, 12, &_uart_event_data.handle);

	*handle = (CellularCommInterfaceHandle_t) !NULL;
	return IOT_COMM_INTERFACE_SUCCESS;
}

static CellularCommInterfaceError_t _comm_close(
	CellularCommInterfaceHandle_t handle
){
	vTaskDelete(_uart_event_data.handle);
	uart_driver_delete(COMM_UART);
	return IOT_COMM_INTERFACE_SUCCESS;
}

static CellularCommInterfaceError_t _comm_send(
	CellularCommInterfaceHandle_t handle,
	const uint8_t *data, uint32_t len,
	uint32_t timeout_ms,
	uint32_t *sent_len
){
	int sent = uart_write_bytes(COMM_UART, data, len);
	if(sent > -1){
		*sent_len = (uint32_t) sent;
		return IOT_COMM_INTERFACE_SUCCESS;
	}else{
		*sent_len = 0;
		return IOT_COMM_INTERFACE_FAILURE;
	}
}

static CellularCommInterfaceError_t _comm_recv(
	CellularCommInterfaceHandle_t handle,
	uint8_t *buffer, uint32_t len,
	uint32_t timeout_ms,
	uint32_t *recv_len
){
	int recv = uart_read_bytes(COMM_UART, buffer, len, timeout_ms / portTICK_PERIOD_MS);
	if(recv > -1){
		*recv_len = (uint32_t) recv;
		return IOT_COMM_INTERFACE_SUCCESS;
	}else{
		*recv_len = 0;
		return IOT_COMM_INTERFACE_FAILURE;
	}
}


#ifndef CELLULAR_PLATFORM_H
#define CELLULAR_PLATFORM_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include <stdint.h>
#include <stdbool.h>

/*-----------------------------------------------------------*/

/**
 * @brief Cellular library platform thread API and configuration.
 *
 * Cellular library create a detached thread by this API.
 * The threadRoutine should be called with pArgument in the created thread.
 *
 * PLATFORM_THREAD_DEFAULT_STACK_SIZE and PLATFORM_THREAD_DEFAULT_PRIORITY defines
 * the platform related stack size and priority.
 */

bool Platform_CreateDetachedThread(
	void ( * threadRoutine )( void * ),
	void * pArgument,
	int32_t priority,
	size_t stackSize
);

#define PLATFORM_THREAD_DEFAULT_STACK_SIZE    ( 2048U )
#define PLATFORM_THREAD_DEFAULT_PRIORITY      ( tskIDLE_PRIORITY + 5U )

/*-----------------------------------------------------------*/

/**
 * @brief Cellular library platform mutex APIs.
 *
 * Cellular library use platform mutex to protect resource.
 *
 * The IotMutex_ functions can be referenced as function prototype for
 * PlatfromMutex_ prefix function in the following link.
 * https://docs.aws.amazon.com/freertos/latest/lib-ref/c-sdk/platform/platform_threads_functions.html
 *
 */

typedef struct PlatformMutex{
	StaticSemaphore_t xMutex;    /**< FreeRTOS mutex. */
	BaseType_t        recursive; /**< Type; used for indicating if this is reentrant or normal. */
} PlatformMutex_t;

bool PlatformMutex_Create( PlatformMutex_t * pNewMutex, bool recursive );
void PlatformMutex_Destroy( PlatformMutex_t * pMutex );
void PlatformMutex_Lock( PlatformMutex_t * pMutex );
bool PlatformMutex_TryLock( PlatformMutex_t * pMutex );
void PlatformMutex_Unlock( PlatformMutex_t * pMutex );

/*-----------------------------------------------------------*/

/**
 * @brief Cellular library platform memory allocation APIs.
 *
 * Cellular library use platform memory allocation APIs to allocate memory dynamically.
 * The FreeRTOS memory management document can be referenced for these APIs.
 * https://www.freertos.org/a00111.html
 *
 */

#define Platform_Malloc    pvPortMalloc
#define Platform_Free      vPortFree

/*-----------------------------------------------------------*/

/**
 * @brief Cellular library platform event group APIs.
 *
 * Cellular library use platform event group for process synchronization.
 *
 * The EventGroup functions in the following link can be referenced as function prototype.
 * https://www.freertos.org/event-groups-API.html
 *
 */

#define PlatformEventGroupHandle_t           EventGroupHandle_t
#define PlatformEventGroup_Delete            vEventGroupDelete
#define PlatformEventGroup_ClearBits         xEventGroupClearBits
#define PlatformEventGroup_Create            xEventGroupCreate
#define PlatformEventGroup_GetBits           xEventGroupGetBits
#define PlatformEventGroup_SetBits           xEventGroupSetBits
#define PlatformEventGroup_SetBitsFromISR    xEventGroupSetBitsFromISR
#define PlatformEventGroup_WaitBits          xEventGroupWaitBits
#define PlatformEventGroup_EventBits         EventBits_t
#define PlatformTickType                     TickType_t

/*-----------------------------------------------------------*/

/**
 * @brief Cellular library platform delay function.
 *
 * Cellular library use platform delay function for waiting events.
 *
 * The delay functions in the following link can be referenced as function prototype.
 * https://www.freertos.org/a00127.html
 *
 */
#define Platform_Delay( delayMs )    vTaskDelay( pdMS_TO_TICKS( delayMs ) )

#endif /* CELLULAR_PLATFORM_H */

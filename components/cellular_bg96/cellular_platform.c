
#include <stdbool.h>

#include "cellular_platform.h"


typedef QueueHandle_t SemaphoreHandle_t;

typedef struct threadInfo{
	void *pArgument;
	void ( * threadRoutine )( void * );
} threadInfo_t;

static void prvThreadRoutineWrapper( void * pArgument ){
	threadInfo_t * pThreadInfo = ( threadInfo_t * ) pArgument;

	pThreadInfo->threadRoutine( pThreadInfo->pArgument );
	Platform_Free( pThreadInfo );

	vTaskDelete( NULL );
}

static bool prIotMutexTimedLock( PlatformMutex_t * pMutex, TickType_t timeout ){
	BaseType_t lockResult = pdTRUE;

	if( pMutex->recursive == pdTRUE )
		lockResult = xSemaphoreTakeRecursive( ( SemaphoreHandle_t ) &pMutex->xMutex, timeout );
	else
		lockResult = xSemaphoreTake( ( SemaphoreHandle_t ) &pMutex->xMutex, timeout );

	return( lockResult == pdTRUE );
}


bool Platform_CreateDetachedThread(
	void ( * threadRoutine )( void * ),
	void * pArgument,
	int32_t priority,
	size_t stackSize
){
	threadInfo_t * pThreadInfo = NULL;
	pThreadInfo = Platform_Malloc( sizeof( threadInfo_t ) );
	if( pThreadInfo == NULL )
		return false;

	pThreadInfo->threadRoutine = threadRoutine;
	pThreadInfo->pArgument = pArgument;

	if(
		xTaskCreate(
			prvThreadRoutineWrapper,
			NULL,
			( configSTACK_DEPTH_TYPE ) stackSize,
			pThreadInfo,
			priority,
			NULL
		) != pdPASS
	){
		Platform_Free( pThreadInfo );
		return false;
	}

	return true;
}

bool PlatformMutex_Create(
	PlatformMutex_t *pNewMutex,
	bool recursive
){
	SemaphoreHandle_t xSemaphore = NULL;
	pNewMutex->recursive = recursive ? pdTRUE : pdFALSE;

	if(recursive)
		xSemaphore = xSemaphoreCreateRecursiveMutexStatic(&pNewMutex->xMutex);
	else
		xSemaphore = xSemaphoreCreateMutexStatic(&pNewMutex->xMutex);

	if(xSemaphore != NULL)
		return true;
	else
		return false;
}

void PlatformMutex_Destroy( PlatformMutex_t * pMutex ){
	vSemaphoreDelete( ( SemaphoreHandle_t ) &pMutex->xMutex );
}

void PlatformMutex_Lock( PlatformMutex_t * pMutex ){
	prIotMutexTimedLock( pMutex, portMAX_DELAY );
}

bool PlatformMutex_TryLock( PlatformMutex_t * pMutex ){
	return prIotMutexTimedLock( pMutex, 0 );
}

void PlatformMutex_Unlock( PlatformMutex_t * pMutex ){
	if( pMutex->recursive == pdTRUE )
		( void ) xSemaphoreGiveRecursive( ( SemaphoreHandle_t ) &pMutex->xMutex );
	else
		( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &pMutex->xMutex );
}

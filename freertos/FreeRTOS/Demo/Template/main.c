#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#define TASK_STACK_SIZE 1024

BaseType_t TaskCreateSafe(
	TaskFunction_t pvTaskCode,
	const char* const pcName,
	unsigned short usStackDepth,
	void* pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t *pxCreatedTask);

void simpleTask(void* data) {
	char* name = (char*)data;
	while (1) {
		printf("%s\n", name);
	}
}

int main() {
	printf("Hello World Example");
	TaskCreateSafe(simpleTask, (const char*) "simple task", TASK_STACK_SIZE, "This is my simple task", 1, NULL);
	vTaskStartScheduler();
	return 0;
}

BaseType_t TaskCreateSafe(
	TaskFunction_t pvTaskCode,
	const char* const pcName,
	unsigned short usStackDepth,
	void* pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t *pxCreatedTask) {
	BaseType_t status = xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
	if (status == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		printf("Error: could not safely create a task > errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY");
		exit(errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY);
	}
	return status;
}

void vAssertCalled(unsigned long ulLine, const char * const pcFileName){
	printf("ASSERT! Line %d, file %s\r\n", ulLine, pcFileName);
}

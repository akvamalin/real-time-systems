#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#define TASK_STACK_SIZE 1024

void simpleTask(void* data) {
	char* name = (char*)data;
	while (1) {
		printf("%s\n", name);
	}
}

int main() {
	printf("Hello World Example");
	xTaskCreate(simpleTask, (const char*) "simple task", TASK_STACK_SIZE, "This is my simple task", 1, NULL);
	vTaskStartScheduler();
	return 0;
}

void vAssertCalled(unsigned long ulLine, const char * const pcFileName){
	printf("ASSERT! Line %d, file %s\r\n", ulLine, pcFileName);
}

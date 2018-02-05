#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TASK_STACK_SIZE 1024
#define MAX_QUEUE_LENGTH 5
#define MAX_MESSAGE_LENGTH 20

xQueueHandle globalQueue;

void senderTask(void* data) {
	int TIMEOUT = 1000;
	int SUSPEND_TIME = 1000;
	char* name = (char*)data;
	int counter = 0, status = 0;
	while (1) {
		status = xQueueSend(globalQueue, &counter, TIMEOUT);
		counter++;
		printf("%s: sending message - %s\n", name, status == pdTRUE ? "SUCCESS" : "FAILED");
		vTaskDelay(SUSPEND_TIME);
	}
}

void receiverTask(void* data) {
	int TIMEOUT = 2000;
	int DELAY = 5000;
	char* name = (char*)data;
	int status = 0;
	int counter = 0;
	while (1) {
		status = xQueueReceive(globalQueue, &counter, TIMEOUT);
		printf("%s: receiving message - %d\n", name, status == pdTRUE ? counter : -1);
		// Let's cause some queue jam here...
		vTaskDelay(DELAY);
	}
}


int main() {
	printf("Simple Task Communication\n");
	globalQueue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(int));
	if (globalQueue == NULL) {
		printf("Houston, we have some trouble: queue has not been created");
		return 1;
	}
	xTaskCreate(senderTask, "sender", TASK_STACK_SIZE, "Sender task", 1, NULL);
	xTaskCreate(receiverTask, "receiver", TASK_STACK_SIZE, "Receiver task", 2, NULL);
	vTaskStartScheduler();
	return 0;
}

void vAssertCalled(unsigned long ulLine, const char * const pcFileName){
	printf("ASSERT! Line %d, file %s\r\n", ulLine, pcFileName);
}

/*
	Development of Real-Time-Systems
	Module: The Principles of Scheduling
	Assignee: Yevhenii Maliavka
*/

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

// Data of a task according to the requirements
struct TaskData {
	// A string to print out
	char* displayString;
	// Job period
	int miliTimeout;
};

// Options for taks creation
struct TaskOptions {
	char* name;
	int stackSize;
	int priority;
	struct TaskData data;
};

// TaskCreateSafe function definition
// outputs error when creation was unsuccessful
// More: https://www.freertos.org/a00125.html
BaseType_t TaskCreateSafe(
	TaskFunction_t pvTaskCode,
	const char* const pcName,
	unsigned short usStackDepth,
	void* pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t *pxCreatedTask);

// Tasks are normally implemented as an infinite loop
// must never attempt to return or exit from their implementing function
void task(void* data) {
	struct TaskData* tdata = (struct TaskData*) data;
	// Block for specific amount of time
	// More: https://www.freertos.org/a00127.html
	const TickType_t delay = tdata->miliTimeout / portTICK_PERIOD_MS;
	while (1) {
		printf("%s\n", tdata->displayString);
		vTaskDelay(delay);
	}
}

// Entry Point
int main() {
	// 1. Create task parameters
	struct TaskOptions task1Options, task2Options;

	// 2. Initialize task 1 parameters
	task1Options.name = "Task1";
	task1Options.stackSize = 1000;
	task1Options.priority = 3;
	task1Options.data.displayString = "This is task 1";
	task1Options.data.miliTimeout = 100;

	// 3. Initialize task 2 parameters
	task2Options.name = "Task2";
	task2Options.stackSize = 100;
	task2Options.priority = 1;
	task2Options.data.displayString = "This is task 2";
	task2Options.data.miliTimeout = 500;

	// 4. Create tasks
	// xTaskCreate creates a new task and adds it to the list of tasks that are ready to run
	// Each task requires RAM that is used to hold the task state, and used by the task as its stack.
	// The required RAM is automatically allocated from the FreeRTOS heap

	// NOTE! Low priority numbers denote low priority tasks

	// 4.1 Create Task 1
	TaskCreateSafe(task, task1Options.name, task1Options.stackSize, &task1Options.data, task1Options.priority, NULL);

	// 4.1 Create Task 2
	TaskCreateSafe(task, task2Options.name, task2Options.stackSize, &task2Options.data, task2Options.priority, NULL);

	// 5. Starts the RTOS scheduler. After calling the RTOS kernel has control over which tasks are executed and when.
	vTaskStartScheduler();
	return 0;
}

void vAssertCalled(unsigned long ulLine, const char * const pcFileName){
	printf("ASSERT! Line %d, file %s\r\n", ulLine, pcFileName);
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
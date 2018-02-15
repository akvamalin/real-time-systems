/*
	Development of Real-Time-Systems
	Module: Static Scheduling
	Done here: Fixed Priority Scheduling
	Assignee: Yevhenii Maliavka
*/

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#define MATRIX_SIZE 20
#define true 1

TaskHandle_t communicationTaskHandler = NULL, matrixTaskHandler = NULL;
int communicationTaskExecution = 0, 
	communicationTaskCounter = 0, 
	matrixTaskExecution = 0, 
	matrixTaskCounter = 0;

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

double** allocateMatrix() {
	int i = 0;
	double**a = (double**)pvPortMalloc(MATRIX_SIZE * sizeof(double*));
	for (i; i < MATRIX_SIZE; i++) {
		a[i] = (double*)pvPortMalloc(MATRIX_SIZE * sizeof(double));
	}
	return a;
}

void fillMatrix(double value, double** matrix) {
	int i = 0, j = 0;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			matrix[i][j] = value;
		}
	}
}

void simulateCPUWorkload() {
	long simulation = 0;
	for (simulation; simulation < 1000000000; simulation++);
}

double matrixesProductSum(double** a, double** b) {
	double sum = 0.0;
	int i = 0, j = 0, k = 0;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			for (k = 0; k < MATRIX_SIZE; k++) {
				sum += a[j][i] * b[i][j];
			}
		}
	}
	return sum;
}

void matrixTask() {
	// Period of the communication Task: 100ms
	int period = 100;
	double** a = allocateMatrix();
	double** b = allocateMatrix();
	double** c = allocateMatrix();
	fillMatrix((double)1.5, a);
	fillMatrix((double)2.6, b);
	int i = 0, j = 0;
	while (true) {
		printf("Calculating...\n");
		simulateCPUWorkload();
		fillMatrix((double)0.0, c);
		for (i = 0; i < MATRIX_SIZE; i++) {
			for (j = 0; j < MATRIX_SIZE; j++) {
				c[i][j] = matrixesProductSum(a, b);
			}
		}
		matrixTaskExecution = matrixTaskCounter;
		printf("Calculated! Matrix Task Execution Time: %d\n", matrixTaskExecution);
		matrixTaskCounter= 0;
		vTaskDelay(period);
	}
}

void communicationTask() {
	// Period of the communication Task: 100ms + I/O operations 100ms = 200ms
	int writingDuration = 100;
	int period = 100;
	while (true) {
		printf("Sending data...\n");
		vTaskDelay(writingDuration);
		communicationTaskExecution = communicationTaskCounter + writingDuration;
		printf("Data sent! Communication Task Execution Time: %d\n", communicationTaskExecution);
		communicationTaskCounter = 0;
		vTaskDelay(period);
	}
}

void priorityManagerTask() {
	// Set task phase to 210 so that the first conditions can be evaluated
	// 200 Is the conditional value and 10 - extra ticks that may occur
	vTaskDelay(210);
	int lastCommunicationTaskPriority = 1;
	while (true) {
		if (communicationTaskExecution > 1000) {
			printf("Increasing priority of Communication task\n");
			vTaskPrioritySet(communicationTaskHandler, 4);
			lastCommunicationTaskPriority = 4;
		}
		// so firstly we check if the execution time has already been measured
		// then we check if the priority has not already been set to 2
		// then we compare it to supposed 200ms execution time
		else if (communicationTaskExecution != 0 &&
				 lastCommunicationTaskPriority!= 2 &&
				 communicationTaskExecution < 200) {
			printf("Decreasing priority of Communication task\n");
			vTaskPrioritySet(communicationTaskHandler, 2);
			lastCommunicationTaskPriority = 2;
		}
		// Period
		vTaskDelay(210);
	}
}

void vApplicationTickHook(void){
	TaskHandle_t currentTaskHandler = xTaskGetCurrentTaskHandle();
	eTaskState communicationTaskState = eTaskGetState(communicationTaskHandler);
	eTaskState matrixTaskState = eTaskGetState(matrixTaskHandler);

	if (currentTaskHandler == matrixTaskHandler) {
		if (communicationTaskState == eReady) {
			// This means that matrix task has preempted the communication task
			// and has put it into the READY state -> the communication task must wait
			communicationTaskCounter++;
		}
		matrixTaskCounter++;
	}
	else if (currentTaskHandler == communicationTaskHandler) {
		if (communicationTaskState == eReady) {
			// This means that communication task has preempted the matrix task
			// and has put it into the READY state -> the matrix task must wait
			matrixTaskCounter++;
		}
		communicationTaskCounter++;
	}
}

// Entry Point
int main() {
	// xTaskCreate creates a new task and adds it to the list of tasks that are ready to run
	// Each task requires RAM that is used to hold the task state, and used by the task as its stack.
	// The required RAM is automatically allocated from the FreeRTOS heap
	// NOTE! Low priority numbers denote low priority tasks
	// Starts the RTOS scheduler. After calling the RTOS kernel has control over which tasks are executed and when.
	TaskCreateSafe(priorityManagerTask, "PriorityManager", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	TaskCreateSafe(matrixTask, "MatrixTask", configMINIMAL_STACK_SIZE * 20, NULL, 3, &matrixTaskHandler);
	TaskCreateSafe(communicationTask, "CommunicatioTask", configMINIMAL_STACK_SIZE, NULL, 1, &communicationTaskHandler);
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


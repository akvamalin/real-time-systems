/*
	Development of Real-Time-Systems
	Module: Static Scheduling
	Assignee: Yevhenii Maliavka
*/

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#define MATRIX_SIZE 20
#define true 1

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
		printf("Calculated!\n");
		vTaskDelay(100);
	}
}

void communicationTask() {
	while (true) {
		printf("Sending data...\n");
		vTaskDelay(100);
		printf("Data sent!\n");
		vTaskDelay(100);
	}
}

void vApplicationTickHook(void)
{
	printf("Hook\n");
}

// Entry Point
int main() {
	// xTaskCreate creates a new task and adds it to the list of tasks that are ready to run
	// Each task requires RAM that is used to hold the task state, and used by the task as its stack.
	// The required RAM is automatically allocated from the FreeRTOS heap

	// NOTE! Low priority numbers denote low priority tasks

	// Starts the RTOS scheduler. After calling the RTOS kernel has control over which tasks are executed and when.
	//TaskCreateSafe(matrixTask, "MatrixTask", configMINIMAL_STACK_SIZE * 20, NULL, 3, NULL);
	//TaskCreateSafe(communicationTask, "CommunicatioTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
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


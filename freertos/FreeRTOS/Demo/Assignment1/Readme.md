# The Development of Real-Time Systems
**Module:** The Principles of Scheduling

**Assignee:** Yevhenii Maliavka

## Simple Tasks Scheduling
> The objective of this assignment is to create two tasks and provide basic scheduling, learn basic FreeRTOS API.

### Target
Implement task scheduling using FreeRTOS API, namely: 
1. Create and start two tasks each of them outputs a provided string every specific for this task amount of time.
2. Provide basic scheduling exposed by FreeRTOS.

### Implementation

I have following libraries included into the project:
```c
	#include <stdio.h> // exposes printf
	#include "FreeRTOS.h" // FreeRTOS Kernel, contains defines and types
	#include "task.h" // exposes API for creating a task and its types

```

According to the task requirements, I have created two data structures:
* `TaskData` contains data used by a task.
* `TaskOpts` contains data with parameters for creating a task that correspond to the **xTaskCreate** function signature as well as an instance of **TaskData**

```c
struct TaskData {
	char* displayString;
	int miliTimeout;
};

struct TaskOptions {
	char* name;
	int stackSize;
	int priority;
	struct TaskData data;
};
```

According to the assignment requirements the task should display a specified string with a provided delay. That means the job will repeat in a certain perion which value equals to **delay**.

```c
void task(void* data) {
	struct TaskData* tdata = (struct TaskData*) data;
	const TickType_t delay = tdata->miliTimeout / portTICK_PERIOD_MS;
	while (1) {
		printf("%s\n", tdata->displayString);
		vTaskDelay(delay);
	}
}
```

In the **main** entry point:
* create and initialize described structures according to the assignment requirements
* create two tasks that are queued into the `Ready State` list if created successfuly 
* start the RTOS scheduler

```c
int main() {
	struct TaskOptions task1Options, task2Options;
	task1Options.name = "Task1";
	task1Options.stackSize = 1000;
	task1Options.priority = 3;
	task1Options.data.displayString = "This is task 1";
	task1Options.data.miliTimeout = 100;
	task2Options.name = "Task2";
	task2Options.stackSize = 100;
	task2Options.priority = 1;
	task2Options.data.displayString = "This is task 2";
	task2Options.data.miliTimeout = 500;
	TaskCreateSafe(task, task1Options.name, task1Options.stackSize, &task1Options.data, task1Options.priority, NULL);
	TaskCreateSafe(task, task2Options.name, task2Options.stackSize, &task2Options.data, task2Options.priority, NULL);
	vTaskStartScheduler();
	return 0;
}
```

There are also two extra functions for simplifying debugging and error tracking.

## Extra Things I learned
When creating a task, low priority numbers denote low priority tasks. I was confused when I saw the output of the first task with prio 3 appearing in the console and only then output from task 2 with prio 1. In another real time operating system I tried out before the lower number prio denoted the higher prioritized task. More to the priorities in RTOS: https://www.freertos.org/RTOS-task-priority.html

## Outro
It would be great if you detect and let me know if there are some errors or mistakenly understood concepts. 
# Real-Time-Systems using FreeRTOS
> Learning real-time systems using FreeRTOS OS on the Coursera. This project contains the practical assignments, implementations of the practical tasks and complete projects.

## Hello World Template
> This is the first time I am getting in touch with FreeRTOS and I want to have my project as clean as possible, so I will try to remove all the dependencies and libs I don't need right now and would eventually include them back as I need them. Some of the proposed `Clean-Up` steps might be wrong, I have not investigated the documentation properly, but these instructions will give a working version of the project to proceed to the basic practical assignments or to do the proposed 10 Mi. tutorial (where the project is clean and already set-up). Feel free to edit this instructions or fix the errors of this docs using pull requests or just get in touch with me. 

### Note!
This project does not includes source files of the FreeRTOS Kernel and its libs. If you want to start with a template right now, you must clone the `Template` and put it under the `FreeRTOS > Demo`. Then just open the project with Visual Studio, Build it and Run.

## Step-by-step instructions

### Installation
1. Download [Visual Studio Community Installer](https://www.visualstudio.com/de/vs/community/).
2. Start Installer and select `Linux Development with C++` workload from the list of workloads.
3. Install the IDE.
4. Download and unzip [FreeRTOSV8.2.3](https://www.it.abo.fi/coursera/RTS/FreeRTOSV8.2.3.zip).
5. Start the Visual Studio Community and Open the `Solution` under the `FreeRTOSV8.2.3 > FreeRTOS > Demo > WIN32-MSVC`.
6. Build the solution and debug the program.
The program should be running, however AFAIK there will be no output.

### Target
Create a `Hello World` project and remove all unnecessary/unused dependencies so that this project:
* can be a base for the next projects.
* the dependencies are included as they are needed.
* the project is lightweight.
* I know what including these or those dependencies mean.

### Steps
1. Go to the `FreeRTOS > Demo`.
2. Remove everything except `WIN32-MSVC` and `Common`.
3. Rename `WIN32-MSVC` to `Template` (this will be eventually a template project to copy-n-paste).
4. In the FreeRTOSConfig.h set 
	* `configUSE_TRACE_FACILITY` to 0 and comment `#include "trcKernelPort.h"`
	* `configUSE_MALLOC_FAILED_HOOK` to 0.
	* `configUSE_IDLE_HOOK`	to 0.
	* `configUSE_TICK_HOOK` to 0.
	* `configUSE_MALLOC_FAILED_HOOK` to	0.
	* `configGENERATE_RUN_TIME_STATS` 0.
	* Comment out:
	```c
	/* Run time stats gathering configuration options. */
	unsigned long ulGetRunTimeCounterValue( void ); /* Prototype of function that returns run time counter. */
	void vConfigureTimerForRunTimeStats( void );	/* Prototype of function that initialises the run time counter. */
	#define configGENERATE_RUN_TIME_STATS			0
	#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()
	#define portGET_RUN_TIME_COUNTER_VALUE() ulGetRunTimeCounterValue()
	``` 
5. Remove `trcConfig.h`, `Trace_Recorder_Configuration`, `Run-time-stats-utils.c`, `Trace.dump`.
6. Remove the `heap_5.c` from the `FreeRTOS Source > Source > Portable` and add `heap_1.c` that can be found under `FreeRTOS > Source > portable  > MemMang`. The files should be added using Visual Studio: right click on a folder, `Add Existing Item`. The original files can be still found using file explorer, AFAIK.
Details about [Memory Management](https://www.freertos.org/a00111.html).
7. Remove all `main***.c` files
7. Add a brand new `main.c`.
```c
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
```
Building and running the application should result into a loop printing the defined string.
This template can now be used for different practical assignments.
The project structure may look following in Visual Studio: 

 ![Project Structure](structure.PNG)


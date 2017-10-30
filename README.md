# real-time-systems

This is just a self-educational project that contains the implementation of the practical tasks from the course "Real-time systems" in the University of Applied Science Mersebug and is published here to trace the code changes and host the source code. 

This project is not the reference or tutorial, may contain errors and is not and doesn't target the "best-practices" implementation. It also contains some short informational snippets from Wikipedia to make a quick recap of the entities or mechanisms being used or implemented.

## 1.1 TO DO: Describe first task

## 1.2 Inter-task communication

Task - in the context of the "Real-time Systems" under microC OS II is a thread within a process.

### Target

The task is to program a system with the following functions:
* **"keyboard" task**, that reads 5 times pro seconds an input from the keyboard. If the `ESC` key has been pressed, the program must stop its execution. Pressing the keys `1-4` starts the corresponding numerated task. 
* **4 working tasks (workers)**, each of them has a `call-counter` and a `timeout-counter`. All counters are initialized with 0 at the beginning.

| Task 1 | Task 2 | Task 3 | Task 4 |
|--------|--------|--------|--------|
| CC:0 | CC:0 | CC:0 | CC:0 |
| TC:0 | TC:0 | TC:0 | TC:0 |

Each of the 4 workers increment their own call-counter after getting called from the keyboard task, and  the timeout-counter if they are not getting called longer than 4 seconds.

The inter-task communication must be implemented using: 
* globar variable
* semaphore
* message (mailbox)

### Theory

Interprocess communication (IPC) refers specifically to the mechanisms an operating system provides to allow the processes to manage shared data. Depending on solution IPC mechanism may provide synchronization or leave it up to processes and threads communicating (such as shared memory).

### Semaphore
A **semaphore** is a variable or abstract data type used to control access to a common resource by multiple processes in a concurrent system . A trivial semaphore is a plain variable that is changed depending on programmer-defined conditions. The variable is then used as a condition to control access to some system resource. 

Semaphores are a useful tool in the prevention of [race conditions](https://en.wikipedia.org/wiki/Race_condition). Semaphores which allow an arbitrary resource count are called **counting semaphores**, while semaphores which are restricted to the values 0 and 1 (or locked/unlocked, unavailable/available) are called **binary semaphores** and are used to implement locks.A mutex is essentially the same thing as a binary semaphore and sometimes uses the same basic implementation. The differences between them are in how they are used. While a binary semaphore may be used as a mutex, a mutex is a more specific use-case, in that only the thread that locked the mutex is supposed to unlock it. Click [here](https://en.wikipedia.org/wiki/Semaphore_(programming)) to read more about semaphores.

#### `Comm() = OSSemCreate(x)`
This function returns the pointer for a created semaphore.
`Comm()` is a pointer of the OS_EVENT type.
If we got NULL, then we should log an error, as the semaphore could not be created.

1. Wait for an event x = 0.
2. Lock critical code x = 1.
3. Counting semaphore x > 1

* If the internal value of the x = 0, the semaphore is not available and the process should wait. We should wait for an event.
* If the internal value of the x = 1, the semaphore will be decremented and the process continues execution.
* If x > 1, then the x will be decremented and the process continues its execution. This would be a counter semaphore.

#### `OSSemPend(semaphore, timeout, &err)`
* If timeout = 0, then we wait, until the semaphore is available.
* If timeout > 0, then we wait a number of ticks. 
When number of ticks is 0, then we enter the critical code and should check &err, if the semaphore is available (has benn posted).

#### `OSSemPost`
`status = OSSemPost(semaphore);`
* OS_NO_ERR
* Overflow happens when we post semaphore, that has been already posted.

#### `OSSemQuery and OSSemAccept`
It is possible to obtain a semaphore without putting a task to sleep if the semaphore is not
available.The code that called OSSemAccept() needs to examine the returned value. A returned value of zero indicates that the semaphore is not available; a nonzero value indicates that the semaphore is available. Furthermore, a nonzero value indicates to the caller the number of resources that are available.

### `Mbox and Queue`
* Create
* Post
* Pend
* Query
* Accept

## 1.3 Performance test

### Target
Create 2, 10, 20, 40 Tasks, where each next task waits for its semaphore being posted from the previous task, release a semaphore for the next one and pends its semaphore again. 
The initial tasks displays the number of round trips per second.
A round trip increments every time when the list of created tasks has been iterated through.
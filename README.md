# real-time-systems

## Inter-task communication

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

A **semaphore** is a variable or abstract data type used to control access to a common resource by multiple processes in a concurrent system . A trivial semaphore is a plain variable that is changed depending on programmer-defined conditions. The variable is then used as a condition to control access to some system resource. 

Semaphores are a useful tool in the prevention of [race conditions](https://en.wikipedia.org/wiki/Race_condition). Semaphores which allow an arbitrary resource count are called **counting semaphores**, while semaphores which are restricted to the values 0 and 1 (or locked/unlocked, unavailable/available) are called **binary semaphores** and are used to implement locks.A mutex is essentially the same thing as a binary semaphore and sometimes uses the same basic implementation. The differences between them are in how they are used. While a binary semaphore may be used as a mutex, a mutex is a more specific use-case, in that only the thread that locked the mutex is supposed to unlock it.Click [here](https://en.wikipedia.org/wiki/Semaphore_(programming)) to read more about semaphores.
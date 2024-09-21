# A single thread scheduler

1. The task thread is a separate thread that is responsible for executing tasks in the task queue, ensuring that tasks are executed in the order they are scheduled.
2. A single thread can ensure that there are no concurrency issues.
3. Task object stores the file and line number where the task is scheduled.
4. When the task is executed, it executes its Callable member, and when an exception is thrown, we can locate the source of the exception based on the file and line number stored in the task object.
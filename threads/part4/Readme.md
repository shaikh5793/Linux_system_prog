## Thread Termination in Linux with POSIX Threads (pthreads)

This document covers methods for terminating threads in Linux using the POSIX Threads (pthreads) library. While the core concepts are generally applicable to other POSIX-compliant systems, always consult your system's documentation for precise details. 

**Methods**

* **`pthread_cancel(pthread_t thread)`**
   * Sends a cancellation request to the target thread (`thread`).
   * The target might not terminate instantly; it must reach cancellation points or change its cancellation state for termination to occur.
   * **Example:**
     ```c
     pthread_cancel(thread_id);
     ``` 

* **`pthread_exit(void *status)`**
   * A thread calls this to terminate itself.
   * An optional `status` can be passed, retrievable by other threads joining this one.
   * **Example:**
     ```c
     pthread_exit(NULL);  
     ```

* **Signals**
   * Unhandled signals can terminate threads forcefully (and might bypass cleanup). Examples include SIGTERM, SIGKILL, SIGINT.
   * Threads can manage signals using signal masks (`pthread_sigmask`).

**Cleanup Handlers**

* **`pthread_cleanup_push(void (*routine)(void *), void *arg)`**
   * Pushes a cleanup routine (`routine`) onto a stack. It's executed when the thread is canceled or exits via `pthread_exit`.
   * `arg` is an argument passed to the cleanup routine.
   * Handlers are executed in LIFO order (last in, first out). 
* **`pthread_cleanup_pop(int execute)`**
   * Removes the most recently pushed handler.
   * If `execute` is non-zero, the handler executes immediately.

**Guaranteed Cleanup Execution**

Cleanup handlers are guaranteed to run when:

* A thread calls `pthread_exit()`.
* A thread is canceled via `pthread_cancel()`.

**Caveats**

Cleanup handlers might *not* run when:

* The entire process terminates (e.g., via `exit()`).
* A thread receives an unhandled signal that terminates it forcefully.

**Cancellation Points**

* `pthread_cancel()` sends a cancellation *request*. The target thread won't terminate immediately unless it cooperates.
* Cancellation points are specific locations in a thread's code where it explicitly checks and acts upon cancellation requests.
* Functions that act as cancellation points include:
    * `pthread_testcancel()`
    * Blocking functions (e.g., `sleep`, `read`, `select`, certain mutex operations)
* Use `pthread_setcancelstate` and `pthread_setcanceltype` to manage how a thread responds to cancellation requests.

**Interactions with Cleanup Handlers**

* Cleanup handlers registered with `pthread_cleanup_push` are **guaranteed** to execute if the thread reaches a cancellation point and acts upon the cancellation request.
* This ensures proper resource cleanup (closing files, releasing memory, unlocking mutexes) even during a cancellation.

Additional Notes

Design your threads to reach cancellation points frequently for timely responses to cancellation requests.
Carefully consider the order of cleanup handlers, especially when dealing with nested resources.
Resources

The Linux man pages:
pthread_cancel: https://man7.org/linux/man-pages/man3/pthread_cancel.3.html
pthread_testcancel: https://man7.org/linux/man-pages/man3/pthread_testcancel.3.html
pthread_setcancelstate: https://man7.org/linux/man-pages/man3/pthread_setcancelstate.3.html
pthread_setcanceltype: https://man7.org/linux/man-pages/man3/pthread_setcanceltype.3.html

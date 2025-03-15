Assignment: Multithreaded Prime Number Counter with Load Balancing

Objective

You will implement a multithreaded prime number counter that finds all prime numbers within a given range [1, N]. The program should:
    1.    Efficiently divide the workload among multiple threads.
    2.    Use thread synchronization to manage shared data safely.
    3.    Implement dynamic load balancing to handle uneven workloads.
    4.    Compare single-threaded vs. multi-threaded performance.

Requirements

1. Input Handling
    •    Accepts N (upper limit) as a command-line argument.
    •    Reads T (number of threads) dynamically based on available CPU cores.

2. Prime Checking Implementation
    •    Uses the Sieve of Eratosthenes (fast) or a brute-force primality test (easier).
    •    Divides the work among T threads efficiently.

3. Threading Strategy
    •    Uses pthreads (C) or std::thread (C++) for multithreading.
    •    Implements static or dynamic workload allocation:
    •    Static allocation: Each thread gets a fixed range [start, end].
    •    Dynamic allocation: Threads pull numbers from a shared queue to balance load.

4. Thread Synchronization
    •    Uses mutexes or atomic variables to prevent race conditions.
    •    Ensures threads don’t overwrite shared memory when counting primes.

5. Performance Comparison
    •    Implements both single-threaded and multi-threaded versions.
    •    Measures execution time using std::chrono (C++) or clock_gettime (C).
    •    Prints speedup factor (single_thread_time / multi_thread_time).

6. Output
    •    Displays the total count of prime numbers found.
    •    Prints execution time and performance results.

Deliverables
    •    Source code (.c or .cpp file)
    •    Short report (1-2 pages) explaining:
    •    How multithreading was implemented
    •    Challenges faced (e.g., synchronization issues, workload balancing)
    •    Performance results (single-threaded vs. multi-threaded execution time)

Hints for Implementation

Prime Checking
    •    A brute-force check: Loop from 2 to sqrt(n).
    •    Sieve of Eratosthenes: More efficient, but requires shared memory management.

Threading Strategy
    •    Static allocation:
    •    Thread i handles numbers in range [i * chunk_size, (i+1) * chunk_size].
    •    Dynamic allocation (more complex but better performance):
    •    Use a shared queue (std::queue in C++ or a thread-safe queue in C).
    •    Each thread fetches numbers dynamically, reducing idle time.

Synchronization
    •    Use mutexes (std::mutex in C++ or pthread_mutex_t in C) when updating shared prime count.
    •    For better performance, use atomic variables (std::atomic<int> in C++).
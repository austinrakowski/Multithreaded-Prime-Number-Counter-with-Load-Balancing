#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

// Shared data
typedef struct {
    int check_val;           
    int current_num;          
    int total_primes;         
    pthread_mutex_t lock;     
} ThreadData;

// Arguments for thread function
typedef struct {
    ThreadData* data;       
    int id;                 
    int work_size;          
} ThreadInfo;

bool isPrime(int num) {
    
    if (num <= 1) {
        return false; 
    }
    int max = sqrt(num);
    for (int i = 2; i <= max; i++) {
        if (num % i == 0) {
            return false; //false if divisor found
        }
    }
    return true; // prime if no divisors found
}

// Function to run in each thread
void* findPrimes(void* args) {

    ThreadInfo* info = (ThreadInfo*)args;
    ThreadData* data = info->data;
    int thread_id = info->id;
    int chunk = info->work_size;
    
    int primes = 0;
    
    int start, end;
    
    while (1) {
        
        pthread_mutex_lock(&data->lock);
        
        // Get next chunk of numbers to check
        start = data->current_num;
        end = start + chunk - 1;
        
        // break when no more values to check
        if (start > data->check_val) {
            pthread_mutex_unlock(&data->lock);
            break;
        }
        
        // Update the next number to be checked
        data->current_num = end + 1;
        
        // Release the lock
        pthread_mutex_unlock(&data->lock);
        
        // Make sure we don't go past the limit
        if (end > data->check_val) {
            end = data->check_val;
        }
        
        // brute force check of numbers in chunk
        for (int i = start; i <= end; i++) {
            if (isPrime(i)) {
                primes++;
            }
        }
    }
    
    pthread_mutex_lock(&data->lock);
    data->total_primes += primes;
    pthread_mutex_unlock(&data->lock);
    
    return NULL;
}

// Single-threaded version
int countPrimesSingleThread(int max) {
    int count = 0;
    for (int i = 1; i <= max; i++) {
        if (isPrime(i)) {
            count++;
        }
    }
    return count;
}

double getTime() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (double)time.tv_sec + (double)time.tv_nsec / 1000000000.0;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Please provide a number to check\n");
        return 1;
    }
    
    // Get the number to check
    int check_val = atoi(argv[1]);
    if (check_val <= 0) {
        printf("Error: Please enter a positive number.\n");
        return 1;
    }
    
    printf("Looking for primes using 1 thread...\n");
    
    // time the single thread version
    double start_time = getTime();
    int single_result = countPrimesSingleThread(check_val);
    double single_time = getTime() - start_time;

    printf("Single thread: Found %d primes in %.4f seconds\n", 
           single_result, single_time);

    // Get number of CPU cores for threads
    int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Detected %d CPU cores available.\n", num_threads);

    printf("Looking for primes using %d threads...\n", num_threads);

    // Initialize the shared data
    ThreadData data;
    data.check_val = check_val;
    data.current_num = 1;
    data.total_primes = 0;
    pthread_mutex_init(&data.lock, NULL);
    
    // Calculate chunk size dynamically based on the range and number of threads
    int chunk_size = check_val / (num_threads * 10);
    
    printf("Using chunk size of %d numbers per work unit\n", chunk_size);
    
    // fixed memory allocation for threads array
    #define MAX_THREADS 64
    pthread_t threads[MAX_THREADS];
    ThreadInfo thread_info[MAX_THREADS];
    
    // Time the multi-threaded version
    start_time = getTime();
    
    // Start threads
    for (int i = 0; i < num_threads; i++) {
        thread_info[i].data = &data;
        thread_info[i].id = i;
        thread_info[i].work_size = chunk_size;
        
        int result = pthread_create(&threads[i], NULL, findPrimes, &thread_info[i]);
        if (result != 0) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double multi_time = getTime() - start_time;
    
    // Show results
    printf("Multi thread: Found %d primes in %.4f seconds\n", 
           data.total_primes, multi_time);
    printf("Speed improvement: %.2f times faster\n", single_time / multi_time);
    
    // Check if both versions got the same answer
    if (single_result != data.total_primes) {
        printf("WARNING: Got different results between single and multi-threaded versions!\n");
    }
    
    pthread_mutex_destroy(&data.lock);
    
    return 0;
}
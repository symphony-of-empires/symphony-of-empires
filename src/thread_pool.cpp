#include "thread_pool.hpp"
#include <algorithm>

/**
 * Constructs the thread pool, this initializes threads for the pool
 */
ThreadPool::ThreadPool() {
    // It's a good idea to use as many threads as the hardware implementation
    // supports. Otherwise we can run into performance hits.
    const size_t n_threads = (size_t)std::thread::hardware_concurrency();

    this->running = true;

    //this->threads = std::vector<std::thread>(n_threads, th);
    for(size_t i = 0; i < n_threads; i++) {
        this->threads.push_back(std::thread(&ThreadPool::thread_loop, this));
    }

    // This vector is not going to expand anymore
    this->threads.shrink_to_fit();

    // We now have our threads running at this point - waiting for jobs to take
}

/**
 * Destructs the thread pool, closing all the pending jobs
 */
ThreadPool::~ThreadPool() {
    // We are going to signal all threads to shutdown
    this->running = false;

    // After that we will start joining all threads - if a thread is executing
    // by a prolonged time, it will block the entire process
    for(std::thread& job: this->threads) {
        job.join();
    }
}

/**
 * Adds a job to the list of pending jobs
 */
void ThreadPool::add_job(std::function<void()> job) {
    std::unique_lock<std::mutex> lock(this->job_mutex);
    this->jobs.push(job);
}

/**
 * This loop is executed on each thread on the thread list, what this basically does
 * is to check in the list of available jobs for jobs we can take
 */
void ThreadPool::thread_loop(void) {
    while(this->running) {
        std::function<void()> fn;

        // We need to make it be like this so locking lasts until the end of the
        // scope. We can't keep jobs queue locked while we execute a job... that
        // would be extremely dumb
        {
            std::unique_lock<std::mutex> lock(this->job_mutex);

            // If there are no available jobs for us to take, we will continue our loop
            if(this->jobs.empty())
                continue;
                
            // Otherwise we execute said job
            fn = this->jobs.back();
            this->jobs.pop();
        }

        // We are now going to call this function and hope it dosen't blocks infinitely
        fn();

        // Now yield back to the system
        std::this_thread::yield();
    }
}
// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      thread_pool.cpp
//
// Abstract:
//      Generalized thread pool implementation.
// ----------------------------------------------------------------------------

#include "unified_render/thread_pool.hpp"
#include <algorithm>

//
// ThreadPool
//

// Constructs the thread pool, this initializes threads for the pool
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

// Destructs the thread pool, closing all the pending jobs
ThreadPool::~ThreadPool() {
    // We are going to signal all threads to shutdown
    std::unique_lock<std::mutex> latch(job_mutex);
    running = false;
    cv_task.notify_all();
    latch.unlock();

    // After that we will start joining all threads - if a thread is executing
    // by a prolonged time, it will block the entire process
    std::vector<std::thread>::iterator job;
    for(job = threads.begin(); job != threads.end(); job++) {
        (*job).join();
    }
}

// Adds a job to the list of pending jobs
void ThreadPool::add_job(std::function<void()> job) {
    std::scoped_lock lock(job_mutex);
    jobs.push(job);
    cv_task.notify_one();
}

// Waits until the job list is empty
void ThreadPool::wait_finished() {
    std::unique_lock<std::mutex> lock(job_mutex);
    cv_finished.wait(lock, [this]() {
        return (jobs.empty() && (busy == 0));
    });
}

// This loop is executed on each thread on the thread list, what this basically does
// is to check in the list of available jobs for jobs we can take
void ThreadPool::thread_loop() {
    while(true) {
        std::unique_lock<std::mutex> latch(job_mutex);
        cv_task.wait(latch, [this]() {
            return (!running || !jobs.empty());
        });

        if(!running) {
            break;
        }

        // pull from queue
        std::function<void()> fn = jobs.front();
        jobs.pop();

        busy++;
        // release lock. run async
        latch.unlock();

        fn();

        latch.lock();
        cv_finished.notify_one();
    }
}
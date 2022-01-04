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
//      thread_pool.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool {
    // An atomic all idling threads use
    std::atomic<bool> running;

    // While std::thread is not copy-able, the std::vector STL container
    // allows for movable elements instead. keep this in mind when a bug happens
    std::vector<std::thread> threads;

    // Queues are suited for the job here, since first-requested-jobs
    // should have higher priority than the last one, this allows sequential
    // completion of tasks
    std::queue<std::function<void()>> jobs;

    // Our mutex for preventing race conditions on the queue above
    std::mutex job_mutex;

    // Called when job is added
    std::condition_variable cv_task;
    // Called when job is finished
    std::condition_variable cv_finished;

    // Amount of threads currently working
    size_t busy = 0;
public:
    ThreadPool();
    ~ThreadPool();
    void add_job(std::function<void()> job);
    void thread_loop(void);
    void wait_finished(void);

    template<typename I, typename F>
    static void for_each(I first, I last, F func) {
        const size_t n_threads = (size_t)std::thread::hardware_concurrency();
        if(!std::distance<I>(first, last))
            return;

        const size_t iterators_per_thread = std::distance<I>(first, last) / n_threads;

        for(; first != last; first++) {
            func(*first);
        }
    }
};
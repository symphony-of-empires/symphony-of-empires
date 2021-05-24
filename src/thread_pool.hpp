#include <thread>
#include <algorithm>
#include <functional>
#include <atomic>
#include <queue>
#include <mutex>

class ThreadPool {
	// An atomic all idling threads use
	std::atomic<bool> running;

	// While std::thread is not copy-able, the std::vector stl container
	// allows for movable elements instead. keep this in mind when a bug comes
	std::vector<std::thread> threads;

	// Queues are better suited for the job here, since first-requested-jobs
	// should have higher priority than the last one, this allows sequential
	// completion of tasks
	std::queue<std::function<void()>> jobs;

	// Our mutex for preventing race conditions on the queue above
	std::mutex job_mutex;
public:
	ThreadPool() {
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

	~ThreadPool() {
		// We are going to signal all threads to shutdown
		this->running = false;

		// After that we will start joining all threads - if a thread is executing
		// by a prolonged time, it will block the entire process
		for(std::thread& job: this->threads) {
			job.join();
		}
	}

	void add_job(std::function<void()> job) {
		std::unique_lock<std::mutex> lock(this->job_mutex);
		this->jobs.push(job);
	}

	void thread_loop(void) {
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
};
#pragma once

#include <queue>
#include <future>

namespace kanso {

	// https://github.com/progschj/thread_pool
	class thread_pool {
		public:
			thread_pool(size_t threads);
			~thread_pool();

			thread_pool(const thread_pool&)              = delete;
			thread_pool& operator=(const thread_pool&)   = delete;
			thread_pool(const thread_pool&&)             = delete;
			thread_pool&& operator=(const thread_pool&&) = delete;

			template <class F, class... Args>
			auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;

		private:
			std::vector<std::thread>          workers_;
			std::queue<std::function<void()>> tasks_;
			std::mutex                        queue_mutex_;
			std::condition_variable           condition_;
			bool                              stop_ = false;
	};

	inline thread_pool::thread_pool(size_t threads) {
		for (size_t i = 0; i < threads; ++i) {
			workers_.emplace_back([this] {
				for (;;) {
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(this->queue_mutex_);
						this->condition_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });
						if (this->stop_ && this->tasks_.empty()) {
							return;
						}
						task = std::move(this->tasks_.front());
						this->tasks_.pop();
					}

					task();
				}
			});
		}
	}

	template <class F, class... Args>
	auto thread_pool::enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
		using return_type = typename std::invoke_result<F, Args...>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
		    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();
		{
			const std::unique_lock<std::mutex> lock(queue_mutex_);

			if (stop_) {
				throw std::runtime_error("enqueue on stopped thread_pool");
			}

			tasks_.emplace([task]() { (*task)(); });
		}
		condition_.notify_one();
		return res;
	}

	inline thread_pool::~thread_pool() {
		{
			const std::unique_lock<std::mutex> lock(queue_mutex_);
			stop_ = true;
		}
		condition_.notify_all();
		for (std::thread& worker : workers_) {
			worker.join();
		}
	}

}; // namespace kanso

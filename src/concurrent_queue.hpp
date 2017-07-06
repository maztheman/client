#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

//full of platform specific stuff
namespace kms {
	template <typename T>
	class concurrent_queue
	{
	public:

		T pop()
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			while (queue_.empty())
			{
				cond_.wait(mlock);
			}
			auto item = queue_.front();
			queue_.pop();
			return item;
		}

		bool try_pop(T* item, std::chrono::milliseconds timeout = std::chrono::milliseconds(25))
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			if (queue_.empty()) {
				return false;
			}
			if (cond_.wait_for(mlock, timeout) == std::cv_status::no_timeout) {
				*item = queue_.front();
				queue_.pop();
				return true;
			}
			return false;
		}

		void pop(T& item)
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			while (queue_.empty())
			{
				cond_.wait(mlock);
			}
			item = queue_.front();
			queue_.pop();
		}

		void push(const T& item)
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			queue_.push(item);
			mlock.unlock();
			cond_.notify_one();
		}

		void push(T&& item)
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			queue_.push(std::move(item));
			mlock.unlock();
			cond_.notify_one();
		}

	private:
		std::queue<T> queue_;
		std::mutex mutex_;
		std::condition_variable cond_;
	};
}

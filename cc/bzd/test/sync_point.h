#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

namespace bzd::test {

template <typename tag>
class SyncPoint
{
public:
	template <bzd::SizeType I>
	struct Type
	{
		Type()
		{
			auto& points = SyncPoint<tag>::getInstance();
			std::unique_lock<std::mutex> lock(points.mutex_);
			points.cv_.wait(lock, [&points] { return points.index_.load() == I; });
			std::cout << "**** Synchronization point " << I << " ****" << std::endl;
			points.index_.store(I + 1);
			points.cv_.notify_all();
		}
	};

	static void reset() noexcept
	{
		SyncPoint<tag>::getInstance().index_ = 1;
	}

private:
	static SyncPoint& getInstance() noexcept
	{
		static SyncPoint<tag> points{};
		return points;
	}

	constexpr SyncPoint() = default;

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	bzd::Atomic<bzd::SizeType> index_{1};
};

}

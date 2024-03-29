#pragma once

#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/platform/types.hh"

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

namespace bzd::test {

template <typename tag>
class SyncPoint
{
public:
	template <bzd::Size I>
	struct Type
	{
		Type()
		{
			auto& points = SyncPoint<tag>::getInstance();
			std::unique_lock<std::mutex> lock(points.mutex_);
			points.cv_.wait(lock, [&points] { return points.index_.load() >= I; });
			std::cout << "**** Synchronization point " << I << " ****" << std::endl;
			bzd::Size expected = I;
			points.index_.compareExchange(expected, I + 1);
			points.cv_.notify_all();
		}
	};

	static void reset() noexcept { SyncPoint<tag>::getInstance().index_ = 1; }

private:
	static SyncPoint& getInstance() noexcept
	{
		static SyncPoint<tag> points{};
		return points;
	}

	SyncPoint() = default;

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	bzd::Atomic<bzd::Size> index_{1};
};

} // namespace bzd::test

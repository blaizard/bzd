#include <mutex>
#include <condition_variable>
#include <thread>

namespace bzd::test {

template <typename tag>
class SyncPoints
{
public:
	template <bzd::SizeType I>
	static auto make() noexcept
	{
		return []() {
			auto& points = SyncPoints<tag>::getInstance();
			std::unique_lock<std::mutex> lock(points.mutex_);
			points.cv_.wait(lock, [&points] { return points.index_.load() == I; });
			std::cout << "**** Synchronization point " << I << " ****" << std::endl;
			points.index_.store(I + 1);
			points.cv_.notify_all();
		};
	}

private:
	static SyncPoints& getInstance() noexcept
	{
		static SyncPoints<tag> points{};
		return points;
	}

	constexpr SyncPoints() = default;

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	bzd::Atomic<bzd::SizeType> index_{1};
};

}

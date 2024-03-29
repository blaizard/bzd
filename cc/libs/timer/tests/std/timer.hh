#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/numeric_limits.hh"
#include "cc/libs/timer/timer_isr.hh"

#include <thread>

namespace bzd::test {

class TimerTest : public bzd::TimerISR<UInt64, TimerTest>
{
public:
	using Tick = UInt64;
	using Duration = UInt32;
	using Element = typename bzd::TimerISR<UInt64, TimerTest>::Element;

public: // Init/shutdown functions.
	bzd::Async<> init() noexcept
	{
		stop_.store(false);
		thread_ = std::thread([this]() {
			while (stop_.load() == false)
			{
				if (alarm_.load() <= getTicks().value())
				{
					alarmClear();
					triggerForISR();
				}
			}
		});
		co_return {};
	}

	bzd::Async<> shutdown() noexcept
	{
		stop_.store(true);
		thread_.join();
		co_return {};
	}

	constexpr TimerTest& operator=(const Tick time) noexcept
	{
		time_.store(time);
		return *this;
	}

	constexpr TimerTest& operator+(const Duration duration) noexcept
	{
		time_ += duration;
		return *this;
	}

	bzd::Async<> delay(const Duration duration) noexcept
	{
		co_await !waitUntilTicks(time_.load() + duration);
		co_return {};
	}

private: // Implementation.
	friend class bzd::TimerISR<UInt64, TimerTest>;

	bzd::Result<Tick, bzd::Error> getTicks() noexcept { return time_.load(); }

	bzd::Result<void, bzd::Error> alarmSet(const Tick alarm)
	{
		alarm_.store(alarm);
		return bzd::nullresult;
	}
	bzd::Result<void, bzd::Error> alarmClear()
	{
		alarm_.store(invalid);
		return bzd::nullresult;
	}

private:
	static constexpr Tick invalid = bzd::NumericLimits<Tick>::max();
	bzd::Atomic<Tick> time_{0};
	bzd::Atomic<Tick> alarm_{invalid};
	bzd::Atomic<Bool> stop_{false};
	std::thread thread_{};
};

} // namespace bzd::test

#include "cc/libs/timer/timer.hh"

namespace bzd::test {

class TimerTest : public bzd::Timer<UInt64, UInt32, TimerTest>
{
public:
	using Time = UInt64;
	using Duration = UInt32;

public: // Init/shutdown functions.
	bzd::Async<> init() noexcept { co_return {}; }

	bzd::Async<> shutdown() noexcept { co_return {}; }

	constexpr TimerTest& operator=(const Time time) noexcept
	{
		time_ = time;
		return *this;
	}

	constexpr TimerTest& operator+(const Duration duration) noexcept
	{
		time_ += duration;
		return *this;
	}

private: // Implementation.
	friend class bzd::Timer<UInt64, UInt32, TimerTest>;

	[[nodiscard]] constexpr Time durationToTime(const Duration duration) noexcept { return time_ + duration; }

	[[nodiscard]] constexpr Time getTime() noexcept { return time_; }

private:
	Time time_{0};
};

} // namespace bzd::test

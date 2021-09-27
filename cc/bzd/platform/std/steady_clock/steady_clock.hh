class SteadyClock
{
public:
	static ClockTick getTicks() noexcept;

	static ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept;

	static bzd::units::Millisecond ticksToMs(const ClockTick ticks) noexcept;
};

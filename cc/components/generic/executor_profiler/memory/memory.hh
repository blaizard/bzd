#pragma once

#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/async/async.hh"
#include "cc/bzd/core/async/executor_profiler.hh"
#include "cc/bzd/core/channel.hh"

#include <iostream>

namespace bzd::components::generic {

class ExecutorProfiler
{
public:
	constexpr void event(const auto event) noexcept { events_.pushBack(Event{event}); }

	/// Print all events previously registered.
	inline bzd::Async<> print(bzd::OStream& out) noexcept
	{
		const auto lock = co_await !out.getLock();

		::std::cout << "START PROFILE (" << events_.size() << " event(s))" << ::std::endl;
		for (const auto& event : events_.asSpans())
		{
			event.match([](const bzd::async::profiler::NewCore e) { ::std::cout << "NewCore " << e.uid << ::std::endl; },
						[](const bzd::async::profiler::DeleteCore e) { ::std::cout << "DeleteCore " << e.uid << ::std::endl; },
						[](const bzd::async::profiler::ExecutableScheduled) { ::std::cout << "ExecutableScheduled" << ::std::endl; },
						[](const bzd::async::profiler::ExecutableUnscheduled) { ::std::cout << "ExecutableUnscheduled" << ::std::endl; },
						[](const bzd::async::profiler::ExecutableCanceled) { ::std::cout << "ExecutableCanceled" << ::std::endl; });
		}
		::std::cout << "END PROFILE" << ::std::endl;

		co_return {};
	}

private:
	using Event = bzd::Variant<bzd::async::profiler::NewCore,
							   bzd::async::profiler::DeleteCore,
							   bzd::async::profiler::ExecutableScheduled,
							   bzd::async::profiler::ExecutableUnscheduled,
							   bzd::async::profiler::ExecutableCanceled>;
	bzd::RingBuffer<Event, 4> events_{};
};

} // namespace bzd::components::generic

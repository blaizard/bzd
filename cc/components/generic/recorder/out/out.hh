#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/print.hh"
#include "cc/bzd/core/units.hh"
#include "cc/components/generic/recorder/out/interface.hh"

namespace bzd::components::generic::recorder {

template <class Context>
class Out : public bzd::Recorder<Out<Context>>
{
public:
	constexpr explicit Out(Context& context) noexcept : context_{context} {}

	bzd::Async<> run() noexcept
	{
		while (true)
		{
			co_await !bzd::apply(
				[this](auto&... sink) -> bzd::Async<> {
					(co_await !record(sink), ...);
					co_return {};
				},
				context_.io.sinks);
			co_await !context_.config.timer.delay(bzd::units::Millisecond{700});
			// co_await !bzd::print(context_.config.out, "\033c");
		}
		co_return {};
	}

private:
	template <class Reader>
	bzd::Async<> record(Reader& sink) noexcept
	{
		auto scope = sink.tryGetLast();
		if (scope)
		{
			co_await !bzd::print(context_.config.out, "{}: {}\n"_csv, sink.getName(), scope.value());
		}
		else
		{
			co_await !bzd::print(context_.config.out, "{}: <unset>\n"_csv, sink.getName());
		}
		co_return {};
	}

private:
	Context& context_;
};

} // namespace bzd::components::generic::recorder

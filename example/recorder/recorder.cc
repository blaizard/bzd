#include "example/recorder/recorder.hh"

namespace example {

bzd::Async<> run()
{
	co_await !bzd::print("Hello {}"_csv, bzd::Record<"hello">::id);

	bzd::Recorder recorder;
	recorder.publish<"hello">('a');

	co_return {};
}

} // namespace example

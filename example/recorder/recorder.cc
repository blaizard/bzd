#include "example/recorder/recorder.hh"

#include "example/recorder/composition.hh"

namespace example {

bzd::Async<> Hello::run()
{
	co_await !bzd::print("Hello {}"_csv, bzd::Data<"example.Hello.input">::id);

	bzd::Recorder recorder;
	recorder.publish<"example.Hello.input">(12);

	co_return {};
}

} // namespace example

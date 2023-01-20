#include "cc/bzd/core/channel.hh"
#include "cc/components/posix/stream/out/interface.hh"

#include <iostream>
#include <unistd.h>

namespace bzd::platform::posix {
template <class Context>
class Out : public bzd::OStream
{
public:
	constexpr Out(Context& context) noexcept : context_{context} {}

	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept final
	{
		co_await !context_.config.proactor.write(out_, data);
		::std::flush(::std::cout);
		co_return {};
	}

private:
	Context& context_;
	FileDescriptor out_{STDOUT_FILENO};
};
} // namespace bzd::platform::posix

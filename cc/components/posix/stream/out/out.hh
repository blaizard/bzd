#include "cc/bzd/core/channel.hh"
#include "cc/components/posix/stream/out/interface.hh"

#include <iostream>
#include <unistd.h>

namespace bzd::platform::posix {
template <class Proactor>
class Out : public bzd::OStream
{
public:
	constexpr Out(Proactor& proactor) noexcept : proactor_{proactor} {}

	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept override
	{
		co_await !proactor_.write(out_, data);
		::std::flush(::std::cout);
		co_return {};
	}

private:
	Proactor& proactor_;
	FileDescriptor out_{STDOUT_FILENO};
};
} // namespace bzd::platform::posix

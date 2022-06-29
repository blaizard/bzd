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

	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override
	{
		const auto size = co_await !proactor_.write(out_.borrow(), data);
		::std::flush(::std::cout);
		co_return size;
	}

private:
	Proactor& proactor_;
	FileDescriptorAccessor out_{STDOUT_FILENO};
};
} // namespace bzd::platform::posix

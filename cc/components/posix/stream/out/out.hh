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
		const auto size = co_await !proactor_.write(STDOUT_FILENO, data);
		::std::flush(::std::cout);
		co_return size;
	}

private:
	Proactor& proactor_;
};
} // namespace bzd::platform::posix

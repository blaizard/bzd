#include "cc/bzd/core/channel.hh"
#include "cc/components/posix/stream/out/interface.hh"

#include <iostream>
#include <unistd.h>

namespace bzd::platform::posix {
template <class Config>
class Out : public bzd::OStream
{
public:
	constexpr Out(Config& config) noexcept : config_{config} {}

	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept final
	{
		co_await !config_.proactor.write(out_, data);
		::std::flush(::std::cout);
		co_return {};
	}

private:
	Config& config_;
	FileDescriptor out_{STDOUT_FILENO};
};
} // namespace bzd::platform::posix

#include "cc/bzd/core/channel.hh"
#include "cc/components/std/stream/out/interface.hh"

#include <iostream>
#include <unistd.h>

namespace bzd::platform::std {
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

		//::std::cout.write(reinterpret_cast<const char*>(data.data()), static_cast<::std::streamsize>(data.size()));
		//::std::flush(::std::cout);
		// co_return data.size();
	}

private:
	Proactor& proactor_;
};
} // namespace bzd::platform::std

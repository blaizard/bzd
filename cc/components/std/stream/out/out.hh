#include "cc/bzd/core/channel.hh"

#include <iostream>
#include <unistd.h>

namespace bzd::platform::std {
class Out : public bzd::OStream
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override
	{
		::std::cout.write(reinterpret_cast<const char*>(data.data()), static_cast<::std::streamsize>(data.size()));
		::std::flush(::std::cout);
		co_return data.size();
	}
};
} // namespace bzd::platform::std

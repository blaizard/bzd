#include "cc/bzd/core/channel.hh"

#include <iostream>

namespace bzd::platform::std {
class Out : public bzd::OStream
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override
	{
		std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
		co_return data.size();
	}
};
} // namespace bzd::platform::std

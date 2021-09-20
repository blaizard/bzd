#include "cc/bzd/platform/out.h"

#include <iostream>

namespace {
class StdoutChannel : public bzd::OChannel
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override
	{
		std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
		co_return data.size();
	}
};
} // namespace

bzd::OChannel& bzd::platform::getOut()
{
	static StdoutChannel stdout_;
	return stdout_;
}

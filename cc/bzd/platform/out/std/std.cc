#include "cc/bzd/platform/out.hh"

#include <iostream>

namespace {
class StdoutChannel : public bzd::OStream
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override
	{
		std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
		co_return data.size();
	}
};
} // namespace

bzd::OStream& bzd::platform::getOut()
{
	static StdoutChannel stdout_;
	return stdout_;
}

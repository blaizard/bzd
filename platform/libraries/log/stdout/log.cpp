#include "bzd/core/system.h"

#include <iostream>

namespace {
class StdoutChannel : public bzd::OChannel
{
public:
	bzd::Expected<bzd::SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override
	{
		std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
		return data.size();
	}
};
} // namespace

bzd::OChannel& bzd::getOut()
{
	static StdoutChannel stdout_;
	return stdout_;
}

#include "bzd/core/system.h"

#include <iostream>

namespace {
class StdoutStream : public bzd::OStream
{
public:
	bzd::SizeType write(const bzd::Span<const char> &data) noexcept override
	{
		std::cout.write(data.data(), data.size());
		return data.size();
	}
};
} // namespace

bzd::OStream &bzd::getOut()
{
	static StdoutStream stdout;
	return stdout;
}

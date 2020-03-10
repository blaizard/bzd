#include "bzd/core/system.h"
#include "bzd/core/registry.h"
#include "bzd/core/channel.h"

namespace bzd { namespace impl {

class Stub : public bzd::OStream
{
public:
	SizeType write(const bzd::Span<const char>& data) noexcept override { return data.size(); }
};

}

OStream& getOut()
{
	return bzd::Registry<bzd::OStream>::getOrCreate<impl::Stub>("stdout");
}

} // namespace bzd

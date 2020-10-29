#include "bzd/core/system.h"

#include "bzd/core/channel.h"
#include "bzd/core/registry.h"

namespace bzd {
namespace impl {

class Stub : public bzd::OChannel
{
public:
	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override { return data.size(); }
};

} // namespace impl

OChannel& getOut()
{
	return bzd::Registry<bzd::OChannel>::getOrCreate<impl::Stub>("stdout");
}

} // namespace bzd

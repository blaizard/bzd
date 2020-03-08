#include "bzd/core/assert.h"
#include "bzd/core/system.h"
#include "bzd/core/registry.h"
#include "bzd/core/channel.h"

namespace bzd { namespace assert {
namespace impl {

class Stub : public bzd::OChannel
{
public:
	bzd::Expected<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override { return data.size(); }
};

bzd::OChannel& getOChannel()
{
	return bzd::Registry<bzd::OChannel>::getOrCreate<Stub>("stdout");
}

void backend(const bzd::StringView& message1, const bzd::StringView& message2)
{
	auto& out = getOChannel();
	out.write(bzd::Span<const UInt8Type>(reinterpret_cast<const unsigned char*>(message1.data()), message1.size()));
	if (message2.size())
	{
		out.write(bzd::Span<const UInt8Type>(reinterpret_cast<const unsigned char*>(message2.data()), message2.size()));
	}
	bzd::panic();
}
}

void unreachable()
{
	impl::backend("Code unreachable.\n");
}

}} // namespace bzd::assert

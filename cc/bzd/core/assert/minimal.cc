#include "bzd/core/assert/minimal.h"

#include "bzd/container/string_view.h"
#include "bzd/core/channel.h"
#include "bzd/core/registry.h"
#include "bzd/platform/system.h"

namespace bzd::assert::impl {

class Stub : public bzd::OChannel
{
public:
	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override { return data.size(); }
};

bzd::OChannel& getOChannel()
{
	return bzd::Registry<bzd::OChannel>::getOrCreate<Stub>("stdout");
}

void backend(const char* message1, const char* message2)
{
	auto& out = getOChannel();
	out.write(bzd::StringView{message1});
	if (message2)
	{
		out.write(bzd::StringView{message2});
	}
	bzd::platform::panic();
}
} // namespace bzd::assert::impl

namespace bzd::assert {
void unreachable()
{
	impl::backend("Code unreachable.\n");
}

} // namespace bzd::assert

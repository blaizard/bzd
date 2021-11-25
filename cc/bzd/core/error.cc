#include "cc/bzd/core/error.hh"

#include "cc/bzd/container/pool.hh"

namespace {
using StringType = bzd::String<64>;
bzd::Pool<StringType, 5> bufferPool{};
} // namespace

bzd::Error::~Error() noexcept
{
	message_.match(
		[](bzd::Optional<bzd::interface::String&> maybeBuffer) {
			if (maybeBuffer)
			{
				maybeBuffer->clear();
				bufferPool.release(reinterpret_cast<StringType&>(maybeBuffer.valueMutable()));
			}
		},
		[](bzd::StringView) {});
}

bzd::Optional<bzd::interface::String&> bzd::Error::reserveBuffer() noexcept
{
	if (bufferPool.empty())
	{
		return nullopt;
	}
	return bufferPool.reserve();
}
